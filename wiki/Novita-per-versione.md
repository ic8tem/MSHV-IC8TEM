# Novità per versione

## 2.76.7-ic8tem.11
* Nuovi pacchetti: **rpm ARM64** (Fedora/openSUSE ARM), **Alpine Linux / musl** x86_64 e ARM64, **zip per macOS**. Nella pagina di ogni release c'è la tabella "Scarica la versione per il tuo sistema", con anche le righe per Windows 7/8/8.1.

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
