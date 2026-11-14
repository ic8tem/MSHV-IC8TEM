# Doppler tracking (EME)

Nella finestra **Astronomical Data**, a destra, ci sono tre riquadri come in WSJT-X.

## Metodi

| Metodo | RX | TX | Note |
|---|---|---|---|
| **Full Doppler to DX Grid** | Sked + D | Sked − D | D = Doppler Io→Luna→DX. Il DX non corregge nulla |
| **Own Echo** | Sked + E | Sked | E = Doppler del proprio eco |
| **Constant Frequency on Moon (CFOM)** | Sked + E/2 | Sked − E/2 | il segnale arriva sulla Luna sempre su Sked |
| **On DX Echo** | Sked + D | Sked + eco del DX (fissato a inizio QSO) − D | il DX trasmette su Sked e ascolta il proprio eco |
| **Call DX** | come On DX Echo | | la Sked diventa la frequenza TX del DX calcolata da dove lo ascolti ora |
| **None** | Sked | Sked | nessuna correzione |

Le formule sono anche nei tooltip dei pulsanti.

## Frequenza Sked e Tx Shift

* Campo **kHz** + **Set kHz**: Sked = MHz attuali + kHz scelti (es. 432 + 090 = 432,090 000).
* **Rx / Tx**: frequenze effettive inviate alla radio.
* **Tx Shift**: da −999,999 a +999,999 MHz (passo 1 kHz), applicato al TX solo con **Enable**. Funziona anche con None.
* Echo Mode (RIT/Dither) è presente per somiglianza con WSJT-X ma disattivato: MSHV non ha il modo Echo.

## Come agisce sul CAT

* Attivo solo con la finestra Astronomical Data **aperta**, CAT attivo e metodo diverso da None (o Tx Shift abilitato).
* In ricezione la radio va sulla frequenza Rx; quando MSHV trasmette la frequenza Tx viene impostata **prima** del PTT; al ritorno in RX il PTT si apre e poi viene reimpostata la Rx (ripetuta dopo 350 ms).
* Aggiornamento ogni 2 s (variazioni ≥ 1 Hz), anche durante il TX (necessario per CFOM).
* Se giri il VFO la Sked segue (scarto > 50 Hz); se cambi banda o frequenza da MSHV, la nuova frequenza diventa la Sked.
* Chiudendo la finestra o scegliendo None la radio torna sulla Sked.
* Con il tracking attivo la funzione *Static TX* di MSHV è sospesa; l'offset del transverter di Rig Control resta applicato.
* Le impostazioni vengono salvate (tranne Call DX).

## Correzione

In `astro()` la tabella `xdop[]` era una variabile locale non inizializzata; ora è `static`.
