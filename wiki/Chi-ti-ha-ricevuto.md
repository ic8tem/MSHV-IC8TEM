# Chi ti ha ricevuto (PSKReporter)

Menu **Options › Text Highlight**, riquadro **PSKReporter**.

Nelle due liste delle decodifiche i nominativi delle stazioni che **ti hanno ricevuto** sulla banda in uso, secondo [PSKReporter](https://pskreporter.info), sono **sottolineati in giallo fluo**. Si vede a colpo d'occhio chi ti sente già: per esempio chi chiama CQ e ha appena decodificato la tua trasmissione.

* **Underline calls that heard me (PSKReporter)** (*Sottolinea i nominativi che mi hanno ricevuto*): attivo di serie; togliendo la spunta MSHV non interroga più PSKReporter.
* **COLOR**: colore della sottolineatura (giallo fluo di serie, uguale per il tema chiaro e per quello scuro; *SET DEFAULT COLORS* lo riporta al giallo). Sul tema chiaro sotto il giallo c'è un bordo più scuro, così la sottolineatura si vede bene anche sullo sfondo chiaro.
* Sotto la casella c'è lo stato, per esempio *Stazioni che ti hanno ricevuto in 144 MHz: 7 (PSKReporter, 21:35 UTC)*, oppure l'errore.
* La sottolineatura si aggiunge agli altri colori: un nominativo già collegato su un'altra banda resta rosa (vedi [[Colori: già collegati e locatori nuovi|Colori-gia-collegati]]) e in più viene sottolineato.

## Come funziona

* Il nominativo cercato è il tuo, quello di **Options › Macros** (*My Call*). MSHV chiede a PSKReporter i rapporti di ricezione del tuo nominativo degli **ultimi 30 minuti**.
* La prima richiesta parte circa **un minuto dopo l'avvio**, poi **ogni 5 minuti** (il minimo che PSKReporter chiede ai programmi) **solo mentre trasmetti**, o se hai trasmesso negli ultimi 30 minuti: senza trasmissioni non arrivano rapporti nuovi e PSKReporter non viene interrogato per niente.
* Un rapporto vale 35 minuti (i 30 della richiesta più i 5 fino alla successiva). PSKReporter restituisce al massimo gli ultimi 100 rapporti per richiesta: le risposte vengono **sommate**, così su una banda HF affollata nessuno sparisce prima dei 35 minuti. Se cambi nominativo nelle macro i dati vecchi vengono scartati.
* Nominativi con prefisso o suffisso: se ti ha ricevuto `IZ0ABC/P` vengono sottolineati sia `IZ0ABC/P` sia `IZ0ABC`, anche fra `< >`.
* PSKReporter raccoglie i rapporti inviati dai programmi delle altre stazioni (WSJT-X, JTDX, MSHV, skimmer SDR…) con qualche minuto di ritardo: la sottolineatura arriva dopo la ricezione, non in tempo reale, e solo per le stazioni che inviano i loro rapporti a PSKReporter.
* La richiesta contiene solo il nominativo cercato e un indirizzo di contatto del programma, come PSKReporter chiede a chi lo interroga in automatico. Serve la connessione a Internet (HTTPS: Qt con OpenSSL oppure `curl` di Windows 10/11).

## Tutte le bande

Funziona su **tutte le bande di MSHV, da 135 kHz a 248 GHz**: HF, 50 e 70 MHz, VHF, UHF e microonde. Conta la **banda in uso** in MSHV (quella scelta nel menu delle bande o letta dalla radio; con un transverter quella data dall'offset impostato in *Interface Control*): chi ti ha ricevuto in 50 MHz non viene sottolineato quando sei in 144 MHz. Cambiando banda la sottolineatura cambia subito, senza aspettare una nuova richiesta.

La banda di ogni rapporto:

* di norma è quella della **frequenza** inviata dalla stazione che ti ha ricevuto;
* se la frequenza **manca** o non cade in nessuna banda, è la banda su cui **stavi trasmettendo** in quel momento (MSHV ricorda le tue trasmissioni e la loro banda). Succede soprattutto **sopra i 4,29 GHz** (5,7 - 10 - 24 - 47 GHz…): i programmi che inviano la frequenza a PSKReporter in 4 byte non possono andare oltre quel valore;
* se la frequenza è la **media frequenza di un transverter** (28, 144, 432, 1296 o 2320 MHz) e tu stavi trasmettendo su una banda più alta, il rapporto vale per **tutte e due** le bande. Esempio: chi ti riceve in 10 GHz con un transverter a 144 MHz, senza offset impostato nel suo programma, ti segnala su 144 MHz, e viene sottolineato anche in 10 GHz. Vale anche al contrario, quando è il tuo MSHV a essere sulla media frequenza (per esempio 28 MHz) perché manca l'offset del transverter.

## Idea

Idea da **WsjtxUtils.Searchlight** di KC3PIB per WSJT-X (<https://github.com/KC3PIB/WsjtxUtils.Searchlight>, licenza MIT); in MSHV il codice è nuovo. Vedi [[Riconoscimenti|Credits]].
