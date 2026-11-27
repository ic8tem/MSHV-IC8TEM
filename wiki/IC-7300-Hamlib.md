# IC-7300, IC-7610 e IC-9700 con Hamlib

Dalla versione **2.76.7-ic8tem.19**, solo **Windows** (64 e 32 bit): in *Options › Rig Control* c'è la radio **Icom IC-7300 (Hamlib)**; dalla **2.76.7-ic8tem.28** anche **Icom IC-7610 (Hamlib)** e **Icom IC-9700 (Hamlib)**. MSHV comanda la radio con la libreria **Hamlib**, la stessa usata da WSJT-X e da IP-TRX-CTRL, al posto dei comandi Icom scritti da LZ2HV.

Non è obbligatorio: anche con le voci normali di MSHV (*Icom IC-7300*, *Icom IC-7610*, *Icom IC-9700* via USB, oppure *Icom LAN*) la finestra **Potenza e ROS** regola la potenza e legge Po, ROS e ALC. Le voci Hamlib sono un'alternativa, per esempio se con i comandi di MSHV qualcosa non va.

## Impostazione

1. *Options › Rig Control*: scegliete **Icom IC-7300 (Hamlib)**, **Icom IC-7610 (Hamlib)** o **Icom IC-9700 (Hamlib)** nell'elenco RIG.
2. **Port**: la porta COM della radio, cioè quella che in Gestione dispositivi compare come "Silicon Labs CP210x USB to UART Bridge". Se la radio ne mostra due, provate prima quella con il numero più basso.
3. **Baud Rate**: la stessa velocità di *MENU › SET › Connectors › CI-V › CI-V USB Baud Rate* della radio (consigliato `115200`; con *Auto* va bene `115200`). Su IC-7610 e IC-9700 la voce del menu ha lo stesso nome (*Connectors › CI-V*).
4. **PTT Method**: **PTT Via CAT COMMAND**.
5. MSHV avvia Hamlib da solo: nel riquadro Network la riga diventa verde con la versione di Hamlib. Se scegliete un'altra porta o velocità, Hamlib si ricollega subito.

La radio deve avere l'indirizzo CI-V predefinito: IC-7300 `94h`, IC-7610 `98h`, IC-9700 `A2h`. Modelli Hamlib usati: 3073, 3078 e 3081. La porta COM può essere aperta da un solo programma: chiudete prima gli altri programmi che usano la stessa porta (IP-TRX-CTRL, rigctld, un altro CAT).

## Cosa fa

* Frequenza e modo letti e impostati (USB-D per i modi digitali), PTT via CAT. Frequenza e modo sono letti di continuo (ogni *Polling Interval* di Rig Control): se cambiate banda dalla radio, MSHV la segue (corretto nella versione **2.76.7-ic8tem.26**).
* Finestra **Potenza e ROS** (*Options › View Power and SWR*), con la connessione *CAT di MSHV*: regolazione della potenza (`RFPOWER`) e lettura di Po (`RFPOWER_METER`), ROS (`SWR`) e ALC (`ALC`).
* La scheda **CAT Log** della finestra Potenza e ROS mostra ogni comando Hamlib e la risposta (o l'errore di Hamlib).
* **IC-9700**: Po al 100 % vuol dire 100 W in 2 m, 75 W in 70 cm e 10 W in 23 cm. Con l'IC-9700 (voce Hamlib o voce normale) la finestra Potenza e ROS mette da sola la *Potenza massima della radio* della banda in uso, così i watt sono giusti su ogni banda.

## Da sapere

* *Access denied* nella riga di Rig Control vuol dire che la porta COM è già aperta da un altro programma (o non è quella della radio): chiudete l'altro programma o scegliete la porta giusta in *Port*.
* Se tornate dalla voce Hamlib alla voce normale (per esempio da *Icom IC-9700 (Hamlib)* a *Icom IC-9700*), dalla **2.76.7-ic8tem.35** MSHV riapre da solo la porta indicata in *Port* e la riga torna a *Disconnected*. Prima restava il messaggio di Hamlib e la porta andava scelta di nuovo (un'altra porta e poi quella giusta).
* La libreria Hamlib (`libhamlib-4.dll`, licenza LGPL) è nella cartella `hamlib` accanto a MSHV ed è caricata solo quando scegliete questa radio. Se non si carica, la riga di Rig Control dice perché e il resto di MSHV funziona normalmente.
* Fino alla versione **2.76.7-ic8tem.29**, su un PC senza Hamlib installato, la riga diceva *Hamlib: cannot load libhamlib-4.dll ... Impossibile trovare il modulo specificato*: Windows non trovava `libusb-1.0.dll`, che sta nella cartella `hamlib`. Corretto nella **2.76.7-ic8tem.30**. Con la .28 o la .29 basta copiare `hamlib\libusb-1.0.dll` nella cartella di `MSHV-IC8TEM.exe`.
* Su Windows ARM, Linux, macOS e BSD questa voce non c'è: lì usate *Hamlib NET rigctl* con `rigctld`, oppure la connessione *Hamlib rigctld* della finestra Potenza e ROS.
