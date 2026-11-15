# Icom LAN — MSHV-IC8TEM

Collegamento diretto via rete alle radio Icom con porta LAN: **CAT e audio (ricezione e trasmissione) sul cavo di rete**, senza cavo USB, senza RS-BA1, senza wfview.

Radio previste: **IC-7300MK2**, IC-705, IC-9700, IC-7610, IC-905. Il codice è stato provato con un simulatore del protocollo del 7300MK2; **sulla radio vera va verificato** (segnalate i risultati a IC8TEM).

## 1. Impostazioni della radio

Nel menu di rete della radio (SET › Network sul 7300MK2; i nomi possono variare leggermente da modello a modello):

| Voce | Valore |
|---|---|
| DHCP o indirizzo IP | annotate l'indirizzo della radio |
| Network Control | **ON** |
| Control Port (UDP) | 50001 (predefinito) |
| Serial Port (UDP) | 50002 (predefinito) |
| Audio Port (UDP) | 50003 (predefinito) |
| Network User (ID / Password) | create un utente: 1–16 caratteri ASCII |

Per trasmettere l'audio che arriva dalla rete, nel menu **Connectors › MOD Input** impostate **DATA MOD** (e, se usate USB senza DATA, anche **DATA OFF MOD**) su **LAN** (sull'IC-705 la voce si chiama WLAN).

Consigliato: impostate anche il **timeout di trasmissione** della radio (TOT): se la rete cade mentre trasmettete, il PTT non può essere tolto dal PC.

## 2. Impostazioni in MSHV

1. **Options › Rig Control**: RIG = **Icom LAN**. È una sola voce per tutte le radio: il modello (cioè l'indirizzo CI-V) viene riconosciuto da solo, perché la radio lo comunica al collegamento.
2. Nel riquadro **Network**: *Server* = indirizzo IP della radio, *Port* = porta di controllo (50001), *LAN User* e *Password* = l'utente creato sulla radio, *CI-V Port* e *Audio Port* = porte seriale e audio della radio (vuoti = porta di controllo +1 e +2, cioè 50002 e 50003). Premete **Connect**: la scritta diventa verde "Connected to …".
3. PTT: **PTT** (via CAT).
4. **Options › Sound Settings**: Input = **Icom LAN Input**, Output = **Icom LAN Output**.

Le tre porte devono essere uguali a quelle del menu di rete della radio (Control, Serial, Audio). Se passate da un router con inoltro di porte, indicate quelle inoltrate.

## 3. Cosa fa

* CAT: frequenza, modo, PTT con i comandi Icom già presenti in MSHV (indirizzo CI-V del modello scelto, B6h per il 7300MK2). Funziona anche il Doppler tracking.
* Audio di ricezione: 48 kHz mono 16 bit. Per FSK441, FSK315, JTMS, JT6M e ISCAT viene ricampionato a 44,1 kHz, quindi **anche questi modi funzionano** via LAN.
* Audio di trasmissione: inviato solo con PTT attivo, a blocchi di 20 ms, buffer della radio 150 ms. I modi a 44,1 kHz vengono ricampionati a 48 kHz.
* Login, rinnovo del token ogni 60 s, ritrasmissione dei pacchetti persi, fino a 3 tentativi di riconnessione (5 se la connessione era già attiva).
* All'uscita da MSHV la sessione viene chiusa in modo pulito, così la radio accetta subito una nuova connessione.

## 4. Limiti

* La radio accetta una sola sessione LAN: se RS-BA1 o wfview sono collegati, MSHV non entra. Per usarli insieme si può mettere davanti lo splitter di S53ZO (<https://github.com/s53zo/IC7300MK2_Lan_Splitter>) e indicare in MSHV l'indirizzo dello splitter.
* Pensato per la rete locale. Via internet aumentano ritardi e perdite: per il meteor scatter il tempo deve restare allineato.
* La password è salvata nel file delle impostazioni codificata in base64, **non cifrata**.
* Lo spettro della radio non viene usato (MSHV ha il suo).

## 5. Riconoscimenti

Il protocollo di rete Icom non è documentato da Icom. Il codice di MSHV-IC8TEM (`src/HvRigControl/HvRigCat/icomlan/`) è scritto da zero in C++/Qt, basandosi su quanto pubblicato da:

* **wfview** — Elliott Liggett W6EL, Phil Taylor M0VSE e collaboratori (GPLv3)
* **kappanhang** — Norbert Varga HA2NON (MIT)
* **AetherSDR** — collaboratori del progetto (GPLv3)
* **Simon S53ZO** — IC7300MK2_Lan_Splitter e PiRO: comportamento specifico del 7300MK2 (solo LPCM 48 kHz, sequenza interna big-endian) e simulatore del MK2 usato per le prove

---

## English summary

Rig Control → `Icom LAN` (one entry, the radio model is detected from its CI-V address), Server = radio IP, Port = 50001, CI-V Port / Audio Port (empty = 50002 / 50003), LAN User / Password = a network user created on the radio. Sound → `Icom LAN Input` / `Icom LAN Output`. On the radio: Network Control ON, DATA MOD = LAN. CAT and 48 kHz audio both ways; FSK441/JTMS/JT6M/ISCAT are resampled to 44.1 kHz. One LAN session at a time. Tested against a protocol simulator; please report results on real radios.
