**MSHV-IC8TEM** — modified version of MSHV 2.76.7 by LZ2HV, with additions by IC8TEM.
Not the official MSHV: please report problems with this version to IC8TEM.

**ic8tem.25:** **Power & SWR** window much smaller: it takes the size of the open tab — the Power and SWR tab is only as big as its content (before, it was as big as the Settings tab); Settings and CAT Log enlarge it when opened, and back on the first tab it shrinks again · shorter protection text (*Stop TX if SWR > 2.5 for 1.0 s*) · the red SWR block message spans the full width under the two boxes.

**ic8tem.24:** Icom IC-7300 (Hamlib) fixed: after the port was reopened (at start, or changing Port or Baud Rate) the status was green but every CAT command was dropped (no frequency reading, band buttons and **F** did not tune the radio, no Power & SWR). The frequency is now read at once on connection and written in the CAT Log. When the port cannot be opened, the red status shows the real Hamlib error in one line (not the truncated debug history) and what to check; no reopen when Port and Baud Rate did not change, one retry.

**ic8tem.23:** MSK144 (Decode menu, off by default): **Noise Blanker** for impulsive noise — NR0V NB2 style (recommended) or DL2FW linear prediction; pings are never blanked — and **Carrier Notch (ZAP)** for steady carriers and birdies (10 Hz notches, never on a continuous MSK signal).

**ic8tem.22:** Text Highlight — new **pink** color for calls already in the log but worked on other bands or modes · **New grids only** (ON): the grid color marks the grids not yet worked instead of the worked ones · faster QSO B4 lists with very big logs.

**ic8tem.21:** Log › Menu › **Find And Delete Duplicate QSOs**: same callsign, band and mode, with the same start time, within N minutes or on the same UTC day; the most complete QSO of each group is kept, the others are selected and deleted after confirmation · faster deletion of many QSOs.

**ic8tem.20:** Windows installer: new folder **C:\MSHV-IC8TEM-NEW** (settings and logs copied from the old one) ·  Power & SWR also with the MSHV CAT of **Yaesu** (FT-991/991A, FT-DX10, FT-710, FT-DX101, FT-891, FT-DX1200, FT-DX3000, FT-DX5000, FT-950, FT-2000, FTX-1), **Kenwood** (TS-2000, TS-590S, TS-590SG, TS-480) and **Elecraft K3/K3S/KX3/KX2** (rig "K3/KX3"): RF power and Po/SWR/ALC; commands and meter scales as in Hamlib · new radio **Elecraft K4** (K4/K4D/K4HD): Po, SWR and ALC in one reading (`TM;`) · **TCI** (drive; tx_power and tx_swr sent by the radio) and **FlexRadio 6xxx** (rfpower, FWDPWR/SWR meters) in the Power & SWR window.

**ic8tem.19:** Icom IC-7300 (Hamlib): uses the normal **Port** and **Baud Rate** of Rig Control (Hamlib opens the COM port itself and reconnects when they change); MSHV no longer freezes while Hamlib waits for the radio; long Hamlib errors go to the CAT Log.

**ic8tem.18:** fix: the radio **Icom IC-7300 (Hamlib)** was missing from the Rig Control list in the Windows packages of ic8tem.17 · the "Txing" bar fills from left to right as the T/R period goes by (green receiving, red transmitting).

**ic8tem.17:** new radio **Icom IC-7300 (Hamlib)** (Windows): the IC-7300 driven by the Hamlib library — frequency, mode, PTT, RF power and Po/SWR/ALC in the Power & SWR window. Rig Control › Network: Server = COM port, Port = baud rate. · JT65 Deep Search: the calls heard in the last 24 hours (any mode) come first in the list of 15000 calls taken from the database (CALL3.TXT).

**ic8tem.16:** fix: Power & SWR now works with the Icom radios on USB (IC-7300, IC-7610...) through the MSHV CAT: power setting and Po/SWR/ALC meters (the window said "not available" with CAT ON) · Icom (USB and LAN): frequency and mode answers are no longer lost when they arrive late (MSHV kept showing an old frequency over LAN) · CAT Log shows every CI-V frame (frequency, mode, PTT) and, with Icom LAN, how much TX audio reached the radio.

