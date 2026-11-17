# QSO doppi nel log

Finestra **Log** › **Menu** › **Find And Delete Duplicate QSOs** (Trova e cancella i QSO doppi).

Sono doppi i QSO con **lo stesso nominativo, la stessa banda e lo stesso modo**, e in più, a scelta:

* **la stessa ora di inizio**: QSO identici, per esempio registrati due volte o importati due volte con *Add Log* / *Add ADIF To Log*;
* **inizio entro N minuti** (predefinito 10): lo stesso QSO salvato due volte a pochi minuti di distanza;
* **lo stesso giorno UTC**: tiene un solo QSO per nominativo, banda e modo in ogni giorno.

In ogni gruppo di doppi MSHV **tiene il QSO più completo** (con locator, rapporti, commento...). A parità, tiene il primo in ordine di tempo. Gli altri vengono selezionati nel log e compare la normale richiesta di conferma di *Delete Selected*: con **Cancel** non si cancella nulla e i QSO restano selezionati per controllarli.

Prima di cancellare conviene fare una copia: **Menu › Create Log Backup**.
