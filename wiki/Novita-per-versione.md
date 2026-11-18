# Novità per versione

## 2.76.7-ic8tem.24
* **Icom IC-7300 (Hamlib)**: corretto. Dopo una riapertura della porta (all'avvio, o cambiando Port o Baud Rate) lo stato era verde "Connected" ma MSHV scartava tutti i comandi: niente lettura della frequenza, i tasti banda e **F** non cambiavano la frequenza, niente Potenza e ROS. Ora alla connessione la frequenza viene letta subito e scritta nel CAT Log.

## 2.76.7-ic8tem.23
* **MSK144 Noise Blanker** (menu Decode): toglie i disturbi impulsivi dall'audio della radio senza toccare i ping. Due metodi: NB2 di NR0V (consigliato) e predizione lineare di DL2FW. Spento di serie.
* **MSK144 Carrier Notch (ZAP)** (menu Decode): toglie le portanti fisse e le spurie con tacche di 10 Hz; non interviene sui segnali MSK continui. Spento di serie. Vedi [[Rumore MSK144|Rumore-MSK144]].

## 2.76.7-ic8tem.22
* **Text Highlight**: nuovo colore **rosa** per i nominativi già nel log ma collegati su altre bande o modi.
* **Locatori**: nuova opzione **New grids only** (attiva): l'azzurro segna solo i locatori non ancora collegati, invece di quelli già collegati. Vedi [[Colori: già collegati e locatori nuovi|Colori-gia-collegati]].
* Il calcolo dei già collegati con log molto grandi è più veloce.

## 2.76.7-ic8tem.21
* **Log › Menu › Find And Delete Duplicate QSOs**: trova i QSO doppi (stesso nominativo, banda e modo; stessa ora, entro N minuti o stesso giorno UTC), tiene il più completo e cancella gli altri dopo la conferma. Vedi [[QSO doppi|QSO-doppi]].
* La cancellazione di molti QSO dal log è molto più veloce.

## 2.76.7-ic8tem.20
* **Windows**: l'installatore mette MSHV in **C:\MSHV-IC8TEM-NEW**. Impostazioni e log della cartella precedente vengono copiati una volta nella nuova; la vecchia cartella resta e si può cancellare.
* **Potenza e ROS** anche con il CAT di MSHV per **Yaesu** (FT-991/991A, FT-DX10, FT-710, FT-DX101, FT-891, FT-DX1200, FT-DX3000, FT-DX5000, FT-950, FT-2000, FTX-1), **Kenwood** (TS-2000, TS-590S, TS-590SG, TS-480) ed **Elecraft K3/K3S/KX3/KX2** (radio "K3/KX3"). Vedi [[Potenza e ROS|Potenza-e-ROS]].
* Nuova radio **Elecraft K4** (K4, K4D, K4HD): con un solo comando (`TM;`) si leggono Po, ROS e ALC.
* **Potenza e ROS** anche con **TCI** (potenza = drive; Po e ROS mandati dalla radio durante la trasmissione) e **FlexRadio 6xxx** (rfpower, strumenti FWDPWR e SWR).

## 2.76.7-ic8tem.19
* **Icom IC-7300 (Hamlib)** usa i normali campi **Port** e **Baud Rate** di Rig Control: MSHV avvia Hamlib sulla porta COM scelta e si ricollega quando la cambiate. MSHV non si blocca più mentre Hamlib aspetta la radio; i messaggi lunghi di Hamlib vanno nel CAT Log.

## 2.76.7-ic8tem.18
* Corretto: nella .17 la radio **Icom IC-7300 (Hamlib)** non compariva nell'elenco di Rig Control (Windows).
* La barra **Txing** si riempie da sinistra a destra con lo scorrere del periodo: verde in ricezione, rossa in trasmissione.

## 2.76.7-ic8tem.17
* Nuova radio **Icom IC-7300 (Hamlib)** per Windows: l'IC-7300 comandato con la libreria Hamlib (frequenza, modo, PTT, potenza, Po/ROS/ALC). Vedi [[IC-7300 con Hamlib|IC-7300-Hamlib]].
* **JT65 Deep Search**: la ricerca profonda usa al massimo 15.000 nominativi del database (CALL3.TXT). Prima vengono quelli sentiti nelle ultime 24 ore in qualsiasi modo, dal più recente; poi gli altri in ordine alfabetico. Così con un CALL3.TXT grande non restano esclusi i nominativi che stai sentendo.

## 2.76.7-ic8tem.16
* Corretto: **Potenza e ROS** con le Icom collegate via USB (IC-7300, IC-7610...) tramite il CAT di MSHV. La finestra diceva "non disponibile" anche con il CAT attivo, quindi non regolava la potenza e non leggeva Po, ROS e ALC.
* Icom (USB e LAN): le risposte di frequenza e modo non vengono più scartate quando arrivano in ritardo. Con Icom LAN MSHV restava fermo su una frequenza vecchia.
* **CAT Log**: ora mostra tutti i comandi CI-V (frequenza, modo, PTT), segnala una risposta con indirizzo CI-V diverso da quello atteso e, con Icom LAN, quanto audio di trasmissione è arrivato alla radio (con l'avviso se MSHV non manda audio a "Icom LAN Output").

## 2.76.7-ic8tem.15
* **Potenza e ROS anche senza il CAT di MSHV**: in *Impostazioni › Connessione* si sceglie **Hamlib rigctld** (qualsiasi radio supportata da Hamlib) o **Icom CI-V su porta seriale propria**. Il CAT Log mostra anche i comandi di rigctld. Vedi [[Potenza e ROS|Potenza-e-ROS]].

## 2.76.7-ic8tem.14
* **Nessun limite di QSO nel Log**: sparisce l'avviso "The maximum number of QSOs in Log is 50000" e non serve più cancellare i QSO vecchi.

## 2.76.7-ic8tem.13
* **Unlock TX Slots acceso di default** (Multi Answer): TX Slots non viene più bloccato a 1 in FT8/FT4 HF. Anche le impostazioni salvate con le versioni precedenti partono sbloccate; chi vuole i limiti originali toglie la spunta (la scelta poi resta salvata).
* Pagina delle release senza tabella: i file si scaricano dalla sezione Assets, solo da github.com.

## 2.76.7-ic8tem.12
* Potenza e ROS: nuova scheda **CAT Log** con i comandi CI-V inviati alla radio e le risposte (in esadecimale e tradotte), pulsante **Leggi ora**; in alto la radio scelta, il tipo di CAT e se è attivo. Corretto: la finestra poteva restare "non disponibile" se il CAT partiva prima della finestra.

## 2.76.7-ic8tem.11
* Nuovi pacchetti: **rpm ARM64** (Fedora/openSUSE ARM), **Alpine Linux / musl** x86_64 e ARM64, **zip per macOS**. 

## 2.76.7-ic8tem.10
* Nuova finestra **Potenza e ROS** (Options › View Power and SWR): potenza di uscita, Po, ROS, ALC, memoria per banda e per modo, Tune a potenza ridotta, protezione ROS che ferma il TX. Radio Icom via USB o LAN. Tradotta nelle lingue di MSHV.

## 2.76.7-ic8tem.9
* **MSK144-WB**: il doppio clic su una decodifica di MSK144-WB (decodificatore MSK144/FSK441 a larga banda su SDRangel) porta MSHV sul modo (MSK144 o FSK441), sulla frequenza (spostamento del transverter compreso), imposta nominativo, locatore e periodo e accende il TX. In UDP Broadcast Settings: server 127.0.0.1, porta 2237.

## 2.76.7-ic8tem.8
* Icom LAN: una sola voce **Icom LAN** per tutte le radio (il modello si riconosce dall'indirizzo CI-V); campi **CI-V Port** e **Audio Port** per cambiare le porte UDP.

## 2.76.7-ic8tem.7
* macOS: corretto l'avvio (mancava una libreria nel pacchetto, l'app si chiudeva subito). Chi ha la 2.76.7-ic8tem.6 su Mac deve installare questa a mano.

## 2.76.7-ic8tem.6
* **Aggiornamenti automatici**: controllo ogni 6 ore, installazione con un clic su tutti i sistemi. Vedi [[Aggiornamenti automatici|Aggiornamenti-automatici]].

## 2.76.7-ic8tem.5
* LoTW: link cliccabile a <https://lotw.arrl.org/lotw-user-activity.csv> e pulsante **Download from ARRL and Load**.
* Dove Qt non ha OpenSSL (Windows) lo scaricamento usa `curl` del sistema (Windows 10/11, macOS, Linux).

## 2.76.7-ic8tem.4
* **Icom LAN**: IC-7300MK2, IC-705, IC-9700, IC-7610, IC-905 via rete con CAT e audio. Vedi [[Icom LAN|Icom-LAN]].
* README: tabella dei riconoscimenti.

## 2.76.7-ic8tem.3
* Colonna **Width** (durata del ping) in MSK144. Vedi [[Durata del ping MSK144|Durata-ping-MSK144]].

## 2.76.7-ic8tem.2
* Installatori Windows (x64, x86, ARM64).
* Cartella dati alternativa se la cartella del programma non è scrivibile.
* Pubblicazione automatica delle release.

## 2.76.7-ic8tem.1
* Doppler tracking, LoTW, filtro LoTW, annullamento fittizio del log, sblocco TX Slots opzionale, pulsanti dei modi all'avvio.
* Correzioni da VU2CPL (puntatori FFT, tuner Flex) e porting macOS.
* 16 distribuzioni: Windows, Linux, Raspberry Pi, Slackware ARM, FreeBSD, OpenBSD, macOS.

Elenco completo dei file modificati: `CHANGES_IC8TEM.md` nel repository.
