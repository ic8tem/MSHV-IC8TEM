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
