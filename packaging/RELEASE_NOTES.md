**MSHV-IC8TEM** — modified version of MSHV 2.76.7 by LZ2HV, with additions by IC8TEM.
Not the official MSHV: please report problems with this version to IC8TEM.

**ic8tem.13:** no limit on the number of QSOs in the Log (the 50000 QSOs warning is gone) · Unlock TX Slots ON by default (older settings start unlocked; untick it for the original limits) · release page without the download table, links only to github.com.

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


The Windows installer needs no administrator rights (it installs in `%LOCALAPPDATA%\Programs\MSHV-IC8TEM`) and keeps your settings and logs when you update or uninstall; Windows SmartScreen may warn because the installer is not code-signed (*More info → Run anyway*). Installed Linux/macOS packages keep your data in `~/.local/share/MSHV-IC8TEM` (Linux) or `~/Library/Application Support/MSHV-IC8TEM` (macOS). The portable folders (zip / tar.gz) keep it inside the folder.

License: GNU GPL v3. MSHV © LZ2HV; WSJT algorithms © K1JT and the WSJT Development Group; macOS port code © VU2CPL; modifications © 2026 IC8TEM. `SHA256SUMS.txt` lists the checksums.

---
*Italiano:* versione modificata di MSHV 2.76.7 (LZ2HV) con Doppler tracking/CFOM via CAT, lista LoTW con pallino e filtro, annullamento fittizio del log, sblocco TX Slots (attivo di default). Scegli il file per il tuo sistema nella sezione Assets; la guida in italiano è `LEGGIMI_DOPPLER_IC8TEM.txt`.
