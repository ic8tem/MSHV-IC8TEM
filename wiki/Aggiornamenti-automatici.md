# Aggiornamenti automatici

Dalla versione **2.76.7-ic8tem.6** MSHV-IC8TEM controlla da solo se su GitHub c'è una versione nuova.

## Quando controlla

* 90 secondi dopo l'avvio e poi **ogni 6 ore** finché il programma resta aperto.
* A mano: **Help › Check for Updates ...**
* Si disattiva togliendo la spunta **Help › Check for Updates Automatically (every 6 hours)**. La scelta viene salvata in `settings/ms_update`.

Il controllo automatico è silenzioso: se GitHub non risponde ritenta dopo 6 ore. Se in quel momento MSHV sta trasmettendo, la domanda viene rimandata di 2 minuti.

## Cosa succede

Se c'è una versione nuova compare una finestra con le novità (pulsante *Show Details*) e tre scelte:

* **Update now**: MSHV scarica il pacchetto adatto alla vostra installazione, controlla la sua impronta SHA256 con il file `SHA256SUMS.txt` della release, aspetta la fine di un'eventuale trasmissione, si chiude, installa e si riapre.
* **Later**: la domanda torna al prossimo controllo.
* **Skip this version**: non chiede più per quella versione, ma avvisa per la successiva.

Impostazioni, log, liste LoTW e registrazioni **restano**: stanno nella cartella dati, che l'aggiornamento non tocca.

## Come installa, per sistema

| Installazione | Come viene aggiornata |
|---|---|
| Windows con installatore (x64, x86, ARM64) | installatore in modalità silenziosa, nella cartella di quell'installazione (dalla 2.76.7-ic8tem.31); MSHV riparte da solo. Non servono permessi di amministratore |
| Windows zip portatile | i file del programma vengono sostituiti; le cartelle `settings`, `log`, `RxWavs`, `ExportLog`, `AllTxtMonthly`, `Screenshots` restano |
| macOS (Apple Silicon e Intel) | `MSHV-IC8TEM.app` viene sostituita e riaperta (la cartella dell'app deve essere scrivibile) |
| Linux AppImage | il file AppImage viene sostituito (la cartella deve essere scrivibile) |
| tar.gz Linux, BSD, Raspberry | i file del programma vengono sostituiti, le cartelle dati restano |
| .deb (Debian, Ubuntu, Mint, Raspberry Pi OS), .rpm (Fedora, openSUSE), .txz (Slackware) | il sistema chiede la **password di amministratore** (pkexec) e installa con il gestore pacchetti |

Se un'installazione non può aggiornarsi da sola (cartella non scrivibile, pkexec assente, programma compilato da voi) compare il pulsante **Open download page**.

## Più installazioni

Si possono avere più copie di MSHV-IC8TEM, ognuna nella sua cartella, per esempio una per l'IC-7610 e una per l'IC-9700. Dalla versione **2.76.7-ic8tem.31** ogni copia si aggiorna da sola, nella sua cartella:

* la finestra dell'aggiornamento mostra la cartella che verrà aggiornata (*Installation*);
* **Update now** chiude e aggiorna solo quella copia, poi la riapre con la stessa riga di comando (per esempio `--inst-name=`); l'altra copia resta aperta e chiede il suo aggiornamento per conto suo;
* le impostazioni, i log e la scelta *Skip this version* sono di ogni copia;
* con l'installatore, i collegamenti di una cartella diversa da `C:\MSHV-IC8TEM-NEW` si chiamano *MSHV-IC8TEM (nome della cartella)*.

Fino alla versione **.30**, con l'installatore, l'aggiornamento andava sempre in `C:\MSHV-IC8TEM-NEW`, anche se lo chiedeva una copia in un'altra cartella. Per questo una seconda copia va portata alla .31 **a mano, una volta**: chiudetela e copiate `MSHV-IC8TEM.exe` dallo zip della .31 nella sua cartella (non estraete tutto lo zip sopra, perché la sua cartella `settings` sostituirebbe le vostre impostazioni). Nella copia ancora alla .30 non premete *Update now*.

In *App installate* di Windows compare una sola voce MSHV-IC8TEM, quella aggiornata per ultima: ogni copia ha comunque il suo `unins000.exe` nella sua cartella.

## Sicurezza

* Si scarica solo da `github.com/ic8tem/MSHV-IC8TEM` in HTTPS.
* Il pacchetto viene installato solo se l'impronta SHA256 coincide con `SHA256SUMS.txt`.
* I pacchetti non sono firmati con un certificato: la sicurezza dipende anche dall'account GitHub del progetto (protetto con autenticazione a due fattori).

Su Windows, dove le librerie Qt di questa build non hanno OpenSSL, il collegamento usa `curl` di Windows 10/11.

Il primo aggiornamento automatico avviene dalla versione **successiva** alla 2.76.7-ic8tem.6: le versioni precedenti vanno aggiornate a mano una volta.
