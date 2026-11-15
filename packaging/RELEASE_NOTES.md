**MSHV-IC8TEM** — modified version of MSHV 2.76.7 by LZ2HV, with additions by IC8TEM.
Not the official MSHV: please report problems with this version to IC8TEM.

**ic8tem.8:** Icom LAN — one entry for all radios (model detected automatically), CI-V and audio UDP ports can be set.

**ic8tem.7:** macOS start-up fixed (a library was missing from the app).

**ic8tem.6: automatic updates** — every 6 hours MSHV checks GitHub and installs new versions with one click (Help › Check for Updates). From the next version on, updates arrive by themselves.

**ic8tem.5:** LoTW list downloaded directly from ARRL (button, link https://lotw.arrl.org/lotw-user-activity.csv) · Italian wiki.

**ic8tem.4: Icom LAN** — IC-7300MK2 (and IC-705, IC-9700, IC-7610, IC-905) controlled over the network with CAT and audio, no USB cable: Rig Control → *Icom IC-7300MK2 LAN*, Sound → *Icom LAN Input/Output*. Guide: docs/ICOM_LAN.md in the source.

**New:** Doppler tracking in *Astronomical Data* (Full Doppler, Own Echo, **CFOM**, On DX Echo, Call DX; Tx Shift ±999.999 MHz, rig set via CAT) · LoTW users list (*Network Configuration → Page 3*), black dot and *Show LoTW Users Only* filter · *Fictitious log reset* in the Log window · optional *Unlock TX Slots* (OFF by default) · MSK144 Width (ping duration) column, optional · default mode buttons FT8, FT4, Q65B, Q65C, Q65A, MSK144, JT65B · fixes from VU2CPL (FFT pointers crash, Flex tuner keyed by Enter).

| System | File |
|---|---|
| Windows 64 bit | **`…-windows-x64-setup.exe`** (installer) or `…-windows-x64.zip` (portable) |
| Windows 32 bit | **`…-windows-x86-setup.exe`** or `…-windows-x86.zip` |
| Windows ARM64 (Snapdragon) | **`…-windows-arm64-setup.exe`** or `…-windows-arm64.zip` |
| Linux x86_64 / aarch64, any distribution | `…-linux-<arch>.AppImage` (self-contained) or `…-linux-<arch>.tar.gz` (system Qt5) |
| Debian / Ubuntu / Mint | `…-debian-ubuntu-mint-<arch>.deb` |
| Fedora / openSUSE | `…-fedora-opensuse-x86_64.rpm` |
| Linux i686 (32 bit) | `…-linux-i686.tar.gz` / `…-debian-ubuntu-mint-i386.deb` |
| Raspberry Pi OS 64 bit | `…-raspios-64bit-arm64.deb` |
| Raspberry Pi OS 32 bit (Pi 2 and later) | `…-raspios-32bit-armhf.deb` |
| Linux armv7l | `…-linux-armv7l-armhf.tar.gz` |
| Slackware ARM64 (Raspberry Pi) | `mshv-ic8tem-…-aarch64-1_ic8tem.txz` (installpkg) |
| FreeBSD amd64 | `…-freebsd-amd64.tar.gz` (see README-UNIX.txt for `pkg install`) |
| OpenBSD amd64 | `…-openbsd-amd64.tar.gz` (see README-UNIX.txt for `pkg_add`) |
| macOS Apple Silicon / Intel | `…-macos-<arch>.dmg` (unsigned: right-click → Open the first time) |

The Windows installer needs no administrator rights (it installs in `%LOCALAPPDATA%\Programs\MSHV-IC8TEM`) and keeps your settings and logs when you update or uninstall; Windows SmartScreen may warn because the installer is not code-signed (*More info → Run anyway*). Installed Linux/macOS packages keep your data in `~/.local/share/MSHV-IC8TEM` (Linux) or `~/Library/Application Support/MSHV-IC8TEM` (macOS). The portable folders (zip / tar.gz) keep it inside the folder.

License: GNU GPL v3. MSHV © LZ2HV; WSJT algorithms © K1JT and the WSJT Development Group; macOS port code © VU2CPL; modifications © 2026 IC8TEM. `SHA256SUMS.txt` lists the checksums.

---
*Italiano:* versione modificata di MSHV 2.76.7 (LZ2HV) con Doppler tracking/CFOM via CAT, lista LoTW con pallino e filtro, annullamento fittizio del log, sblocco TX Slots opzionale. Scegli il file per il tuo sistema dalla tabella sopra; la guida in italiano è `LEGGIMI_DOPPLER_IC8TEM.txt`.
