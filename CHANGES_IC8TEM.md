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

## 2026-11-15 — 2.76.7-ic8tem.6: automatic updates
* `src/HvUpdate/mshv_updater.*` (new) — checks https://api.github.com/repos/ic8tem/MSHV-IC8TEM/releases/latest 90 s after start and every 6 hours (Help › Check for Updates, Help › Check for Updates Automatically). On *Update now*: downloads the package of this installation and `SHA256SUMS.txt`, checks SHA256, starts a helper (PowerShell on Windows, sh elsewhere) that waits for MSHV to close, installs and restarts it. Settings in `settings/ms_update`.
* `src/mshv_ic8tem_version.h` (new) — version compiled in; the build stops if it differs from MSHV_VERSION.
* `src/main_ms.cpp` — Help menu entries.
* Packaging: every package carries `mshv-package.txt` (asset name of its own kind) so the updater picks the right file; Windows installer accepts `/AUTOUPDATE=1` and restarts MSHV after a silent update.

## 2026-11-15 — 2.76.7-ic8tem.7
* macOS: the app did not start on some Macs ("Could not load the Qt platform plugin cocoa": `libbrotlicommon.1.dylib` missing — macdeployqt does not follow `@rpath` dependencies). New `packaging/macos/fix_bundle.sh` copies every missing Homebrew library into the bundle and stops the build if any reference still points outside the app.

## 2026-11-15 — 2.76.7-ic8tem.8
* Icom LAN: one rig entry **Icom LAN** instead of five models; the CI-V address announced by the radio at login selects the Icom model (`Icom::ModelFromCivAddr`). Old names `Icom IC-xxxx LAN` in the settings are mapped to it.
* Icom LAN: **CI-V Port** and **Audio Port** fields in Rig Control (empty = control port + 1 / + 2), saved with the network settings.

## 2026-11-15 — 2.76.7-ic8tem.9: MSK144-WB
* UDP (WSJT-X protocol, "UDP Broadcast Settings"): new message type 64 sent by MSK144-WB (wideband MSK144/FSK441 decoder on SDRangel) on a double click: message, time, SNR, dial frequency, TX flag, mode, his call, his locator.
  * `src/HvTxW/HvRadioNetW/MessageClient.*`, `radionetw.*` — read the message (the last three fields are optional) and pass it on.
  * `src/main_ms.*` — stop a TX in progress, switch to MSK144 or FSK441, then (after the mode change) `HvTxW::SetIc8temSelect()`.
  * `src/HvTxW/hvtxw.*` — `SetIc8temSelect()`: QSY with `SetDefFreqGlobal(3, …)` (transverter offset of Rig Control applied), then the same as a double click in the decode list: MSK144 via `DecListTextAll()` with Auto Seq according to the TX flag; FSK441 his call and locator, TX on with `auto_on()`. For "CQ nnn" the frequency sent by MSK144-WB (MHz of the signal + nnn kHz) is set again after the list's own QRG QSY.

## 2026-11-16 — 2.76.7-ic8tem.10: Power & SWR
* `src/HvPowerSwr/hvpowerswrw.*` (new) — window Options › View Power and SWR: RF power (slider, presets, per-band and optional per-mode memory, read from radio), Po/SWR/ALC meters while transmitting (every 200 ms), tune at the per-band tune power with restore, SWR protection (stop and block TX until Unlock), ALC and low Po warnings. Settings in `settings/ms_pwrswr`.
* `src/HvRigControl/HvRigCat/rigdef.h`, `icom/icom.*` — new CAT commands SET_RFPOWER, GET_RFPOWER, GET_METERS (CI-V 14 0A, 15 11/12/13) and their answers.
* `src/HvRigControl/HvRigCat/hvrigcatw.*`, `src/HvRigControl/hvrigcontrol.*` — commands and answers forwarded; available only with Icom CI-V or Icom LAN and the CAT running.
* `src/main_ms.*` — menu entry, TX state, mode, tune and stop TX connected to the window.
* Translations: `packaging/i18n/ic8tem_strings.py` + `make_qm.py` build `src/HvTranslations/ic8tem_<lang>.qm/.ts` for the 15 MSHV languages; loaded next to the original `mshv_<lang>.qm`.

