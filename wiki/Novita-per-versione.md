# Novità per versione

## 2.76.7-ic8tem.36
* **Decodifica Q65 come in WSJT-X 3.3.0-beta8**: gli spettri coprono tutto il periodo T/R, così gli ultimi simboli di un segnale in ritardo (delay EME) non vanno più persi. Nelle prove, Q65-60A con 2,5–3,5 s di ritardo a −31 dB: 4 decodifiche su 12 (la .35: 1 su 12).
* Di nuovo decodificati i **segnali in anticipo di 0,8 s o più** (Q65-30): 12 su 12 a −22 dB, la .35 nessuno. Era un difetto del porting: tutti i simboli finivano spostati di un posto.
* **Q65 Pileup**: l'elenco tiene **50 chiamanti** (erano 40); a elenco pieno esce quello sentito meno di recente. La lista a priori arriva a 511 messaggi. Vedi [[Q65 Pileup|Q65-Pileup]].
* Con un corrispondente che finisce con **/R o /P** la lista a priori contiene anche i messaggi con il nominativo tra `< >` (per esempio `<IC8TEM> DL3WDG/R RR73`: 3 su 3, la .35 1 su 3).
* Correzione: dalla .34 la lista a priori del corrispondente conteneva 205 copie dello stesso messaggio; ora sono 206 messaggi diversi come in WSJT-X, e la decodifica Q65 è più veloce.
* **Decodificatori più veloci, stesse decodifiche**: la versione Windows 64 bit è compilata con più ottimizzazioni (-O3). FT8 circa un quarto più veloce, Q65 e MSK144 dal 5 al 10%. Dopo aver usato Q65 il programma occupa circa 68 MB di RAM in meno. FT8, FT4, MSK144 e JT65B danno le stesse decodifiche della .35.

## 2.76.7-ic8tem.35
* *Rig Control*: dopo una prova con una radio di rete o Hamlib, la radio normale su porta seriale (per esempio **Icom IC-9700**) parte pulita: la riga rossa della radio precedente (per esempio *Hamlib: COM3 115200: Access denied*) torna a *Disconnected*, il test PTT segue la porta e MSHV riapre da solo la porta indicata in *Port*. Prima, scegliendo di nuovo la stessa porta, non si riapriva.
* La barra **Txing** è **grigia in ricezione** e rossa solo in trasmissione: verde sembrava una trasmissione in corso. Il resto della barra ha di nuovo il colore dei pulsanti (era nero).

## 2.76.7-ic8tem.34
* **Q65 Pileup** come in WSJT-X 2.7 e 3.2, per le DXpedition EME con tanti chiamanti deboli: **Mode › Q65 › Q65 Pileup**, oppure **clic destro su un pulsante Q65**. Vedi [[Q65 Pileup|Q65-Pileup]].
* Finestra **Q65 Pileup - Callers**: gli ultimi 40 chiamanti (24 ore) con frequenza, nominativo, locatore, **elevazione della Luna** al loro locatore ed età; **clic = risposta con Tx3**, Ctrl+clic = togli dall'elenco.
* L'elenco alimenta la decodifica a priori; escono i chiamanti con la Luna più di 5° sotto l'orizzonte e le stazioni appena registrate nel log.
* Segno **`#`** (WSJT-X 3.2): una decodifica `Q0#` viene da una stazione che ha ricevuto la tua trasmissione precedente; con il Pileup acceso MSHV manda lo stesso segno alla stazione in TO RADIO quando l'ha decodificata nel periodo appena finito. Con due liste delle decodifiche la colonna *Type* compare durante il Pileup.
* I messaggi con `#` dei chiamanti in elenco ora si decodificano anche con l'elenco: nelle prove 8 su 8 fino a −28 dB (con la .33: 0 su 8 a −28 dB).

## 2.76.7-ic8tem.33
* **Chi ti ha ricevuto (PSKReporter) su tutte le bande**, da 135 kHz a 248 GHz: HF, VHF, UHF e microonde. Vedi [[Chi ti ha ricevuto|Chi-ti-ha-ricevuto]].
* Correzione: in **70 MHz** (la banda con cui MSHV parte) la sottolineatura non si attivava dopo un riavvio, finché non si cambiava banda.
* Un rapporto **senza frequenza**, o con una frequenza fuori da ogni banda (succede sopra i 4,29 GHz con i programmi che inviano la frequenza in 4 byte: 5,7 - 10 - 24 GHz…), conta per la banda su cui stavi trasmettendo in quel momento.
* Un rapporto sulla **media frequenza di un transverter** (28, 144, 432, 1296, 2320 MHz) mentre trasmetti su un'altra banda conta per tutte e due.
* Le risposte di PSKReporter (al massimo gli ultimi 100 rapporti) vengono sommate: su una banda HF affollata nessuno sparisce prima dei 35 minuti.

