# Potenza e ROS

Dalla versione **2.76.7-ic8tem.10**: **Options › View Power and SWR** apre una finestra a parte per comandare la potenza di uscita e leggere Po, ROS e ALC.

Si collega alla radio in tre modi (scheda *Impostazioni › Connessione*, dalla versione **2.76.7-ic8tem.15**):

* **CAT di MSHV**: radio Icom con il cavo USB (CI-V) o **Icom LAN**, con il CAT di MSHV attivo.
* **Hamlib rigctld (TCP)**: qualsiasi radio supportata da Hamlib, **anche senza il CAT di MSHV**. Si indica indirizzo e porta di rigctld (di solito `127.0.0.1` e `4532`). Comandi usati: `l RFPOWER`, `L RFPOWER`, `l RFPOWER_METER`, `l SWR`, `l ALC`, `f`. rigctld può servire più programmi insieme: per esempio `rigctld -m 3073 -r COM5 -s 115200` per l'IC-7300 (IC-7610: `-m 3078`), poi MSHV (Rig Control › *Hamlib NET rigctl*) e questa finestra si collegano entrambi a rigctld.
* **Icom CI-V su porta seriale propria**: la finestra apre da sola la porta COM della radio (velocità e indirizzo CI-V: IC-7300 `94`, IC-7300MK2 `B6`, IC-7610 `98`, IC-705 `A4`, IC-9700 `A2`), **anche senza il CAT di MSHV**. Una porta seriale può essere aperta da un solo programma: se il CAT di MSHV o un altro programma usa già quella porta, la finestra lo segnala.

Se nessuna connessione funziona, la finestra lo dice e i comandi restano disattivati.

## Scheda "Potenza e ROS"

* **Potenza di uscita**: cursore in %, pulsanti − / +, scelte rapide 10–25–35–50–100 %. Accanto, i watt calcolati dalla potenza massima della radio.
* **Memoria banda**: *Salva per questa banda* ricorda la potenza della banda in uso (anche per modo, se attivato nelle impostazioni).
* **Tune (x W)**: abbassa la potenza al valore di Tune della banda, avvia il Tune di MSHV e, finita la trasmissione, rimette la potenza di prima.
* **Leggi dalla radio**: porta il cursore al valore impostato sulla radio.
* **Misure in trasmissione**: Po in watt con il picco, ROS su fasce verde/giallo/rosso con il segno della soglia, ALC.
* **Protezione**: se il ROS supera la soglia (predefinita 2,5) per più del tempo scelto (1 s), MSHV ferma la trasmissione e la blocca finché non premete **Sblocca**.

## Scheda "Impostazioni"

* Potenza massima della radio (W), ogni quanto leggere gli strumenti (predefinito 200 ms), lettura solo in trasmissione.
* Tabella **potenza per banda** (160 m – 23 cm) con la potenza di Tune.
* Potenza memorizzata applicata al cambio di banda; memoria separata per modo (es. 2 m MSK144 e 2 m FT8).
* Avvisi: ALC sopra il 50 % (audio troppo alto nei modi digitali), Po sotto metà della potenza impostata.

## Scheda "CAT Log"

Mostra ogni comando CI-V di potenza e strumenti inviato alla radio (`>>`) e ogni risposta (`<<`), in esadecimale e tradotta (es. `FE FE E0 98 15 12 00 30 FD   SWR 30 = 1.3`). **Leggi ora** chiede subito potenza e strumenti, anche senza trasmettere. Una risposta `FA` significa che la radio ha rifiutato il comando. *Copia* mette tutto negli appunti, per inviarlo a chi deve controllare.

Con rigctld il registro mostra i comandi di testo e le risposte di Hamlib.

In alto nella prima scheda si vede lo stato della connessione: con il CAT di MSHV la radio scelta in Rig Control, il tipo di collegamento e se il CAT è attivo; con la connessione propria rigctld o la porta CI-V e se risponde.

## Da sapere

* **Con il transverter** (es. 144 → 28 MHz) la radio misura la potenza di pilotaggio e il ROS verso il transverter, **non** in antenna. Per il ROS dell'antenna serve un misuratore esterno.
* Le scale di Po, ROS e ALC sono quelle dei manuali CI-V Icom (IC-7300: ROS 1,5 = 48, 2,0 = 80, 3,0 = 120). Su altri modelli possono differire leggermente.
* Comandi CI-V usati: `14 0A` potenza, `15 11` Po, `15 12` ROS, `15 13` ALC.
* Impostazioni salvate in `settings/ms_pwrswr`.
* La finestra è tradotta nelle stesse lingue di MSHV.
