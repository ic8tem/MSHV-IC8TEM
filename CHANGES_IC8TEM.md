# Changes by IC8TEM to MSHV 2.76.7

Base: MSHV 2.76.7 full source code by LZ2HV. Modifications started November 2026.
All modified lines carry the comment `IC8TEM mod`. A unified diff against the original is in `DOPPLER_IC8TEM.patch`.

## 2026-11-03 — Doppler tracking
* `src/HvTxW/HvAstroDataW/hvastrodataw.*` — Doppler tracking panel, sked frequency, Tx Shift (±999.999 MHz), Echo Mode box (disabled).
  * Full Doppler: RX = Sked + D, TX = Sked − D (D = Me→Moon→DX one-way Doppler)
  * Own Echo: RX = Sked + E, TX = Sked (E = own echo Doppler)
  * CFOM: RX = Sked + E/2, TX = Sked − E/2
  * On DX Echo: RX = Sked + D, TX = Sked + DX echo Doppler (frozen at start) − D
  * Call DX: Sked = current RX − D, then as On DX Echo
  * Fix: `xdop[]` in `astro()` made `static` (was read uninitialized).
* `src/HvRigControl/hvrigcontrol.*` — CAT: TX frequency before PTT on, RX frequency after PTT off (resent after 350 ms), updates during TX; Static TX suspended while tracking.
* `src/HvTxW/hvtxw.*`, `src/main_ms.cpp` — wiring and settings (`def_doppler_track`).

## 2026-11-04 — LoTW, log, Multi Answer
* `src/HvTxW/HvRadioNetW/hvlotwusers.*` (new) — LoTW users list (Page 3 of Network Configuration), settings in `settings/ms_lotw`.
* `src/HvDecodeList/decodelist.cpp` — black dot for LoTW users.
* `src/HvTxW/HvLogW/hvlogw.*` — Fictitious log reset (`def_log_fict_reset`).
* `src/HvTxW/hvmultianswermodw.*` — TX Slots unlock.

## 2026-11-05 — Defaults, filters
* `src/main_ms.cpp`, `bin/settings/ms_settings` — default mode switcher buttons FT8, FT4, Q65B, Q65C, Q65A, MSK144, JT65B (visible).
* `src/HvDecodeList/decodelist.cpp` — LoTW dot aligned to the right edge of the message column.
* `src/HvDecodeList/hvfilterdialog.*`, `decodelist.*` — filter "Show LoTW Users Only".

