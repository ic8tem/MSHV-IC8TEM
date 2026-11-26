# Riconoscimenti — Credits

| What | From | License | How it is used here |
|---|---|---|---|
| MSHV, the whole program | Hrisimir Hristov, LZ2HV — <https://sourceforge.net/projects/mshv/> | GPLv3 | base of this version |
| Decoders/encoders (JT6M, JT65, JTMS, ISCAT, MSK144, FT8, FT4, Q65, QRA64) and `astro()` used by Doppler tracking | Joe Taylor K1JT and the WSJT Development Group — <https://wsjt.sourceforge.io> | GPLv3 | already in MSHV |
| Q65 Pileup: callers list with Moon elevation (`get_q3list.f90`), list of codewords with both values of the spare bit 78 (`q65_set_list2`), `#` marks and the bit sent to the station being worked (WSJT-X 3.2.0-rc1); a priori list decoding fixes in `q65_check_llh()` | Joe Taylor K1JT and the WSJT Development Group — WSJT-X 2.7.0 and 3.2.0-rc1 — <https://wsjt.sourceforge.io> | GPLv3 | ported to C++ on top of the Pileup activity ported by LZ2HV in MSHV 2.72; callers window, Moon elevation from MSHV's own Moon routine, menu and right click are new |
| TCI client | Expert Electronics, modified by LZ2HV | MIT | already in MSHV |
| FlexRadio native VITA-49 backend | Manoj Kumar R, VU2CPL | GPLv3 | already in MSHV 2.76.7 |
| macOS port: PortAudio audio, menus/keys, CPU meter, fonts, `mac_complex_shim.h`, `mshv_thread_helper.h`, `mshv_txtrace.h`, Boost `clang.hpp`; fixes for FFT pointers and Flex tuner | Manoj Kumar R, VU2CPL — <https://github.com/vu2cpl/mshv-macos-port> | GPLv3 (Boost files: Boost Software License 1.0) | code merged |
| Icom LAN (RS-BA1) protocol: packet layout, login, token, CI-V and audio streams | wfview — Elliott Liggett W6EL, Phil Taylor M0VSE and contributors — <https://gitlab.com/eliggett/wfview> | GPLv3 | protocol reference; the C++ code in `src/HvRigControl/HvRigCat/icomlan/` is new |
| Icom LAN protocol (first open implementation) | kappanhang — Norbert Varga, HA2NON — <https://github.com/nonoo/kappanhang> | MIT | protocol reference |
| Icom LAN packet primitives | AetherSDR contributors — <https://github.com/aethersdr/AetherSDR> | GPLv3 | protocol reference (credential table, packet sizes) |
| IC-7300MK2 LAN behaviour (48 kHz LPCM only, big-endian inner sequence for token renewal) and a synthetic MK2 used to test this code | Simon S53ZO — IC7300MK2_Lan_Splitter <https://github.com/s53zo/IC7300MK2_Lan_Splitter> and PiRO <https://github.com/s53zo/piro> | AGPLv3 / GPLv3 | reference and local test only; no code included |
| Hamlib library (`libhamlib-4.dll`, Windows packages) for "Icom IC-7300 / IC-7610 / IC-9700 (Hamlib)" | The Hamlib Group — <https://hamlib.github.io> | LGPL v2.1 or later | DLL shipped unchanged in the `hamlib` folder, loaded at run time; licence file next to it |
| MSK144 impulse blanker (menu Decode › MSK144 Noise Blanker (NR0V NB2)) | approach of the "NB2" noise blanker of WDSP — Warren Pratt, NR0V — <https://github.com/TAPR/OpenHPSDR-wdsp> | GPL v2 or later | method reused (detection against the average level, advance/hang time, raised-cosine slews); the C++ code in `src/HvDecoderMs/hvmsknoise.h` is new, changed for meteor scatter (pings never blanked) |
| MSK144 linear prediction blanker (menu Decode › MSK144 Noise Blanker LPC (DL2FW)) | `alt_noise_blanking()` — Michael Wild, DL2FW — UHSDR firmware <https://github.com/df8oe/UHSDR> | GPLv3 | ported to double precision and plain C++ in `src/HvDecoderMs/hvmsknoise.h`, robust threshold for digital signals |
| LoTW user activity list (`lotw-user-activity.csv`) | ARRL — <https://lotw.arrl.org> | data file, downloaded by the user | read at run time |
| Terrestrial `CALL3.TXT` for the locator lookup | WSJT-X Improved — Uwe Risse, DG2YCB — <https://sourceforge.net/projects/wsjt-x-improved/> | data file | downloaded at run time (every 14 days), read only |
| EME `CALL3.TXT` for the locator lookup | Lance Collister, W7GJ — <https://www.bigskyspaces.com/w7gj/> | data file | downloaded at run time (every 14 days), read only |
| `grid_data.bin` (call → locator) for the locator lookup | JTDX team — <https://sourceforge.net/projects/jtdx/> | data file | downloaded at run time (every 14 days), read only; format as read by Decodium `UsStateDataManager` (GPLv3) |
| Online locator lookup | G7VRD public APIs — <https://g7vrd.co.uk/public-apis> | free hobby service (beta) | asked only when no other source has the locator; answers kept 30 days |
| "Calls that heard me" (underline of the stations that received you) | idea from WsjtxUtils.Searchlight — KC3PIB — <https://github.com/KC3PIB/WsjtxUtils.Searchlight> | MIT | idea only; the C++ code in `src/HvTxW/hvpskheard.*` is new |
| Reception reports for "Calls that heard me" | PSKReporter — Philip Gladstone, N1DQ — <https://pskreporter.info> | free service | asked at run time (`retrieve.pskreporter.info`), at most every 5 minutes and only while transmitting |
| Windows installer | Inno Setup — Jordan Russell and Martijn Laan | Inno Setup License | build tool |
| AppImage | linuxdeploy and Qt plugin — <https://github.com/linuxdeploy> | MIT | build tool |

Original notices are kept in every file. If you see your work here without the right credit, please tell IC8TEM and it will be fixed.

See also the License section of the README.
