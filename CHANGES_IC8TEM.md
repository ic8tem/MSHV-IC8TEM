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

## 2026-11-18 — ic8tem.26
* `src/HvRigControl/HvRigCat/hvrigcatw.*` — new `RestartPolling()`. `set_freq`, `set_mode` and `set_ptt` stop the polling timer, and only `SetWriteCmd` started it again (drivers that write to the MSHV port, and the fictive write of Network/OmniRig). "Icom IC-7300 (Hamlib)" and Icom LAN do not write through MSHV, so after the first band change, F, mode change or TX the frequency and the mode were read no more (the display kept the frequency read at connection).
* `hamlibrig/hamlibrig.*`, `icomlan/icomlan.*` — `EmitCmdSent()` after SET_FREQ / SET_MODE / SET_PTT, connected to `RestartPolling()`. Tested with Hamlib 4.6.5 and its IC-7300 simulator (`simulators/simic7300`): before, no reading after `set_freq`; now the new frequency is shown about one polling interval later.

## 2026-11-18 — ic8tem.27
* `src/HvDecodeList/decodelist.*` — header sections `Interactive` instead of `Fixed` (message column still `Stretch`, minimum section 16 px). A width set by the user is kept in percent of the automatic width (`def_section_sizes` × font factor) per group of modes with the same columns (FT8/FT4/FT2/Q65, MSK144/MSK144ms, JT65, …), applied in `SaveAndCorrSectionSize()`, sent to the other list (`EmitUserColWidth`), saved as `def_dl_colw` (index 112). Menu *Default Column Widths*.
* Dashed line for the periods with no decodes: list 1 gets the decode requests (`EmitDataToDecode`: period time, mouse button, RTD flags) and the end of decoding (`EmitDecode(false,0)`); after the last request of a period (full decode, FT8 stage 6, last RTD chunk) with no decode → a row with the time, drawn as a dashed line in `drawRow()`, not selectable for spot/country, header *Message 0*. A later decode of the same period removes it. Not for mouse decodes of part of the period (buttons 1/3), files, TX periods. Option *Dashed Line For Periods With No Decodes*, `def_nodec_lines` (index 113, default on).
* `packaging/i18n/ic8tem_strings.py` — the 2 new menu texts in 15 languages.

## 2026-11-20 — ic8tem.28
* `src/HvTxW/hvlocsrc.*` (new) — `HvLocSrc`: locator lookup when the call is not in the MSHV database. RX cache fed by the live decodes of list 1 (`ValidateStationInfo`, id 0): `ParseMsg()` takes the grid sent by the station itself (`CQ [mod] CALL GRID`, `CALL1 CALL2 [R] [serial] GRID`, JT65 `OOO`; never `RR73`), kept 20 T/R periods. Lists in `settings/database/locsources`: `CALL3.TXT` of WSJT-X Improved (SourceForge), `CALL3.TXT` EME of W7GJ, `grid_data.bin` of JTDX (`qCompress` + `QDataStream` big endian: `quint32` rows, `QString` call, `QString` grid, as read by Decodium). Read in a thread into a sorted array of 16-byte records (call + grid code), binary search. Downloaded 30 s after start when missing or older than 14 days, checked every 6 h; QNetworkAccessManager (redirects followed) or the system curl when Qt has no OpenSSL; a download is checked before it replaces the old list. Online lookup `api.g7vrd.co.uk/v1/locators/by-callsign/CALL` only when nothing else has the locator, 1.2 s after the last change of the call; answers kept 30 days (not found: 1 day), 15 min pause after HTTP 429, 2 min after other errors. Order RX, C3, EME, JTDX, WEB; a 4-character grid becomes 6 characters if a later source has the same square. Settings `settings/ms_locsrc`; page 4 of Network Configuration (`HvLocSrcW`).
* `src/HvTxW/hvtxw.*` — `Check("call")`: with no MSHV database entry, `LocFromOtherSources()`; label `RX:`, `C3:`, `EME:`, `JTDX:`, `WEB:` (tooltip: every source). `AddToLogMultiAnswerQSO`: local sources after the database. `HvTxW::DecodedMsgText()` picks the message column of each mode.
* TX restart (`CheckAutoRestartTx`): option *TO RADIO Call Decoded Again Sets Auto Is On* (`main_ms.*`, `def_auto_restart_tx`, index 114, default off). AUTO off, call in TO RADIO = sender of a live decode (`MsgSenderCall`: second call, call after CQ/QRZ/DE and modifier, fox call), selected TX message for him and not RR73/73/RRR/CQ, not Multi Answer, not blocked by the SWR protection (`HvPowerSwrW::EmitTxBlocked`) → TX period opposite to the decode (`SetTimePeriod_p(time,1)` when period recognition is on) and `auto_on()`. At most 3 automatic restarts per call; AUTO pressed by the operator resets the count.
* `src/HvRigControl/HvRigCat/network/network_def.h` — Hamlib entries `HAMLIB_FIRST`..: "Icom IC-7300 (Hamlib)" (3073, index 16 as before), "Icom IC-7610 (Hamlib)" (3078), "Icom IC-9700 (Hamlib)" (3081); `IS_HAMLIB()`, `HAMLIB_HL_MODEL()`; `hvrigcatw.cpp`, `hvrigcontrol.cpp`, `hamlibrig.cpp` use them. Tested with Hamlib 4.6.5 simulators `simic7610`/`simic9700` (also answering 15 12 and 15 13): frequency, set_freq, RFPOWER, Po/SWR/ALC; the MSHV CAT (CI-V 98h / A2h) reads 15 11/12/13 as well.
* `src/HvPowerSwr/hvpowerswrw.*` — `AutoMaxW()`: with an IC-9700 on the MSHV CAT the maximum power follows the band (2 m 100 W, 70 cm 75 W, 23 cm 10 W); `EmitTxBlocked(bool)`.
* `src/HvTxW/HvRadioNetW/radionetw.*` — `AddPage()` for page 4.
* `packaging/i18n/ic8tem_strings.py` — 29 new strings in 15 languages.

