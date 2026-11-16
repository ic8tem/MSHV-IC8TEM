# MSHV-IC8TEM

Modified version of **MSHV 2.76.7** by Hrisimir Hristov, LZ2HV, with additions by **Costantino, IC8TEM**.

> This is **not** the official MSHV. Please report problems with this version to IC8TEM, not to LZ2HV.
> Official MSHV: <https://sourceforge.net/projects/mshv/>

## What is added

| Area | Addition |
|---|---|
| Astronomical Data | **Doppler tracking** panel like WSJT-X: Full Doppler to DX Grid, Own Echo, **Constant Frequency on Moon (CFOM)**, On DX Echo, Call DX, None. Sked frequency, Set kHz, live Rx/Tx frequencies, **Tx Shift up to ±999.999 MHz**. Frequencies are sent to the rig via CAT (TX frequency set before PTT, RX frequency after PTT off). |
| LoTW | Options → Network Configuration → **Page 3**: **Download from ARRL and Load** button, or the link <https://lotw.arrl.org/lotw-user-activity.csv> and *Load*; choose the minimum last-upload date. A **black dot** at the right edge of the message column marks decodes from LoTW users. |
| Decode List Filters | New **Show LoTW Users Only** option (messages addressed to you are always shown). |
| Log | **Fictitious log reset YYYYMMDDHHmmSS (UTC)**: QSOs before that time are ignored for worked-before calls/locators (new squares) and dupe check. The log is not modified. |
| Multi Answer | Optional **Unlock TX Slots** checkbox, **ON by default** since ic8tem.13 (untick it for the original LZ2HV limits). |
| **Icom LAN** | Native network connection to Icom radios with a LAN port — **IC-7300MK2**, IC-705, IC-9700, IC-7610, IC-905 — with **CAT and audio (RX and TX) over the network**, no USB cable, no RS-BA1, no wfview. Rig Control → `Icom LAN` (one entry, model detected from the CI-V address; address, ports 50001/50002/50003 editable, user, password); Sound → `Icom LAN Input` / `Icom LAN Output`. 48 kHz audio, resampled to 44.1 kHz for FSK441, JTMS, JT6M, ISCAT. See [docs/ICOM_LAN.md](docs/ICOM_LAN.md). |
| Power & SWR | Options → **View Power and SWR**: RF power (slider, presets, per-band and per-mode memory), Po / SWR / ALC meters while transmitting, tune at reduced power, SWR protection that stops and blocks TX. Icom CI-V on USB and Icom LAN. Translated in the MSHV languages. |
| Automatic updates | Every 6 hours (and Help → **Check for Updates**) MSHV asks GitHub for a newer release; **Update now** downloads the right package, checks its SHA256, closes, installs and restarts. Settings and logs are kept. Windows installer/zip, macOS, AppImage, tar.gz; .deb/.rpm/.txz through the system password (pkexec). Can be turned off. |
| MSK144 ping duration | Decode List menu → **Show/Hide Width (Ping Duration) Column MSK144**: duration of each ping in ms (20 ms steps), as in MSK144ms. FSK441, JTMS and JT6M already have the Width column. |
| Mode switcher | Default mode buttons: FT8, FT4, Q65B, Q65C, Q65A, MSK144, JT65B. |
| Fix | `astro()` used a non-initialized local array for the DX Doppler (`xdop[2]`); now `static`. |

Documentation (Italian): **[Wiki](../../wiki)** — the pages are also in the [wiki/](wiki/) folder.

Every change in the source is marked with the comment `IC8TEM mod`. The full list is in [CHANGES_IC8TEM.md](CHANGES_IC8TEM.md).

## Downloads

Ready-made packages for Windows (x64, x86, ARM64), Linux (AppImage, tar.gz, .deb, .rpm; x86_64, i686, aarch64, armhf/armv7l), Raspberry Pi OS 32/64 bit, Slackware ARM64, FreeBSD and macOS are on the [Releases](../../releases) page.

## Building