## 2.76.7-ic8tem.32
* **Chi ti ha ricevuto (PSKReporter)**: nelle liste delle decodifiche i nominativi delle stazioni che ti hanno ricevuto sulla banda in uso negli ultimi 30 minuti, secondo PSKReporter, sono **sottolineati in giallo fluo**. Vedi [[Chi ti ha ricevuto|Chi-ti-ha-ricevuto]].
* *Options › Text Highlight*, riquadro **PSKReporter**: casella per accenderlo o spegnerlo (acceso di serie), tasto **COLOR** per il colore della sottolineatura, stato (quante stazioni ti hanno ricevuto e ora dell'ultimo aggiornamento).
* PSKReporter viene interrogato circa un minuto dopo l'avvio, poi ogni 5 minuti solo mentre trasmetti (o fino a 30 minuti dopo l'ultima trasmissione).
* Idea da *WsjtxUtils.Searchlight* di KC3PIB (per WSJT-X); il codice in MSHV è nuovo.

## 2.76.7-ic8tem.31
* **Aggiornamento automatico con più installazioni** (per esempio una per radio, ognuna nella sua cartella): ora ogni installazione aggiorna **la propria cartella**. Prima, con l'installatore di Windows, l'aggiornamento finiva sempre in `C:\MSHV-IC8TEM-NEW`: una seconda installazione in un'altra cartella non veniva mai aggiornata, e chiedendo l'aggiornamento da lì si chiudeva e riapriva l'MSHV dell'altra cartella. Vedi [[Aggiornamenti automatici|Aggiornamenti-automatici]].
* MSHV riparte con la stessa riga di comando (per esempio `--inst-name=`). Due installazioni aperte insieme non si cancellano più a vicenda il file scaricato.
* L'installatore dà ai collegamenti di una seconda cartella un nome proprio, per esempio *MSHV-IC8TEM (MSHV-NEW-9700)*, così non prende quelli della prima, e non copia mai le impostazioni di un'installazione in un'altra.
* **Una volta sola**: una seconda installazione fuori da `C:\MSHV-IC8TEM-NEW` va portata alla .31 a mano (basta copiare `MSHV-IC8TEM.exe` dallo zip nella sua cartella); dalla .31 in poi si aggiorna da sola.

## 2.76.7-ic8tem.30
* **IC-7300, IC-7610 e IC-9700 (Hamlib)**: corretto. Su un PC senza Hamlib installato la riga di Rig Control restava rossa con "Hamlib: cannot load libhamlib-4.dll ... Impossibile trovare il modulo specificato". La libreria di Hamlib ha bisogno di `libusb-1.0.dll`, che sta accanto a lei nella cartella `hamlib`, ma Windows la cercava solo nella cartella del programma. Ora MSHV carica Hamlib in modo che Windows cerchi le sue DLL nella cartella `hamlib`, e ogni compilazione per Windows prova che la libreria si carichi davvero. Vedi [[IC-7300, IC-7610, IC-9700 con Hamlib|IC-7300-Hamlib]].

## 2.76.7-ic8tem.29
* **Potenza e ROS con il CAT di MSHV, senza Hamlib**: corretto. Le radio Yaesu, Kenwood ed Elecraft dell'elenco (dalla versione .20) non venivano riconosciute, perché MSHV confrontava il nome con il marchio davanti ("Yaesu FT-991") e per le Elecraft non registrava il tipo di radio: la finestra restava "non disponibile". Ora funzionano.
* **Nuove radio** nella finestra Potenza e ROS: Yaesu **FT-450**, **FT-450D** e **FTDX-9000**; Kenwood **TS-890S** (potenza, Po in watt, ROS, ALC), **TS-990S** (potenza, ROS, ALC), **TS-870S** (potenza, ROS o ALC), **TS-850**, **TS-690S**, **TS-450S** (ROS o ALC); **PowerSDR** e **Thetis** (potenza, Po, ROS). Da provare: BG2FX FX-4CR, Hilberling PT-8000A, SDR Console. Comandi e scale come in Hamlib. Vedi [[Potenza e ROS|Potenza-e-ROS]].
* Po (e ROS con PowerSDR/Thetis) si legge durante la trasmissione anche quando il PTT è su RTS o DTR: prima solo con il PTT via CAT.

