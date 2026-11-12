**MSHV-IC8TEM** — modified version of MSHV 2.76.7 by LZ2HV, with additions by IC8TEM.
Not the official MSHV: please report problems with this version to IC8TEM.

**New:** Doppler tracking in *Astronomical Data* (Full Doppler, Own Echo, **CFOM**, On DX Echo, Call DX; Tx Shift ±999.999 MHz, rig set via CAT) · LoTW users list (*Network Configuration → Page 3*), black dot and *Show LoTW Users Only* filter · *Fictitious log reset* in the Log window · optional *Unlock TX Slots* (OFF by default) · default mode buttons FT8, FT4, Q65B, Q65C, Q65A, MSK144, JT65B · fixes from VU2CPL (FFT pointers crash, Flex tuner keyed by Enter).

| System | File |
|---|---|
| Windows 64 bit | `…-windows-x64.zip` (unzip, run `MSHV-IC8TEM.exe`) |
| Windows 32 bit | `…-windows-x86.zip` |
| Windows ARM64 (Snapdragon) | `…-windows-arm64.zip` |
| Linux x86_64 / aarch64, any distribution | `…-linux-<arch>.AppImage` (self-contained) or `…-linux-<arch>.tar.gz` (system Qt5) |
| Debian / Ubuntu / Mint | `…-debian-ubuntu-mint-<arch>.deb` |
| Fedora / openSUSE | `…-fedora-opensuse-x86_64.rpm` |
| Linux i686 (32 bit) | `…-linux-i686.tar.gz` / `…-debian-ubuntu-mint-i386.deb` |
| Raspberry Pi OS 64 bit | `…-raspios-64bit-arm64.deb` |
| Raspberry Pi OS 32 bit (Pi 2 and later) | `…-raspios-32bit-armhf.deb` |
| Linux armv7l | `…-linux-armv7l-armhf.tar.gz` |
| Slackware ARM64 (Raspberry Pi) | `mshv-ic8tem-…-aarch64-1_ic8tem.txz` (installpkg) |
| FreeBSD amd64 | `…-freebsd-amd64.tar.gz` (see README-UNIX.txt for `pkg install`) |
| macOS Apple Silicon / Intel | `…-macos-<arch>.dmg` (unsigned: right-click → Open the first time) |

Installed packages keep your data in `~/.local/share/MSHV-IC8TEM` (Linux) or `~/Library/Application Support/MSHV-IC8TEM` (macOS). The portable folders (zip / tar.gz) keep it inside the folder.

License: GNU GPL v3. MSHV © LZ2HV; WSJT algorithms © K1JT and the WSJT Development Group; modifications © 2026 IC8TEM. `SHA256SUMS.txt` lists the checksums.

---
*Italiano:* versione modificata di MSHV 2.76.7 (LZ2HV) con Doppler tracking/CFOM via CAT, lista LoTW con pallino e filtro, annullamento fittizio del log, sblocco TX Slots opzionale. Scegli il file per il tuo sistema dalla tabella sopra; la guida in italiano è `LEGGIMI_DOPPLER_IC8TEM.txt`.