**ic8tem.15:** Power & SWR — own connection, also without the MSHV CAT: Hamlib rigctld (any radio) or Icom CI-V on its own serial port (Settings › Connection).

**ic8tem.14:** no limit on the number of QSOs in the Log (the 50000 QSOs warning is gone).

**ic8tem.13:** Unlock TX Slots ON by default (older settings start unlocked; untick it for the original limits) · release page without the download table, links only to github.com.

**ic8tem.12:** Power & SWR — CAT Log tab (CI-V frames sent and received, decoded), Read now, rig/CAT status; fix: window could stay "not available" when the CAT started first.

**ic8tem.11:** new packages — rpm ARM64, Alpine Linux (musl) x86_64 and ARM64, macOS zip.

**ic8tem.10: Power & SWR window** (Options › View Power and SWR) — RF power, Po/SWR/ALC meters, per-band memory, SWR protection. Icom USB and LAN.

**ic8tem.9:** MSK144-WB — a double click in the MSK144-WB list (wideband MSK144/FSK441 decoder) sets MSHV: mode, QSY, his call, period and TX. In Network Configuration, UDP Broadcast Settings: server 127.0.0.1, port 2237.

**ic8tem.8:** Icom LAN — one entry for all radios (model detected automatically), CI-V and audio UDP ports can be set.

**ic8tem.7:** macOS start-up fixed (a library was missing from the app).

**ic8tem.6: automatic updates** — every 6 hours MSHV checks GitHub and installs new versions with one click (Help › Check for Updates). From the next version on, updates arrive by themselves.

**ic8tem.5:** LoTW list downloaded directly from ARRL (button, link https://lotw.arrl.org/lotw-user-activity.csv) · Italian wiki.

**ic8tem.4: Icom LAN** — IC-7300MK2 (and IC-705, IC-9700, IC-7610, IC-905) controlled over the network with CAT and audio, no USB cable: Rig Control → *Icom IC-7300MK2 LAN*, Sound → *Icom LAN Input/Output*. Guide: docs/ICOM_LAN.md in the source.

**New:** Doppler tracking in *Astronomical Data* (Full Doppler, Own Echo, **CFOM**, On DX Echo, Call DX; Tx Shift ±999.999 MHz, rig set via CAT) · LoTW users list (*Network Configuration → Page 3*), black dot and *Show LoTW Users Only* filter · *Fictitious log reset* in the Log window · *Unlock TX Slots* (ON by default) · MSK144 Width (ping duration) column, optional · default mode buttons FT8, FT4, Q65B, Q65C, Q65A, MSK144, JT65B · fixes from VU2CPL (FFT pointers crash, Flex tuner keyed by Enter).


The Windows installer needs no administrator rights (it installs in `C:\MSHV-IC8TEM-NEW`; settings and logs of the previous folder `%LOCALAPPDATA%\Programs\MSHV-IC8TEM` are copied there once) and keeps your settings and logs when you update or uninstall; Windows SmartScreen may warn because the installer is not code-signed (*More info → Run anyway*). Installed Linux/macOS packages keep your data in `~/.local/share/MSHV-IC8TEM` (Linux) or `~/Library/Application Support/MSHV-IC8TEM` (macOS). The portable folders (zip / tar.gz) keep it inside the folder.

License: GNU GPL v3. MSHV © LZ2HV; WSJT algorithms © K1JT and the WSJT Development Group; macOS port code © VU2CPL; modifications © 2026 IC8TEM. `SHA256SUMS.txt` lists the checksums.

---
*Italiano:* versione modificata di MSHV 2.76.7 (LZ2HV) con Doppler tracking/CFOM via CAT, lista LoTW con pallino e filtro, annullamento fittizio del log, sblocco TX Slots (attivo di default). Scegli il file per il tuo sistema nella sezione Assets; la guida in italiano è `LEGGIMI_DOPPLER_IC8TEM.txt`.
