# IC-7300 con Hamlib

Dalla versione **2.76.7-ic8tem.17**, solo **Windows** (64 e 32 bit): in *Options › Rig Control* c'è la radio **Icom IC-7300 (Hamlib)**. MSHV comanda l'IC-7300 con la libreria **Hamlib**, la stessa usata da WSJT-X e da IP-TRX-CTRL, al posto dei comandi Icom scritti da LZ2HV.

## Impostazione

1. *Options › Rig Control*: scegliete **Icom IC-7300 (Hamlib)** (è nell'elenco delle radio di rete).
2. Nel riquadro **Network**:
   * **Server** = porta seriale della radio, per esempio `COM5` (la stessa che vedete in Gestione dispositivi, "Silicon Labs CP210x").
   * **Port** = velocità, per esempio `115200` (deve essere uguale a *CI-V USB Baud Rate* della radio).
3. Premete **Connect**: la riga diventa verde con la versione di Hamlib.

La porta COM può essere aperta da un solo programma: chiudete prima gli altri programmi che usano la stessa porta (IP-TRX-CTRL, rigctld, un altro CAT).

## Cosa fa

* Frequenza e modo letti e impostati (USB-D per i modi digitali), PTT via CAT.
* Finestra **Potenza e ROS** (*Options › View Power and SWR*), con la connessione *CAT di MSHV*: regolazione della potenza (`RFPOWER`) e lettura di Po (`RFPOWER_METER`), ROS (`SWR`) e ALC (`ALC`).
* La scheda **CAT Log** della finestra Potenza e ROS mostra ogni comando Hamlib e la risposta (o l'errore di Hamlib).

## Da sapere

* La libreria Hamlib (`libhamlib-4.dll`, licenza LGPL) è nella cartella `hamlib` accanto a MSHV ed è caricata solo quando scegliete questa radio. Se non si carica, la riga di Rig Control dice perché e il resto di MSHV funziona normalmente.
* Su Windows ARM, Linux, macOS e BSD questa voce non c'è: lì usate *Hamlib NET rigctl* con `rigctld`, oppure la connessione *Hamlib rigctld* della finestra Potenza e ROS.