## 2026-11-20 — ic8tem.29
* `src/HvRigControl/HvRigCat/hvrigcatw.*` — fix: `PwrTextFamily()` compared `get_rig_name()` ("Yaesu FT-991", the list text with the maker) with the bare model names, so no Yaesu/Kenwood/Elecraft radio was ever recognised since ic8tem.20; new `PwrModelName()` takes the name from the family table. The Elecraft branch of `SetRig()` now sets `s_active_model_id`/`s_active_fact_id` (never set before).
* New text CAT families (commands and scales from Hamlib 4.6): Yaesu FT-450/FT-450D (family 1), FTDX-9000 (8: `RM08` Po W, `RM09` SWR, `RM07` ALC, PC 200 W); Kenwood TS-890S/TS-990S (7: `RM11;RM21;RM;` ALC/SWR 0-70, TS-890S `SM;` Po in TX, PC 200 W on the TS-990S), TS-870S/TS-850/TS-690S/TS-450S (6: `RM;` selected meter, SWR = 60/(30-n)-1, ALC n/30; PC only on TS-870S, read only on TS-450S), clones BG2FX FX4 and Hilberling PT-8000A (2), SDR Console (3); PowerSDR/Thetis (9: `ZZPC`, `ZZRM5` Po W, `ZZRM8` SWR, in TX).
* `pwr_tx`: GET_METERS carries "tx" from the Power & SWR window (`hvpowerswrw.cpp`), so Po is asked in TX also with PTT on RTS/DTR (before: only with CAT PTT, `s_f_ptt`).
* Tested with the Hamlib 4.6.5 simulators (FT-991, FT-450, TS-890S, TS-990S, TS-590SG, K3, K4, PowerSDR) and a parser test for every family.

