# Potenza e ROS

Dalla versione **2.76.7-ic8tem.10**: **Options › View Power and SWR** apre una finestra a parte per comandare la potenza di uscita e leggere Po, ROS e ALC.

Funziona con le radio **Icom**, sia con il cavo USB (CI-V) sia con **Icom LAN**, con il CAT attivo. Con altre radio la finestra lo dice e i comandi restano disattivati.

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

In alto nella prima scheda si vede la radio scelta in Rig Control, il tipo di collegamento (Icom CI-V, Icom LAN, OmniRig, rete...) e se il CAT è attivo: la finestra funziona solo con **Icom CI-V** o **Icom LAN** e CAT attivo.

## Da sapere

* **Con il transverter** (es. 144 → 28 MHz) la radio misura la potenza di pilotaggio e il ROS verso il transverter, **non** in antenna. Per il ROS dell'antenna serve un misuratore esterno.
* Le scale di Po, ROS e ALC sono quelle dei manuali CI-V Icom (IC-7300: ROS 1,5 = 48, 2,0 = 80, 3,0 = 120). Su altri modelli possono differire leggermente.
* Comandi CI-V usati: `14 0A` potenza, `15 11` Po, `15 12` ROS, `15 13` ALC.
* Impostazioni salvate in `settings/ms_pwrswr`.
* La finestra è tradotta nelle stesse lingue di MSHV.
