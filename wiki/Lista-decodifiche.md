# Lista delle decodifiche: colonne e periodi vuoti

Dalla versione **2.76.7-ic8tem.27**.

## Larghezza delle colonne

Le colonne della lista (*Time*, *dB*, *DT*, *Freq*…) si possono stringere o allargare: trascinate il bordo della colonna nell'intestazione. Un doppio clic sul bordo adatta la colonna al testo. La colonna del messaggio prende lo spazio che resta.

* Utile con i caratteri grandi (*Options › Font*): le colonne non occupano più spazio del necessario.
* La larghezza resta in percentuale della larghezza automatica, quindi segue la grandezza dei caratteri.
* Vale per tutti i modi con le stesse colonne (FT8, FT4, FT2 e Q65 insieme; MSK144; JT65) ed è la stessa nelle due liste (*Band Activity* e *RX Frequency*).
* Le larghezze sono salvate alla chiusura di MSHV.
* Per tornare alle larghezze automatiche del modo in uso: menu della lista decodifiche › **Default Column Widths**.

## Riga tratteggiata per i periodi senza decodifiche

Quando un periodo in ricezione viene decodificato e non dà nessuna decodifica, nella lista compare una riga con l'ora UTC del periodo e una linea tratteggiata:

    185230  -1  1.5  CE2SV HD4W RR73      1587
    185245  - - - - - - - - - - - - - - - - - -
    185300   0  1.5  DL5NAM HD4W +03      1585

* Vale per tutti i modi (FT8, FT4, Q65, MSK144 anche con la decodifica in tempo reale, JT65…), solo nella lista *Band Activity*.
* Nessuna riga per i periodi in cui trasmettete, con *Monitor* spento, senza *Auto Dec*, per le decodifiche fatte con il mouse su una parte del periodo e per i file aperti.
* Con la decodifica FT8 in tre tempi la riga arriva solo dopo l'ultimo; se una decodifica dello stesso periodo arriva dopo la riga, la riga sparisce.
* L'intestazione mostra *Message 0*.
* Si spegne dal menu della lista decodifiche: **Dashed Line For Periods With No Decodes** (acceso di default).
