# Riconoscimenti — Credits

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
| Hamlib library (`libhamlib-4.dll`, Windows packages) for "Icom IC-7300 (Hamlib)" | The Hamlib Group — <https://hamlib.github.io> | LGPL v2.1 or later | DLL shipped unchanged in the `hamlib` folder, loaded at run time; licence file next to it |
| MSK144 impulse blanker (menu Decode › MSK144 Noise Blanker (NR0V NB2)) | approach of the "NB2" noise blanker of WDSP — Warren Pratt, NR0V — <https://github.com/TAPR/OpenHPSDR-wdsp> | GPL v2 or later | method reused (detection against the average level, advance/hang time, raised-cosine slews); the C++ code in `src/HvDecoderMs/hvmsknoise.h` is new, changed for meteor scatter (pings never blanked) |
| MSK144 linear prediction blanker (menu Decode › MSK144 Noise Blanker LPC (DL2FW)) | `alt_noise_blanking()` — Michael Wild, DL2FW — UHSDR firmware <https://github.com/df8oe/UHSDR> | GPLv3 | ported to double precision and plain C++ in `src/HvDecoderMs/hvmsknoise.h`, robust threshold for digital signals |
| LoTW user activity list (`lotw-user-activity.csv`) | ARRL — <https://lotw.arrl.org> | data file, downloaded by the user | read at run time |
| Windows installer | Inno Setup — Jordan Russell and Martijn Laan | Inno Setup License | build tool |
| AppImage | linuxdeploy and Qt plugin — <https://github.com/linuxdeploy> | MIT | build tool |

Original notices are kept in every file. If you see your work here without the right credit, please tell IC8TEM and it will be fixed.

See also the License section of the README.