## 2.76.7-ic8tem.28
* **Ricerca del locatore**: se il nominativo in TO RADIO non è nel messaggio cliccato né nel database di MSHV, MSHV cerca il locatore nelle decodifiche degli ultimi 20 periodi (quello che la stazione stessa ha trasmesso), poi nelle liste **CALL3.TXT di WSJT-X Improved**, **CALL3.TXT EME di W7GJ** e **grid_data.bin di JTDX**, che scarica e aggiorna da solo ogni 14 giorni, e infine **in rete** (G7VRD). L'etichetta accanto a LOCATOR dice la fonte (`RX:FM05`, `C3:`, `EME:`, `JTDX:`, `WEB:`); col mouse sopra si vedono tutte. Impostazioni in *Options › Network Configuration › Page 4*. Vedi [[Ricerca del locatore|Ricerca-locatore]].
* **Ripartenza della TX**: nuova opzione *TO RADIO Call Decoded Again Sets Auto Is On* (menu *Decode Lists Options*, spenta di serie). Se la TX si è fermata e il nominativo in TO RADIO viene decodificato di nuovo, MSHV riaccende AUTO e lo richiama nel periodo giusto (al massimo 3 volte per nominativo, non con il blocco ROS). Vedi [[Ripartenza della TX|Ripartenza-TX]].
* **IC-7610 e IC-9700**: potenza, Po, ROS e ALC nella finestra Potenza e ROS funzionano con il CAT di MSHV (USB e LAN; provato con i simulatori di Hamlib). Nuove voci **Icom IC-7610 (Hamlib)** e **Icom IC-9700 (Hamlib)** in Rig Control (Windows), come l'IC-7300. Con l'IC-9700 la potenza massima si adatta alla banda (100 W in 2 m, 75 W in 70 cm, 10 W in 23 cm). Vedi [[IC-7300, IC-7610, IC-9700 con Hamlib|IC-7300-Hamlib]].

## 2.76.7-ic8tem.27
* **Lista decodifiche**: le colonne si possono stringere o allargare trascinando il loro bordo nell'intestazione (doppio clic sul bordo: larghezza adatta al testo). Utile con i caratteri grandi. Le larghezze restano salvate, seguono la grandezza dei caratteri e sono le stesse nelle due liste; *Default Column Widths* nel menu della lista le riporta a quelle automatiche.
* **Riga tratteggiata** con l'ora UTC per ogni periodo decodificato senza nessuna decodifica (tutti i modi, anche MSK144 in tempo reale); si spegne con *Dashed Line For Periods With No Decodes*. Vedi [[Lista decodifiche: colonne e periodi vuoti|Lista-decodifiche]].

## 2.76.7-ic8tem.26
* **Icom IC-7300 (Hamlib)**: corretto. Dopo il primo comando alla radio (tasto banda, **F**, cambio di modo o trasmissione) MSHV smetteva di leggere la frequenza: la radio cambiava banda, ma MSHV restava fermo sulla frequenza letta alla connessione. Ora frequenza e modo vengono letti di continuo, come con le altre radio, e MSHV segue anche i cambi fatti dalla radio.
* **Icom LAN**: stessa correzione (dopo la prima trasmissione o il primo cambio di banda la frequenza non veniva più letta).

## 2.76.7-ic8tem.25
* **Potenza e ROS**: la finestra è molto più piccola. Prende la misura della scheda aperta: la scheda *Power and SWR* occupa solo lo spazio che le serve (prima era grande quanto la scheda *Settings*), *Settings* e *CAT Log* la ingrandiscono quando le aprite e, tornando alla prima scheda, si rimpicciolisce di nuovo. Vedi [[Potenza e ROS|Potenza-e-ROS]].
* Scritta della protezione più corta (*Stop TX if SWR > 2.5 for 1.0 s*); il messaggio rosso del blocco per ROS alto va sotto i due riquadri, a tutta larghezza.

## 2.76.7-ic8tem.24
* **Icom IC-7300 (Hamlib)**: corretto. Dopo una riapertura della porta (all'avvio, o cambiando Port o Baud Rate) lo stato era verde "Connected" ma MSHV scartava tutti i comandi: niente lettura della frequenza, i tasti banda e **F** non cambiavano la frequenza, niente Potenza e ROS. Ora alla connessione la frequenza viene letta subito e scritta nel CAT Log.
* **Icom IC-7300 (Hamlib)**: se la porta non si apre, la scritta rossa dice l'errore vero in una riga (prima c'era il registro interno di Hamlib, tagliato) e cosa controllare: Baud Rate uguale a quella della radio, porta COM inesistente o usata da un altro programma. Non riapre più la porta se Port e Baud Rate non sono cambiati, e se l'apertura non riesce riprova una volta.
* Ricordati: con Hamlib la porta della radio è usata da Hamlib; la trasmissione passa dal comando CAT oppure dalla **Port 2** (per esempio COM2 con PTT Via RTS).

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
