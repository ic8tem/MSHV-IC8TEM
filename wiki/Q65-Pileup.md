# Q65 Pileup

Menu **Mode › Q65 › Q65 Pileup**, oppure **clic destro su un pulsante Q65** della barra dei modi.

È la funzione di WSJT-X (dalla 2.7.0) pensata per le **DXpedition EME**, per esempio in 6 m, con tanti chiamanti debolissimi: il programma ricorda chi ti ha chiamato e usa quell'elenco per decodificare meglio i messaggi successivi delle stesse stazioni. Dalla 3.2.0-rc1 WSJT-X usa anche un bit del messaggio Q65 rimasto libero per dire *"ho ricevuto la tua trasmissione precedente"*: quelle decodifiche sono marcate con **`#`**. MSHV-IC8TEM fa le due cose come WSJT-X.

MSHV aveva già, dalla 2.72, l'attività *Pileup* (in *Options › Macros*, *Activity Type*) con l'elenco dei chiamanti e la decodifica a priori. La versione .34 aggiunge il comando nel menu e sul pulsante, la finestra dei chiamanti, l'elevazione della Luna e il segno `#`.

## Accendere e spegnere

* **Mode › Q65 › Q65 Pileup** (casella): accesa porta MSHV in **Q65A** se non eri già in un modo Q65, sceglie l'attività **Pileup** e apre la finestra dei chiamanti; spenta torna all'attività che avevi prima.
* **Clic destro su un pulsante Q65** (Q65A, Q65B, Q65C, Q65D) della barra dei modi: da un altro modo passa a quel Q65 e accende il Pileup; se sei già in Q65 lo accende o lo spegne.
* L'attività resta salvata: alla riapertura di MSHV il Pileup è ancora acceso.
* In Pileup i messaggi sono quelli con il locatore, per esempio Tx3 `K1ABC IC8TEM R JN70`.

## La finestra dei chiamanti

**Mode › Q65 › Q65 Pileup - Callers...** la apre in qualsiasi momento.

| Colonna | Contenuto |
|---|---|
| N | numero d'ordine |
| Freq | frequenza audio dell'ultima decodifica (Hz); l'elenco è ordinato per frequenza |
| Call | nominativo del chiamante |
| Grid | locatore (4 caratteri) |
| El | **elevazione della Luna** al locatore del chiamante, in gradi; le righe con la Luna sotto l'orizzonte sono in grigio |
| Age (h) | ore dall'ultima decodifica |
| # | il chiamante ha ricevuto la tua trasmissione precedente |

* **Clic** su un chiamante: risposta con **Tx3** (`suo nominativo, tuo nominativo, R, tuo locatore`). La frequenza di ricezione va sulla sua; il periodo di trasmissione (l'altro rispetto al suo, con *Recognize Period* acceso, come di serie) e AUTO si regolano come con il doppio clic nelle liste.
* **Ctrl+clic**: toglie il chiamante dall'elenco.
* **Refresh** aggiorna, **Erase list** cancella tutto l'elenco (con conferma).
* La stazione in **TO RADIO** è in grassetto. Sotto l'elenco c'è il numero dei chiamanti e di quelli con `#`.
* Posizione e dimensioni della finestra vengono ricordate.

## L'elenco

* Al massimo **40 chiamanti**, tenuti **24 ore**; a elenco pieno, per far posto a uno nuovo esce il primo entrato.
* Entrano le stazioni decodificate mentre chiamano con il locatore (per esempio `IC8TEM K1ABC FN42` oppure `IC8TEM K1ABC R FN42`), solo dalle decodifiche dal vivo, non da un file aperto. Come in WSJT-X non entrano i nominativi composti (`/P`, `/MM`…) né quelli di più di 6 caratteri.
* L'elenco alimenta la **decodifica a priori** (tipo `Q3` nella colonna *Type*): i messaggi successivi dei chiamanti si decodificano qualche dB più in basso.
* Con il Pileup acceso, a ogni decodifica escono i chiamanti con la **Luna più di 5° sotto l'orizzonte** al loro locatore: non possono più sentirti. L'elevazione è calcolata per il centro del quadrato con le formule della Luna già presenti in MSHV (scarto di circa mezzo grado rispetto alle effemeridi; WSJT-X usa le effemeridi JPL).
* Quando **registri il QSO** nel log, il nominativo esce dall'elenco. Se ti richiama, rientra.

## Il segno `#`

* **In ricezione**: una decodifica marcata `Q0#` (colonna *Type*) viene da una stazione che ha ricevuto la tua trasmissione precedente. Con due liste delle decodifiche la colonna *Type* compare solo con il Pileup acceso; con una lista c'è sempre. Il `#` compare anche nella finestra dei chiamanti; una decodifica successiva della stessa stazione senza `#` lo toglie.
* **In trasmissione**: con il Pileup acceso, quando nel periodo appena finito hai decodificato la stazione in **TO RADIO**, la tua trasmissione successiva porta lo stesso segno, e nella lista la tua riga TX mostra `#`. Il segno vale per una sola trasmissione.
* I messaggi con il segno si decodificano anche con l'elenco dei chiamanti: MSHV prova per ogni messaggio sia la forma con il bit a 0 sia quella con il bit a 1. Nelle prove, un messaggio con `#` di un chiamante in elenco: 8 su 8 a −26, −27 e −28 dB (MSHV .33: 6, 4 e 0 su 8); i messaggi senza `#` come prima.
* Le versioni precedenti di MSHV e di WSJT-X decodificano comunque i messaggi con il segno (lo ignorano), ma non con l'elenco dei chiamanti.

## Origine

Funzione e algoritmi di **WSJT-X 2.7.0 e 3.2.0-rc1** di Joe Taylor K1JT e del WSJT Development Group (GPLv3), portati in MSHV; la prima parte (attività *Pileup* ed elenco) è di LZ2HV in MSHV 2.72. Vedi [[Riconoscimenti|Credits]].