## 2026-11-16 — 2.76.7-ic8tem.11: more packages
* `.github/workflows/build-all.yml` — rpm also for aarch64 (openSUSE Leap 15.6 on ARM runner); Alpine Linux 3.20 / musl tar.gz for x86_64 and aarch64; macOS zip next to the dmg.
* `packaging/make_release_body.py` + `.github/workflows/release-text.yml` — release page starts with the table "Scarica la versione per il tuo sistema" (system, installer, zip), built from the files really published; rows for Windows 7 SP1/8/8.1 point to the Windows 10/11 files.
* Windows installer: `MinVersion=6.1sp1` (Windows 7 SP1).

## 2026-11-16 — 2.76.7-ic8tem.12
* Power & SWR: tab **CAT Log** with the CI-V frames of power/meters sent and received (hex + decoded; `FA` = NG), *Read now*, *Copy*, *Pause*, *Clear*; status line with rig, connection type and CAT state (`HvRigCat::PwrStatus()`).
* Fix: availability is asked every second (`HvRigControl::AskPwrCapable()`); before, the window stayed "not available" when the CAT had started before it was connected.
* `icom.cpp`: frames 14/15 sent and 14/15/FA received are reported as LOG_TX / LOG_RX.
* Translations of the new strings in the 15 MSHV languages.

## 2026-11-16 — ic8tem.13
* (ic8tem.14) `src/HvTxW/HvLogW/hvlogw.cpp` — no limit on the number of QSOs in the Log (CheckLogCount warning disabled).
* `src/HvTxW/hvmultianswermodw.*` — "Unlock TX Slots" ON by default; settings saved by older versions start unlocked (new "u1" mark marks the saved choice).
* `packaging/make_release_body.py` — no download table; `src/HvUpdate/mshv_updater.cpp` — release page link to github.com.

## 2026-11-16 — ic8tem.15
* `src/HvPowerSwr/hvpwrlink.*` (new) — own connection of the Power & SWR window: Hamlib rigctld (TCP) or Icom CI-V on its own serial port, independent from the MSHV CAT.
* `src/HvPowerSwr/hvpowerswrw.*` — Settings › Connection, meters routed through the chosen connection; translations updated.

## 2026-11-16 — ic8tem.16
* `src/HvRigControl/HvRigCat/hvrigcatw.cpp` — the active rig type is now set also for the Icom CI-V radios (it was never set), so the Power & SWR window works with them.
* `src/HvRigControl/HvRigCat/icom/icom.cpp` — frequency (03) and mode (04/26) answers accepted also when they arrive after the next poll command (Icom LAN latency); CAT Log of every frame.
* `src/HvRigControl/HvRigCat/icomlan/icomlan.*` — TX audio statistics in the CAT Log.