## 2026-11-12 — Publication
* `src/HvTxW/hvmultianswermodw.*` — TX Slots unlock is now an option ("Unlock TX Slots"), OFF by default; original LZ2HV restrictions restored when OFF.
* `src/config.h` — program name "MSHV-IC8TEM".
* README, CHANGES, build scripts (`compila.bat`, `build_msys.sh`, `.github/workflows/build-windows.yml`).
* `src/HvMsCore/mscore.cpp` — `fft_avg`/`fft_window` set to NULL before first use (fix from VU2CPL's macOS port: freed before allocation).
* `src/HvRigControl/HvRigCat/network/flexpanel.cpp` — Flex tuner Tune/Bypass are no longer default buttons, so Enter in a power box does not key the radio (fix from VU2CPL).
* `src/mshv_ic8tem_path.h` (new), `main_ms.cpp`, `hvcty.cpp`, `decodelist.cpp`, `hvastrodataw.cpp` — writable data folder: `MSHV_DATA_DIR` (Linux/BSD packages, AppImage) or `~/Library/Application Support/MSHV-IC8TEM` (macOS); unchanged on Windows.
* `packaging/`, `.github/workflows/build-all.yml` — packages for Windows x64/x86/ARM64, Linux (AppImage, tar.gz, .deb, .rpm), Raspberry Pi OS, Slackware ARM64, FreeBSD, macOS.
* macOS: the `#if defined _MACOS_` blocks of VU2CPL's macOS port (github.com/vu2cpl/mshv-macos-port, GPLv3) merged into 26 files (PortAudio audio, CPU meter, Flex/TCI socket fix, menus and Control key, fonts, Log menu button), plus `mac_complex_shim.h`, `mshv_thread_helper.h`, `mshv_txtrace.h` (by VU2CPL) and `mshv_app_path.h` (maps his data path to `MshvAppPath()`). His user-defined bands and update check are not included. `hvfontdialog.cpp` taken from his tree (also fixes the saved list-font lookup).
* Explicit `#include <pthread.h>` in four headers (needed by clang on Windows ARM64).
* Bundled Boost config: `clang.hpp` and the clang selection from VU2CPL's port, so clang builds (macOS, FreeBSD, OpenBSD, Windows ARM64) compile. `mac_complex_shim.h` enabled for every libc++ build; `malloc.h` replaced by `stdlib.h` on Apple/BSD.
* OpenBSD: built with the PortAudio (sndio) audio path of the macOS port (`_OPENBSDHV_`), CPU meter via `sysctl(KERN_CPTIME2)`.

## 2026-11-12 — 2.76.7-ic8tem.2
* Windows installers (`…-windows-x64-setup.exe`, `x86`, `arm64`), Inno Setup, per-user install in `%LOCALAPPDATA%\Programs\MSHV-IC8TEM` (no administrator rights); settings and logs are kept on update and uninstall. Italian and English.
* `src/mshv_ic8tem_path.h` — if the program folder is not writable (e.g. copied into Program Files or /opt), data go to `%APPDATA%\MSHV-IC8TEM` or `~/.local/share/MSHV-IC8TEM`, seeded from the program folder.
* Automatic release when `MSHV_VERSION` changes on `main`.

## 2026-11-14 — 2.76.7-ic8tem.3
* `src/HvDecoderMs/decodermsk144.cpp` — MSK144: the ping duration is now measured for every decode (before: only in MSK144ms) and written in the Width field (20 ms steps). Also present in ALL.TXT. Short messages (sh) have no value.
* `src/HvDecodeList/decodelist.*`, `src/main_ms.*` — menu Decode List: **Show/Hide Width (Ping Duration) Column MSK144**, OFF by default, saved in `def_msk_width_col`.

## 2026-11-14 — 2.76.7-ic8tem.4: Icom LAN
* `src/HvRigControl/HvRigCat/icomlan/icomlan.*` (new) — native Icom network protocol (RS-BA1): login, token renewal, CI-V stream, 48 kHz audio stream both ways, retransmissions, reconnection. Runs in its own thread. CAT commands are the existing MSHV Icom driver (`icom.cpp`), carried over UDP.
* `src/HvRigControl/HvRigCat/network/network_def.h` — new rigs `Icom IC-7300MK2 LAN`, `IC-705 LAN`, `IC-9700 LAN`, `IC-7610 LAN`, `IC-905 LAN` (network models 15-19).
* `src/HvRigControl/HvRigCat/hvrigcatw.cpp` — creates the Icom LAN rig.
* `src/HvRigControl/hvrigcontrol.*` — Network box: LAN User and Password fields (saved with the network settings, password base64-encoded, not encrypted).
* `src/SettingsMs/settings_ms.cpp` — sound devices `Icom LAN Input` / `Icom LAN Output`.
* `src/HvMsCore/mscore.cpp` — `Icom LAN Input`: 48 kHz, or 44.1 kHz (resampled in the Icom LAN thread) for the modes that need 11025 Hz (FSK441, FSK315, JTMS, JT6M, ISCAT).
* `src/HvMsPlayer/libsound/rawplayer.cpp`, `genmesage.cpp`, `src/HvRigControl/HvRigCat/network/network.cpp` — TX audio to the Icom LAN stream (44.1 kHz TX resampled to 48 kHz).
* README: credits table for all reused code and protocol references.

## 2026-11-14 — 2.76.7-ic8tem.5
* `src/HvTxW/HvRadioNetW/hvlotwusers.*` — clickable link https://lotw.arrl.org/lotw-user-activity.csv and button **Download from ARRL and Load** (download, check, copy to settings/database, load).
* Where Qt has no OpenSSL (Windows) the download uses the system `curl` (Windows 10/11, macOS, Linux).
* `wiki/` + `.github/workflows/wiki.yml` — Italian wiki with all the new functions, published to the GitHub wiki.