* **GitHub Actions**: changing `MSHV_VERSION` in `.github/workflows/build-all.yml` on `main` builds every package and publishes the release `v<MSHV_VERSION>` automatically. *Actions* → *Build all distributions* → *Run workflow* only builds (no release).
* **Windows, local**: run `compila.bat` (installs MSYS2 + Qt5 in `C:\msys64` the first time, then builds into `bin\`).
* **Linux / BSD, local**: install Qt5 (widgets, network, websockets), ALSA, PulseAudio, FFTW3 development packages, then `sh packaging/linux/build_unix.sh MSHV_x86_64.pro`.
* Original instructions by LZ2HV: `README_HOW_TO_COMPILE.txt`.

## License

MSHV is free software released under the **GNU General Public License v3** (see [COPYING.txt](COPYING.txt)); this modified version is distributed under the same license.

* MSHV — Copyright © Hrisimir Hristov, LZ2HV.
* JT6M, JT9, JT65, JTMS, QRA64, ISCAT, MSK144, FT8, FT4, Q65 algorithms and code — Copyright © 2001-2025 Joseph Taylor, K1JT, and the WSJT Development Group (GPLv3).
* macOS support (PortAudio audio, Mac menus/keys, CPU meter, fixes) — code from the macOS port by Manoj Kumar R, VU2CPL (<https://github.com/vu2cpl/mshv-macos-port>, GPLv3); its FFT-pointer and Flex-tuner fixes are also used on every platform.
* FFTW — GPL; qextserialport — MIT; Qt — LGPLv3 (dynamically linked); PortAudio — MIT.
* Modifications — Copyright © 2026 Costantino, IC8TEM, released under GPLv3.

## Credits — code and knowledge reused

| What | From | License | How it is used here |
|---|---|---|---|
| MSHV, the whole program | Hrisimir Hristov, LZ2HV — <https://sourceforge.net/projects/mshv/> | GPLv3 | base of this version |
| Decoders/encoders (JT6M, JT65, JTMS, ISCAT, MSK144, FT8, FT4, Q65, QRA64) and `astro()` used by Doppler tracking | Joe Taylor K1JT and the WSJT Development Group — <https://wsjt.sourceforge.io> | GPLv3 | already in MSHV |
| TCI client | Expert Electronics, modified by LZ2HV | MIT | already in MSHV |
| FlexRadio native VITA-49 backend | Manoj Kumar R, VU2CPL | GPLv3 | already in MSHV 2.76.7 |
| macOS port: PortAudio audio, menus/keys, CPU meter, fonts, `mac_complex_shim.h`, `mshv_thread_helper.h`, `mshv_txtrace.h`, Boost `clang.hpp`; fixes for FFT pointers and Flex tuner | Manoj Kumar R, VU2CPL — <https://github.com/vu2cpl/mshv-macos-port> | GPLv3 (Boost files: Boost Software License 1.0) | code merged |
| Icom LAN (RS-BA1) protocol: packet layout, login, token, CI-V and audio streams | wfview — Elliott Liggett W6EL, Phil Taylor M0VSE and contributors — <https://gitlab.com/eliggett/wfview> | GPLv3 | protocol reference; the C++ code in `src/HvRigControl/HvRigCat/icomlan/` is new |
| Icom LAN protocol (first open implementation) | kappanhang — Norbert Varga, HA2NON — <https://github.com/nonoo/kappanhang> | MIT | protocol reference |
| Icom LAN packet primitives | AetherSDR contributors — <https://github.com/aethersdr/AetherSDR> | GPLv3 | protocol reference (credential table, packet sizes) |
| IC-7300MK2 LAN behaviour (48 kHz LPCM only, big-endian inner sequence for token renewal) and a synthetic MK2 used to test this code | Simon S53ZO — IC7300MK2_Lan_Splitter <https://github.com/s53zo/IC7300MK2_Lan_Splitter> and PiRO <https://github.com/s53zo/piro> | AGPLv3 / GPLv3 | reference and local test only; no code included |
| LoTW user activity list (`lotw-user-activity.csv`) | ARRL — <https://lotw.arrl.org> | data file, downloaded by the user | read at run time |
| Windows installer | Inno Setup — Jordan Russell and Martijn Laan | Inno Setup License | build tool |
| AppImage | linuxdeploy and Qt plugin — <https://github.com/linuxdeploy> | MIT | build tool |

Original notices are kept in every file. If you see your work here without the right credit, please tell IC8TEM and it will be fixed.

This program is distributed in the hope that it will be useful, but **WITHOUT ANY WARRANTY**; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Contact: IC8TEM — <https://www.qrz.com/db/ic8tem>
