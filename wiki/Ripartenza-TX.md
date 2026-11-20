# Ripartenza della TX quando il corrispondente torna

Dalla versione **2.76.7-ic8tem.28**: menu *Options › Decode Lists Options › TO RADIO Call Decoded Again Sets Auto Is On* (spenta di serie). Modi MSK144, FT8, FT4, FT2, JT65 e Q65.

Se la trasmissione si è fermata (TX watchdog, AUTO spento a mano, propagazione sparita) e il nominativo che avete in **TO RADIO** viene decodificato di nuovo come stazione che trasmette, per esempio `CQ WA2LMC FM05`, `W1AB WA2LMC RR73` o `IC8TEM WA2LMC -05`, MSHV riaccende **AUTO** e ricomincia a chiamarlo con il messaggio TX selezionato. Il periodo di trasmissione diventa quello opposto al suo, se il riconoscimento del periodo è attivo.

La TX riparte solo se:

* il messaggio TX selezionato è per lui e non chiude il QSO (non `RR73`, `73`, `RRR` e non CQ);
* è lui a trasmettere: un messaggio di chi lo chiama (`WA2LMC W1XYZ -10`) non conta;
* la protezione ROS della finestra Potenza e ROS non ha bloccato la trasmissione;
* non siete in Multi Answer.

Al massimo **3 ripartenze automatiche** per lo stesso nominativo; poi serve il pulsante **AUTO**, che azzera il conteggio. Il TX watchdog vale anche dopo ogni ripartenza.

Per smettere di chiamarlo: togliete il nominativo da TO RADIO, selezionate un altro messaggio (per esempio CQ) o spegnete l'opzione.
