# MSHV-IC8TEM — wiki

Versione modificata di **MSHV 2.76.7** di Hrisimir Hristov, **LZ2HV**, con aggiunte di Costantino Cerrotta, **IC8TEM**.
Non è l'MSHV ufficiale: i problemi di questa versione vanno segnalati a IC8TEM. MSHV ufficiale: <https://sourceforge.net/projects/mshv/>

**Download:** <https://github.com/ic8tem/MSHV-IC8TEM/releases> — guida: [[Installazione]]

## Funzioni nuove

| Funzione | Dove si trova | Pagina |
|---|---|---|
| Doppler tracking EME (Full Doppler, Own Echo, CFOM, On DX Echo, Call DX), frequenza Sked, Tx Shift ±999,999 MHz, comandi CAT | Finestra *Astronomical Data* | [[Doppler tracking|Doppler-tracking]] |
| Utenti LoTW: lista ARRL, scaricamento diretto, pallino nero nelle decodifiche | *Options › Network Configuration › Page 3* | [[LoTW]] |
| Filtro "Show LoTW Users Only" e "Hide QSO B4 Messages" | *Decode List Filters* | [[Filtri]] |
| Annullamento fittizio del log (data UTC) | Finestra *Log* | [[Annullamento fittizio del log|Annullamento-fittizio-del-log]] |
| Sblocco TX Slots (acceso di default) | *Multi Answer Auto Seq Protocol* | [[Multi Answer TX Slots|Multi-Answer-TX-Slots]] |
| Colonna Width (durata del ping) in MSK144 | Menu della lista decodifiche | [[Durata del ping MSK144|Durata-ping-MSK144]] |
| Colonne della lista più strette o più larghe (trascinando il bordo) e riga tratteggiata per i periodi senza decodifiche | Lista decodifiche e suo menu | [[Lista decodifiche: colonne e periodi vuoti|Lista-decodifiche]] |
| **Chi ti ha ricevuto**: nominativi sottolineati in giallo fluo se ti hanno ricevuto su questa banda negli ultimi 30 minuti, secondo PSKReporter; tutte le bande, da 135 kHz a 248 GHz (HF, VHF, UHF, microonde) | *Options › Text Highlight*, riquadro *PSKReporter* | [[Chi ti ha ricevuto|Chi-ti-ha-ricevuto]] |
| **Q65 Pileup** (WSJT-X 2.7 / 3.2) per i pileup EME: finestra dei chiamanti con elevazione della Luna, clic = risposta Tx3, decodifica a priori, segno `#` (la stazione ha ricevuto la tua trasmissione precedente) in ricezione e in trasmissione | *Mode › Q65 › Q65 Pileup* o clic destro su un pulsante Q65 | [[Q65 Pileup|Q65-Pileup]] |
| **Ricerca del locatore** quando il nominativo non è nel database: decodifiche degli ultimi 20 periodi, CALL3 (WSJT-X Improved e W7GJ EME), grid_data.bin di JTDX scaricati da soli, G7VRD in rete; l'etichetta dice la fonte | Accanto a *LOCATOR*; *Options › Network Configuration › Page 4* | [[Ricerca del locatore|Ricerca-locatore]] |
| **Ripartenza della TX** quando il nominativo in TO RADIO viene decodificato di nuovo | *Options › Decode Lists Options* | [[Ripartenza della TX|Ripartenza-TX]] |
| **Icom LAN**: IC-7300MK2, IC-705, IC-9700, IC-7610, IC-905 via rete con CAT e audio | *Rig Control* e *Sound Settings* | [[Icom LAN|Icom-LAN]] |
| **Icom IC-7300 / IC-7610 / IC-9700 (Hamlib)** (Windows) | *Rig Control* | [[IC-7300, IC-7610, IC-9700 con Hamlib|IC-7300-Hamlib]] |
| **Potenza e ROS**: potenza di uscita, Po, ROS, ALC, memoria per banda, protezione ROS (Icom USB e LAN) | *Options › View Power and SWR* | [[Potenza e ROS|Potenza-e-ROS]] |
| **Aggiornamenti automatici** ogni 6 ore, installazione con un clic | *Help › Check for Updates* | [[Aggiornamenti automatici|Aggiornamenti-automatici]] |
| Pulsanti dei modi all'avvio: FT8, FT4, Q65B, Q65C, Q65A, MSK144, JT65B | Barra dei modi | — |
| Correzioni (Doppler `astro()`, puntatori FFT, tasto Enter sul tuner Flex) | — | [[Novità per versione|Novita-per-versione]] |

## Altre pagine

* [[Installazione]] — tutte le distribuzioni (Windows, Linux, Raspberry Pi, BSD, macOS) e dove finiscono le impostazioni
* [[Novità per versione|Novita-per-versione]]
* [[Riconoscimenti|Credits]] — codice e conoscenze riprese da altri autori

Le pagine sono mantenute anche nella cartella `wiki/` del repository.