## 2026-11-16 — ic8tem.17
* `src/HvRigControl/HvRigCat/hamlibrig/` (new) — "Icom IC-7300 (Hamlib)": Hamlib library loaded at run time (Windows builds with HAVE_HAMLIB), worker thread, frequency/mode/PTT/RF power/meters.
* `network/network_def.h`, `hvrigcatw.cpp`, `hvrigcontrol.cpp` — new network entry (Server = COM port, Port = baud rate); Icom LAN checks limited to its own entry.
* `.github/workflows/build-all.yml` — Windows x64/x86 download the official Hamlib Windows package and ship its DLLs in `hamlib\`.
* `src/HvDecoderMs/decoderjt65.cpp`, `decoderms.*`, `main_ms.cpp` — JT65 deep search: calls heard in the last 24 h (any decoded mode) first, most recent first, then the database in alphabetical order (limit 15000 calls unchanged).

## 2026-11-17 — ic8tem.18
* CI: HAMLIB_DIR passed to qmake before the project file (with -after the Windows builds had no HAVE_HAMLIB); the build fails if Hamlib was downloaded but not enabled.
* `src/main_ms.*` — "Txing" bar shows the progress of the T/R period (green RX, red TX), updated every 100 ms.

## 2026-11-17 — ic8tem.19
* `hvrigcontrol.*` — Icom IC-7300 (Hamlib) takes Port and Baud Rate from the serial combos (MSHV does not open the port, Hamlib reconnects on change).
* `hamlibrig.*` — no blocking in the GUI thread when the rig is replaced (worker closes and quits by itself), one Hamlib open/close at a time, long messages shortened (full text in the CAT Log).

## 2026-11-17 — ic8tem.20
* `hvrigcatw.*` — Power & SWR for text CAT radios: Yaesu NEWCAT (PC, RM4/5/6), Kenwood TS-2000/TS-590/TS-480 (PC, RM, SM0), Elecraft K3 family (PC, TM, BG, SW); meter scales from Hamlib. Answers parsed in HvRigCat (the drivers ignore them).
* `yaesu.cpp`, `kenwood.cpp`, `elecraft.cpp` — a short answer no longer drops the following answers of the same chunk.
* `elecraft_def.h` — new radio K4 (K3 command set, 4800-115200 baud); `hvrigcatw.cpp` — K4 `TM;` (ALC, CMP, FWD, SWR in one answer), `PCnnnH;`.
* `network.*` — Power & SWR for TCI (drive, tx_power/tx_swr notifications) and FlexRadio 6xxx (rfpower, FWDPWR/SWR meters).
* `packaging/windows/mshv-ic8tem.iss` — install folder C:\MSHV-IC8TEM-NEW (UsePreviousAppDir=no); settings/log/RxWavs/ExportLog/AllTxtMonthly/Screenshots copied once from the previous folder.

## 2026-11-17 — ic8tem.21
* `src/HvTxW/HvLogW/hvlogw.*` — Menu "Find And Delete Duplicate QSOs": same call + band + mode and same start / within N minutes / same UTC day; keeps the most complete QSO of each group, selects the others and uses the Delete Selected confirmation. `HvLogList::DeleteSel` removes consecutive rows in one call.

## 2026-11-17 — ic8tem.22
* `src/HvTextColor/hvtxtcolor.*` — "Worked in the log on other bands or modes" (pink, color 7) and "New grids only"; saved after the calls text in `default_txt_mark` (#30, #31).
* `src/HvDecodeList/hvworked.h` (new), `decodelist.*` — pink calls and new grids painted in the message column.
* `src/HvTxW/HvLogW/hvlogw.*` — QSO B4 lists built with sets (fast with very big logs); every log call kept for the pink highlight.

## 2026-11-18 — ic8tem.23
* `src/HvDecoderMs/hvmsknoise.h` (new) — impulse blanker (approach of WDSP NB2, NR0V, changed so that pings are never blanked), linear prediction blanker (port of `alt_noise_blanking()` by DL2FW, UHSDR, GPLv3) and carrier notch (steady lines in the median-over-time spectrum, not with a continuous MSK signal).
* `decodermsk144.cpp`, `decoderms.*` — `analytic_msk144_2()` applies them in the full-period and real-time MSK144 decoding; `SetMskNoise()`.
* `main_ms.*` — Decode menu: MSK144 Noise Blanker Off / NR0V NB2 / LPC DL2FW, MSK144 Carrier Notch (ZAP); setting `def_msk_nb_zap`.

## 2026-11-18 — ic8tem.24
* `src/HvRigControl/HvRigCat/hamlibrig/hamlibrig.cpp` — fix: reopening the rig emitted "Disconnected" first, so the commands stayed blocked after the following "Connected"; the reopen is now silent and every "Connected" status enables the commands. First frequency reading at once, written in the CAT Log.
* `hamlib_wrap.cpp` — error text from `rigerror2()` (one line; with older Hamlib the last line of `rigerror()`), hints for timeout and IO error; `hamlibrig.*` — no reopen with the same port and speed, 0.4 s pause after closing and one retry.

## 2026-11-18 — ic8tem.25
* `src/HvPowerSwr/hvpowerswrw.*` — window size = size of the open tab. A QTabWidget is as big as its biggest page (Settings), also for minimumSizeHint and heightForWidth: each page now wraps its content and the content of the pages not shown is hidden, so only the open tab counts. `Fit()` sizes the window to the Power and SWR tab when it is shown, when the tab changes, when the "not available" note appears or goes and after Unlock; in the other tabs, and for a new status line or the SWR block message, the window only grows. Kept inside the screen.
* Compact Power and SWR tab: the preset buttons share the row (the style gave each push button 75-80 px), big % font +6 instead of +10 with a fixed width, meters 110 px minimum, protection text `Stop TX if SWR > %1 for %2 s` (new string, 15 translations in `packaging/i18n/ic8tem_strings.py`), CAT status and events wrap (events: 2 lines reserved), SWR block message full width under the boxes. Settings tab: rigctld / CI-V and max power / meter interval on two rows each.