## 2026-11-20 — ic8tem.30
* `src/HvRigControl/HvRigCat/hamlibrig/hamlib_wrap.cpp` — fix: on a PC without Hamlib in PATH the "(Hamlib)" radios stopped at "Hamlib: cannot load libhamlib-4.dll ... The specified module could not be found". `libhamlib-4.dll` imports `libusb-1.0.dll`, shipped next to it in the `hamlib` folder, but `QLibrary` uses `LoadLibrary`, which looks for the imports only in the program folder, the system folders and PATH. The library is now loaded first with `LoadLibraryExW(..., LOAD_WITH_ALTERED_SEARCH_PATH)` (imports from its own folder; `libwinpthread-1.dll`/`libgcc_s_*` already in memory from the program folder), then `QLibrary` takes the module in memory. Only the first error is shown, with a hint when `libusb-1.0.dll` is missing (new string in 15 languages).
* `src/main.cpp` — `--hamlib-check=<file>` (load as the radios do) and `--hamlib-check-plain=<file>` (plain `QLibrary`, as up to ic8tem.29): write the result to `<file>` and quit (Windows builds with Hamlib only).
* `.github/workflows/build-all.yml` — Windows x64/x86: "Hamlib load test" runs the packaged `MSHV-IC8TEM.exe --hamlib-check` with only the Windows folders in PATH (the runner's MinGW, Qt or Hamlib folders could hide a missing DLL); no release when it fails.

## 2026-11-21 — ic8tem.31
* `src/HvUpdate/mshv_updater.cpp` — fix: with two Windows installations made with the installer (for example one per radio, in two folders) only one was ever updated: the updater started `setup.exe /SILENT /AUTOUPDATE=1` without `/DIR`, and with `UsePreviousAppDir=no` the installer always went to its default folder `C:\MSHV-IC8TEM-NEW` (closing and restarting the MSHV running there). Now `/DIR="<folder of this installation>"` and `/AUTOUPDATE=2`; the helper waits for the installer and starts MSHV again from its folder with the same command line (`--inst-name=` kept, also for zip, tar.gz, AppImage, macOS and packages). The temporary folders are per installation (`MSHV-IC8TEM-update-<tag>`, `MSHV-IC8TEM-unzip-<tag>`, tag = 8 hex of SHA-1 of the program folder): two installations running together deleted each other's download. The update window shows the folder of the installation.
* `packaging/windows/mshv-ic8tem.iss` — `/AUTOUPDATE=2`: no start of MSHV by the installer (`/AUTOUPDATE=1` of the older updaters still restarts it); shortcuts untouched by an automatic update; shortcuts of a folder other than `C:\MSHV-IC8TEM-NEW` named "MSHV-IC8TEM (<folder>)", so a second installation does not take those of the first. Fix: settings and logs of the previous folder are copied only into a folder that had no settings yet and never during an automatic update (before, an installation into an existing folder could get the settings of the folder registered last, that is of the other installation).
* `packaging/windows/test_installer.ps1` (new), `.github/workflows/build-all.yml` — Windows x64/x86: installs into `C:\MSHV-IC8TEM-NEW` and `C:\MSHV test 9700`, updates the second one as MSHV does (`/SILENT /AUTOUPDATE=2 /DIR=...`) and checks that only that folder changed, that both keep their settings, that the shortcuts are right and that the installer did not start MSHV; no release when it fails.

## 2026-11-23 — ic8tem.32
* `src/HvTxW/hvpskheard.*` (new) — calls that heard me: about one minute after the start, then every 5 minutes (the limit asked by PSKReporter) while transmitting or within 30 minutes of the last transmission, MSHV reads `https://retrieve.pskreporter.info/query?senderCallsign=<My Call>&flowStartSeconds=-1800&rronly=1&noactive=1&appcontact=...` (Qt network with OpenSSL, otherwise the system `curl`, as the locator lookup). The receivers are kept per band (MSHV band table) with the time of their last report, also under the base call (`IZ0ABC/P` → `IZ0ABC`); a report counts for 35 minutes; an error keeps the data already read and is shown with PSKReporter's own message. Idea from WsjtxUtils.Searchlight by KC3PIB (MIT), no code reused.
* `src/HvDecodeList/decodelist.*` — underline (Text Highlight color 8, fluorescent yellow 230,255,0, with a darker edge on the light theme) under the calls of the stations that received me on the band in use, also when another color already marks the call; both lists repaint when a new answer arrives. Default colors 7 and 8 set in the constructor too.
* `src/HvTextColor/hvtxtcolor.*` — group *PSKReporter*: "Underline calls that heard me (PSKReporter)" (on by default, option 32 of the saved text marks; older settings: on), COLOR button, status line; 9 colors per theme (older color settings keep the default for the new one).
* `src/HvTxW/hvtxw.*`, `src/main_ms.cpp` — My Call (Macros), band and TX state to the PSKReporter lookup; first start: the default options reach the lists.
* `packaging/i18n/ic8tem_strings.py` — 9 new strings in 15 languages.

## 2026-11-23 — ic8tem.33
* `src/HvTxW/hvpskheard.*` — calls that heard me on every MSHV band (135 kHz … 248 GHz). Band of a report from its frequency; when the frequency is missing or in no band (4-byte frequency field: nothing above 4.29 GHz, so 5.7/10/24/47 GHz arrive wrapped) the band I was transmitting on at the time of the report (history of my transmissions with their band; a report counts from 30 s before the start to 150 s after the end of a transmission); when one of the two bands is the usual IF of a transverter (28, 144, 432, 1296, 2320 MHz) and lies below the other, the report counts for both (other station's transverter without offset, or mine). The answers are merged (PSKReporter returns only the last 100 records) and pruned after 35 minutes; an answer cut short keeps the reports already read. A band change during TX splits the transmission. Status count only of the valid reports.
* `src/HvTxW/hvtxw.cpp` — fix: the band reached the lookup only when it changed, so on 70 MHz (the band HvTxW starts with) it stayed unknown after a restart and nothing was underlined; now the band is set at the start and at every band selection.

## 2026-11-26 — ic8tem.34
* **Q65 Pileup** of WSJT-X 2.7.0 and 3.2.0-rc1 (K1JT and the WSJT Development Group, GPLv3), on top of what MSHV 2.72 already had (activity *Pileup*, list `tsil.3q` of the last 40 callers kept 24 hours, a priori decoding `q3` with that list):
  * `src/main_ms.*`, `src/HvButtons/hvmodbtsw.*` — Mode › Q65: **Q65 Pileup** (on: Q65A when not already in Q65, activity *Pileup*, callers window; off: back to the previous activity) and **Q65 Pileup - Callers...**; right click on a Q65 mode button switches it on and off (from another mode always on, since the activity is kept from the last session). Position and visibility of the window saved (`def_q65_pileup_w`).
  * `src/HvTxW/hvq65pileupw.*` (new) — callers window: N, Freq, Call, Grid, El (Moon elevation at the caller's grid), Age (h), `#`; sorted by frequency; click = Tx3 to the caller (`call mycall R grid`) with the RX frequency and the TX period taken from his last decode and Auto as with a double click in the lists; Ctrl+click removes the caller; Refresh; Erase list (with confirmation); the station in TO RADIO in bold; callers with the Moon below the horizon dimmed.
  * `src/HvDecoderMs/decoderq65.*` — WSJT-X 3.2: the spare 78th bit is read back (`iand(dat4(13),1)`) and a decode that carries it gets `#` after its Q code (`Q0#`); the a priori list holds both codewords of every message (bit 0 and 1: up to 411, `MAX_NCW` as in WSJT-X 3.2), so the flagged messages of known callers are decoded with the list too; AP type 3 masks bit 78 only outside the Pileup activity. In the Pileup activity: Moon elevation of every caller (centre of the grid, MSHV `tmoonsub_`, within 0.5° of an ephemeris) and callers with the Moon more than 5° below the horizon dropped at the next decode (as `get_q3list.f90`; not with data from a file); a later decode without `#` clears the mark. The list is read and written under a mutex (decoder threads and window). Fixes: `ReadList()` checks the size of `tsil.3q` (empty or cut file).
  * `src/HvMsPlayer/libsound/HvGenQ65/gen_q65.*`, `q65_subs.cpp`, `genmesage*.cpp`, `mpegsound.h`, `src/HvMsPlayer/msplayerhv.*` — `genq65(..., iflag)`: `dgen(13)=2*dgen(13)+iflag`; `Q65_FULLAPLIST_SIZE` 256 → 411; `q65_check_llh()` sets the likelihood and rejects symbols out of the alphabet before using them (WSJT-X 3.2).
  * `src/HvTxW/hvtxw.*` — in the Pileup activity a transmission carries bit 78 ("I copied your last transmission") when the period just ended decoded the station in TO RADIO as the sender (single use; kept when the message changes during the same transmission; a decode that ends after the start of the transmission sets it when the message is generated again); our TX row in the list shows `#`. QSO logged in the Pileup activity: the call leaves the list (WSJT-X `rm_q3list`).
  * `src/HvDecodeList/decodelist.*` — with two decode lists the *Type* column (Q code and `#`) is shown during the Pileup activity, 50 px wide (with one list it is always there).
  * `src/HvTxW/HvMakros/hvmakros.*` — `SetActivityId()`, the inverse of the activity combo box.
  * `packaging/i18n/ic8tem_strings.py` — 22 new strings in 15 languages.
  * Tests: bit 78 there and back (`Q0#` / `Q0`; MSHV .33 decodes the flagged message as `Q0`); Moon elevation against PyEphem; a priori decoding with the list, flagged message at −26/−27/−28 dB: 8/8 each (MSHV .33: 6/8, 4/8, 0/8), unflagged as in .33; no false decodes on noise with the full list; the whole program under Xvfb (menu, right click, window, decodes with `#`, click = Tx3 sent with `#`, Ctrl+click, removal after logging, back to the previous activity).

## 2026-11-27 — ic8tem.35
* `src/HvRigControl/hvrigcontrol.*` — Rig Control: after a network radio (Icom (Hamlib), Icom LAN, TCI, Hamlib NET rigctl...) a serial radio no longer inherits its state. The Network line goes back to "Disconnected" (the last message, e.g. "Hamlib: COM3 115200: Access denied", stayed in red and looked like an error of the serial radio); the PTT test follows the serial port; with an Icom (Hamlib) entry the port shown in Port had been opened by Hamlib and choosing it again did not reopen it: MSHV now opens it itself (after 1.5 s, up to 5 tries while Hamlib frees the port). A late status of the network radio just removed is ignored: it could stop the polling of the serial radio. From a user report (IC-9700).
* `src/main_ms.cpp` — "Txing" bar: grey while receiving (green, with the moving fill, looked like a transmission), red while transmitting; the rest of the bar takes the button colour of the window (from the second refresh it was black, because the label palette returned the colour of its own gradient).
* Tests: simulated IC-9700 (CI-V A2) on a virtual serial port: the classic "Icom IC-9700" entry reads frequency and mode every second (DIGU 144.174.000) and the CAT Log shows every frame; switch from a network rig showing an error to "Icom IC-9700": line back to "Disconnected", PTT test "NO PORT SELECTED", port chosen, CAT running; Txing bar in the light and dark styles, receiving and transmitting (Tune).
