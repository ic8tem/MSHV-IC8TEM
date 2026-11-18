/* MSHV-IC8TEM  Noise tools for the MSK144 decoder (menu Decode > MSK144 Noise).
 *
 * 1) Impulse blanker on the analytic (complex) signal, after the approach of the
 *    "NB2" noise blanker of WDSP by Warren Pratt NR0V (GPL v2 or later):
 *    impulses found against the average level, blanked with advance and hang
 *    guard times and raised-cosine slews.
 *    Changes for meteor scatter: the level is the median of 4 ms blocks around
 *    the impulse, events whose core is longer than 3 ms are left untouched
 *    (pings, long crashes) and the level just before and after the event must
 *    be low too (so the start and the end of a ping are never taken for an
 *    impulse).
 *
 * 2) Linear prediction blanker: port of alt_noise_blanking() by Michael Wild
 *    DL2FW, UHSDR firmware (mcHF), GNU GPLv3: LPC coefficients (Levinson-Durbin),
 *    inverse filter + matched filter to find the impulses, impulse samples
 *    replaced by the weighted forward and backward linear prediction.
 *    Changes: double precision, plain C++ instead of ARM CMSIS, robust threshold
 *    (median absolute deviation, needed with noise-like digital signals),
 *    11-sample repair window at 12 kHz, only short isolated impulses.
 *
 * 3) Carrier notch ("ZAP" for MSK144): like the ZAP of the old WSJT, birdies are
 *    narrow lines steady in time. The median over time of 0.17 s spectra
 *    ignores the pings; a line well above the nearby spectrum is removed with a
 *    10 Hz notch. Groups of evenly spaced lines (a continuous MSK signal) are
 *    never notched.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVMSKNOISE_H
#define HVMSKNOISE_H

#ifdef complex                       // the decoders define complex as _Complex
#pragma push_macro("complex")
#undef complex
#define HVMSKNOISE_POP_COMPLEX
#endif
#include <cmath>
#include <vector>
#include <algorithm>
#ifdef HVMSKNOISE_POP_COMPLEX
#pragma pop_macro("complex")
#undef HVMSKNOISE_POP_COMPLEX
#endif

namespace HvMskNoise
{
static const double PI_ = 3.14159265358979323846;

static inline double Median(std::vector<double> &v) // reorders v
{
    if (v.empty()) return 0.0;
    size_t m = v.size()/2;
    std::nth_element(v.begin(), v.begin()+m, v.end());
    return v[m];
}

// ------------------------------------------------------------------ 1) impulse blanker
// iq = interleaved complex samples (re,im), n = number of complex samples.
// Returns the number of impulses blanked.
static inline int BlankImpulses(double *iq,int n,double fs = 12000.0,double kthr = 5.0)
{
    const int BL    = (int)(0.004*fs);   // 4 ms blocks
    const int GAP   = (int)(0.001*fs);   // closer than 1 ms = same event
    const int DMAX  = (int)(0.003*fs);   // core (-12 dB) longer than this: a ping, left untouched
    const int BMAX  = (int)(0.015*fs);   // never blank more than 15 ms (a long crash)
    const int GUARD = (int)(0.0005*fs);  // 0.5 ms advance and hang time
    const int SLEW  = (int)(0.0005*fs);  // 0.5 ms raised-cosine slews
    const int SIDE  = (int)(0.004*fs);   // level around the event, 4 ms each side
    if (BL < 4 || n < BL*12) return 0;

    std::vector<double> env(n);
    for (int i = 0; i < n; ++i) env[i] = std::sqrt(iq[2*i]*iq[2*i] + iq[2*i+1]*iq[2*i+1]);

    int nb = n/BL;
    std::vector<double> bmed(nb), ref(nb), tmp;
    tmp.reserve(SIDE+BL);
    for (int b = 0; b < nb; ++b)
    {
        tmp.assign(env.begin()+b*BL, env.begin()+(b+1)*BL);
        bmed[b] = Median(tmp);
    }
    for (int b = 0; b < nb; ++b)
    {
        int a = std::max(0,b-4), z = std::min(nb-1,b+4);
        tmp.assign(bmed.begin()+a, bmed.begin()+z+1);
        ref[b] = Median(tmp);
    }

    int nev = 0;
    int i = 0;
    while (i < n)
    {
        int b = std::min(i/BL,nb-1);
        if (!(ref[b] > 0.0 && env[i] > kthr*ref[b]))
        {
            ++i;
            continue;
        }
        int s = i, e = i;
        double peak = env[i];
        for (int j = i+1; j < n && j-e <= GAP; ++j)
        {
            int bj = std::min(j/BL,nb-1);
            if (env[j] > kthr*ref[bj])
            {
                e = j;
                if (env[j] > peak) peak = env[j];
            }
        }
        i = e+1;
        // width of the core (above peak/4): an impulse is short even when its filter ringing is long
        int c0 = s, c1 = e;
        while (c0 < e && env[c0] < 0.25*peak) ++c0;
        while (c1 > s && env[c1] < 0.25*peak) --c1;
        if (c1-c0+1 > DMAX || e-s+1 > BMAX) continue;   // a ping or a long crash: keep it

        // the level just before and after must be low too (not the edge of a ping)
        tmp.clear();
        for (int k = std::max(0,s-GAP-SIDE); k < std::max(0,s-GAP); ++k) tmp.push_back(env[k]);
        double lv = Median(tmp);
        tmp.clear();
        for (int k = std::min(n,e+GAP+1); k < std::min(n,e+GAP+1+SIDE); ++k) tmp.push_back(env[k]);
        double rv = Median(tmp);
        double side = std::max(lv,rv);
        if (side <= 0.0 || peak < kthr*side) continue;

        int a = s-GUARD, z = e+GUARD;
        for (int k = 0; k < SLEW; ++k)       // slew down
        {
            int idx = a-SLEW+k;
            if (idx < 0 || idx >= n) continue;
            double w = 0.5 + 0.5*std::cos(PI_*(k+1)/(SLEW+1));
            iq[2*idx] *= w;
            iq[2*idx+1] *= w;
        }
        for (int idx = std::max(0,a); idx <= std::min(n-1,z); ++idx)
        {
            iq[2*idx] = 0.0;
            iq[2*idx+1] = 0.0;
        }
        for (int k = 0; k < SLEW; ++k)       // slew up
        {
            int idx = z+1+k;
            if (idx < 0 || idx >= n) continue;
            double w = 0.5 - 0.5*std::cos(PI_*(k+1)/(SLEW+1));
            iq[2*idx] *= w;
            iq[2*idx+1] *= w;
        }
        ++nev;
    }
    return nev;
}

// ------------------------------------------------------------------ 2) linear prediction blanker (DL2FW)
// x = real audio samples (12 kHz), repaired in place. Returns the number of impulses repaired.
static inline int LpcBlank(double *x,int n,double kthr = 10.0)
{
    const int ORD = 10;          // LPC order (DL2FW)
    const int FR = 128;          // frame (DL2FW)
    const int PL = 5;            // repair 2*PL+1 = 11 samples (0.9 ms at 12 kHz)
    const int IL = 2*PL+1;
    const int MAXIMP = 5;        // max impulses per frame (DL2FW)
    if (n < 2*FR) return 0;

    std::vector<double> e(FR+ORD), m(FR), tmp(FR);
    int nimp = 0;
    for (int s = 0; s+FR <= n; s += FR)
    {
        // autocorrelation and Levinson-Durbin
        double R[ORD+1];
        for (int l = 0; l <= ORD; ++l)
        {
            double acc = 0.0;
            for (int t = 0; t < FR-l; ++t) acc += x[s+t]*x[s+t+l];
            R[l] = acc;
        }
        if (!(R[0] > 0.0)) continue;
        R[0] *= (1.0+1.0e-9);
        double a[ORD+1], any[ORD+1];
        a[0] = 1.0;
        for (int k = 1; k <= ORD; ++k) a[k] = 0.0;
        double alfa = R[0];
        bool ok = true;
        for (int mm = 1; mm <= ORD; ++mm)
        {
            double sum = 0.0;
            for (int u = 1; u < mm; ++u) sum += a[u]*R[mm-u];
            double k = -(R[mm]+sum)/alfa;
            for (int v = 1; v < mm; ++v) any[v] = a[v] + k*a[mm-v];
            for (int w = 1; w < mm; ++w) a[w] = any[w];
            a[mm] = k;
            alfa *= (1.0-k*k);
            if (!(alfa > 0.0))
            {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        // inverse filter (prediction error), then matched filter: peak delayed by ORD
        for (int t = -ORD; t < FR; ++t)
        {
            double acc = 0.0;
            for (int j = 0; j <= ORD; ++j)
            {
                int idx = s+t-j;
                if (idx >= 0 && idx < n) acc += a[j]*x[idx];
            }
            e[t+ORD] = acc;
        }
        for (int t = 0; t < FR; ++t)
        {
            double acc = 0.0;
            for (int j = 0; j <= ORD; ++j)
            {
                int ti = t-j;
                if (ti >= -ORD) acc += a[ORD-j]*e[ti+ORD];
            }
            m[t] = acc;
        }

        // robust level of the matched filter output (median absolute deviation)
        tmp.assign(m.begin(), m.end());
        double med = Median(tmp);
        for (int t = 0; t < FR; ++t) tmp[t] = std::fabs(m[t]-med);
        double sig = 1.4826*Median(tmp);
        if (!(sig > 0.0)) continue;
        double thr = kthr*sig;

        int cnt = 0;
        for (int t = ORD; t < FR && cnt < MAXIMP; ++t)
        {
            if (std::fabs(m[t]-med) <= thr) continue;
            // only short isolated impulses: the excess must end within the repair window
            int r0 = t, r1 = t;
            while (r0 > 0 && std::fabs(m[r0-1]-med) > 0.5*thr) --r0;
            while (r1 < FR-1 && std::fabs(m[r1+1]-med) > 0.5*thr) ++r1;
            if (r1-r0+1 > IL)
            {
                t = r1;
                continue;
            }
            int c = s+t-ORD;                     // impulse centre (matched filter delay)
            int first = c-PL, last = c+PL;
            if (first-ORD < 0 || last+ORD >= n)
            {
                t += PL;
                continue;
            }
            double fw[ORD+IL], bw[IL+ORD];
            for (int k = 0; k < ORD; ++k) fw[k] = x[first-ORD+k];
            for (int q = 0; q < IL; ++q)         // forward prediction
            {
                double acc = 0.0;
                for (int j = 1; j <= ORD; ++j) acc -= a[j]*fw[ORD+q-j];
                fw[ORD+q] = acc;
            }
            for (int k = 0; k < ORD; ++k) bw[IL+k] = x[last+1+k];
            for (int q = IL-1; q >= 0; --q)      // backward prediction
            {
                double acc = 0.0;
                for (int j = 1; j <= ORD; ++j) acc -= a[j]*bw[q+j];
                bw[q] = acc;
            }
            for (int q = 0; q < IL; ++q)         // weighted sum (DL2FW)
            {
                double wb = (double)q/(double)(IL-1);
                x[first+q] = (1.0-wb)*fw[ORD+q] + wb*bw[q];
            }
            ++cnt;
            ++nimp;
            t += PL;
        }
    }
    return nimp;
}

// ------------------------------------------------------------------ 3) carrier notch
// radix-2 complex FFT in place, interleaved (re,im), forward
static inline void Fft(double *a,int n)
{
    for (int i = 1, j = 0; i < n; ++i)
    {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j)
        {
            std::swap(a[2*i],a[2*j]);
            std::swap(a[2*i+1],a[2*j+1]);
        }
    }
    for (int len = 2; len <= n; len <<= 1)
    {
        double ang = -2.0*PI_/len;
        double wr = std::cos(ang), wi = std::sin(ang);
        for (int i = 0; i < n; i += len)
        {
            double cr = 1.0, ci = 0.0;
            for (int k = 0; k < len/2; ++k)
            {
                int p = i+k, q = i+k+len/2;
                double tr = a[2*q]*cr - a[2*q+1]*ci;
                double ti = a[2*q]*ci + a[2*q+1]*cr;
                a[2*q] = a[2*p]-tr;
                a[2*q+1] = a[2*p+1]-ti;
                a[2*p] += tr;
                a[2*p+1] += ti;
                double ncr = cr*wr - ci*wi;
                ci = cr*wi + ci*wr;
                cr = ncr;
            }
        }
    }
}

static const int ZAP_N = 2048;       // 5.86 Hz bins at 12 kHz, 0.17 s
static const int ZAP_RING = 32;      // real time decoding: last 32 slices (about 5.5 s)
static const int ZAP_MAXL = 8;
static const double ZAP_TONE = 12.0;   // MSK tone metric (median over time) above which no notch

struct ZapState
{
    std::vector<double> ring;        // ZAP_RING slices x nbin
    std::vector<double> tring;       // MSK tone metric of the slices
    int nbin;
    int nsl;
    int pos;
    ZapState() : nbin(0), nsl(0), pos(0) {}
    void Reset()
    {
        ring.clear();
        tring.clear();
        nbin = 0;
        nsl = 0;
        pos = 0;
    }
};

// normalized power spectra (300..2700 Hz) of the 2048-sample slices of x, and for each slice
// the MSK tone metric: MSK squared has lines at twice its tones (2000 and 4000 Hz)
static inline int ZapSlices(const double *x,int n,double fs,std::vector<double> &out,std::vector<double> &tone,
                            int &kb0,int &nbin)
{
    kb0 = (int)(300.0*ZAP_N/fs);
    int kb1 = (int)(2700.0*ZAP_N/fs);
    nbin = kb1-kb0+1;
    int ns = n/ZAP_N;
    out.assign((size_t)ns*nbin,0.0);
    tone.assign(ns,0.0);
    std::vector<double> a(2*ZAP_N), tmp(nbin), sq;
    int q0 = (int)(1000.0*ZAP_N/fs), q1 = (int)(5000.0*ZAP_N/fs);
    for (int s = 0; s < ns; ++s)
    {
        // squared signal: MSK tones
        for (int t = 0; t < ZAP_N; ++t)
        {
            double w = 0.5 - 0.5*std::cos(2.0*PI_*t/ZAP_N);
            double v = x[s*ZAP_N+t];
            a[2*t] = w*v*v;
            a[2*t+1] = 0.0;
        }
        Fft(&a[0],ZAP_N);
        sq.resize(q1-q0+1);
        for (int k = q0; k <= q1; ++k) sq[k-q0] = a[2*k]*a[2*k] + a[2*k+1]*a[2*k+1];
        std::vector<double> sqc(sq);
        double sqm = Median(sqc);
        // both lines, 2000 Hz apart (a single carrier gives only one line)
        double p2 = 0.0, p4 = 0.0;
        int k2 = -1, k4 = -1;
        for (int k = q0; k <= q1; ++k)
        {
            double f = k*fs/ZAP_N;
            double v = sq[k-q0];
            if (std::fabs(f-2000.0) <= 420.0 && v > p2) { p2 = v; k2 = k; }
            if (std::fabs(f-4000.0) <= 420.0 && v > p4) { p4 = v; k4 = k; }
        }
        double dsep = (k2 >= 0 && k4 >= 0) ? std::fabs((k4-k2)*fs/ZAP_N - 2000.0) : 1.0e9;
        tone[s] = (sqm > 0.0 && dsep <= 18.0) ? std::min(p2,p4)/sqm : 0.0;

        for (int t = 0; t < ZAP_N; ++t)
        {
            double w = 0.5 - 0.5*std::cos(2.0*PI_*t/ZAP_N);
            a[2*t] = w*x[s*ZAP_N+t];
            a[2*t+1] = 0.0;
        }
        Fft(&a[0],ZAP_N);
        double *o = &out[(size_t)s*nbin];
        for (int k = 0; k < nbin; ++k)
        {
            double re = a[2*(kb0+k)], im = a[2*(kb0+k)+1];
            o[k] = re*re + im*im;
        }
        tmp.assign(o,o+nbin);
        double med = Median(tmp);
        if (med > 0.0) for (int k = 0; k < nbin; ++k) o[k] /= med;
    }
    return ns;
}

// Finds the steady narrow lines (Hz). st = 0: from this buffer only (full period),
// st != 0: real time decoding, the slices are added to the ring of the last ~5.5 s.
static inline int ZapFindLines(const double *x,int n,double fs,ZapState *st,double *lines)
{
    std::vector<double> sl, tn;
    int kb0 = 0, nbin = 0;
    int ns = ZapSlices(x,n,fs,sl,tn,kb0,nbin);
    const double *src = 0;
    std::vector<double> tones;
    int nsl = 0;
    if (st)
    {
        if (st->nbin != nbin)
        {
            st->Reset();
            st->nbin = nbin;
            st->ring.assign((size_t)ZAP_RING*nbin,0.0);
            st->tring.assign(ZAP_RING,0.0);
        }
        for (int s = 0; s < ns; ++s)
        {
            std::copy(&sl[(size_t)s*nbin], &sl[(size_t)s*nbin]+nbin, &st->ring[(size_t)st->pos*nbin]);
            st->tring[st->pos] = tn[s];
            st->pos = (st->pos+1) % ZAP_RING;
            if (st->nsl < ZAP_RING) st->nsl++;
        }
        src = &st->ring[0];
        nsl = st->nsl;
        tones.assign(st->tring.begin(), st->tring.begin()+nsl);
    }
    else
    {
        src = sl.empty() ? 0 : &sl[0];
        nsl = ns;
        tones = tn;
    }
    if (nsl < 12 || nbin < 40) return 0;
    // an MSK signal present most of the time (tropo, Es): its spectrum is made of steady
    // lines too, so no notch at all
    if (Median(tones) > ZAP_TONE) return 0;

    // median over time of each bin: the pings do not count
    std::vector<double> M(nbin), col(nsl), R(nbin), tmp;
    for (int k = 0; k < nbin; ++k)
    {
        for (int s = 0; s < nsl; ++s) col[s] = src[(size_t)s*nbin+k];
        M[k] = Median(col);
    }
    for (int k = 0; k < nbin; ++k)
    {
        int a = std::max(0,k-8), z = std::min(nbin-1,k+8);
        tmp.assign(M.begin()+a, M.begin()+z+1);
        R[k] = Median(tmp);
    }
    double fl[64], pw[64];
    int nl = 0;
    for (int k = 1; k < nbin-1 && nl < 64; ++k)
    {
        if (!(R[k] > 0.0) || M[k] <= 8.0*R[k] || M[k] < M[k-1] || M[k] < M[k+1]) continue;
        // narrow on both sides (not the edge of a wide signal)
        if (k < 3 || k > nbin-4 || M[k] < 4.0*M[k-3] || M[k] < 4.0*M[k+3]) continue;
        // not a tooth of a comb: MSK144 frames repeat every 72 ms (lines 13.9 Hz apart),
        // short messages every 20 ms (50 Hz): a carrier has only noise there
        bool comb = false;
        const double sp[2] = {fs/864.0, 50.0};
        for (int q = 0; q < 2 && !comb; ++q)
            for (int sg = -1; sg <= 1; sg += 2)
            {
                double kk = k + sg*sp[q]*ZAP_N/fs;
                int k0 = (int)std::floor(kk);
                if (k0 < 0 || k0+1 >= nbin) continue;
                double fr = kk-k0;
                double v = (1.0-fr)*M[k0] + fr*M[k0+1];
                if (v > 3.0*R[k]) comb = true;
            }
        if (comb) continue;
        double den = M[k-1] - 2.0*M[k] + M[k+1];
        double d = (den != 0.0) ? 0.5*(M[k-1]-M[k+1])/den : 0.0;
        if (d > 0.5) d = 0.5;
        if (d < -0.5) d = -0.5;
        fl[nl] = (kb0+k+d)*fs/ZAP_N;
        pw[nl] = M[k]/R[k];
        ++nl;
    }
    if (nl == 0) return 0;
    // evenly spaced lines (13.9 Hz MSK144 frames, 50 Hz short messages) = a signal, not birdies
    int npair = 0;
    for (int p = 0; p < nl; ++p)
        for (int q = p+1; q < nl; ++q)
        {
            double dlt = std::fabs(fl[q]-fl[p]);
            if (dlt > 600.0) continue;
            double r50 = std::fabs(dlt - 50.0*std::floor(dlt/50.0+0.5));
            double r14 = std::fabs(dlt - (12000.0/864.0)*std::floor(dlt/(12000.0/864.0)+0.5));
            if (r50 < 1.5 || r14 < 1.0) ++npair;
        }
    if (npair >= 2 || nl > ZAP_MAXL) return 0;
    // strongest first
    for (int p = 0; p < nl; ++p)
        for (int q = p+1; q < nl; ++q)
            if (pw[q] > pw[p])
            {
                std::swap(pw[p],pw[q]);
                std::swap(fl[p],fl[q]);
            }
    for (int p = 0; p < nl; ++p) lines[p] = fl[p];
    return nl;
}

// spec = interleaved complex spectrum of nfft points (positive bins used), 10 Hz notch per line
static inline void ZapApply(double *spec,int nfft,double fs,const double *lines,int nl,double halfw = 5.0)
{
    double df = fs/nfft;
    int nh = nfft/2;
    for (int l = 0; l < nl; ++l)
    {
        int ia = (int)std::floor((lines[l]-halfw)/df);
        int ib = (int)std::ceil((lines[l]+halfw)/df);
        if (ia < 1) ia = 1;
        if (ib > nh) ib = nh;
        for (int k = ia; k <= ib; ++k)
        {
            spec[2*k] = 0.0;
            spec[2*k+1] = 0.0;
        }
    }
}
}
#endif
