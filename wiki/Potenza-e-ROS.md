# Potenza e ROS

Dalla versione **2.76.7-ic8tem.10**: **Options › View Power and SWR** apre una finestra a parte per comandare la potenza di uscita e leggere Po, ROS e ALC.

Si collega alla radio in tre modi (scheda *Impostazioni › Connessione*, dalla versione **2.76.7-ic8tem.15**):

* **CAT di MSHV**, con il CAT di MSHV attivo:
  * **Icom** con il cavo USB (CI-V), **Icom LAN** o le voci Hamlib **Icom IC-7300 / IC-7610 / IC-9700 (Hamlib)**;
  * **Yaesu** FT-991/FT-991A (stesso protocollo: voce "FT-991"), FT-DX10, FT-710, FT-DX101, FT-891, FT-DX1200, FT-DX3000, FT-DX5000, FT-950, FT-2000, FTX-1, FT-450, FT-450D: potenza `PCnnn;`, Po `RM5;`, ROS `RM6;`, ALC `RM4;`. **FTDX-9000**: Po `RM08;` (in watt), ROS `RM09;`, ALC `RM07;`, potenza `PCnnn;` fino a 200 W;
  * **Kenwood** TS-2000, TS-590S, TS-590SG, TS-480: potenza `PCnnn;`, ROS e ALC `RM;`, Po `SM0;` durante la trasmissione. Con il TS-2000 `RM;` restituisce solo lo strumento scelto sulla radio (SWR o ALC);
  * **Kenwood TS-890S**: potenza `PCnnn;`, ALC e ROS `RM11;RM21;RM;`, Po in watt `SM;` durante la trasmissione. **TS-990S**: potenza (fino a 200 W), ALC e ROS come il TS-890S; la Po non si legge via CAT;
  * **Kenwood TS-870S, TS-850, TS-690S, TS-450S**: ROS o ALC con `RM;`, cioè lo strumento scelto sulla radio (ROS oppure ALC); potenza `PCnnn;` solo con il TS-870S (il TS-450S la legge soltanto, TS-850 e TS-690S non la regolano via CAT); la Po non si legge via CAT;
  * **Da provare sulla radio**: BG2FX FX-4CR (comandi del TS-590), Hilberling PT-8000A (del TS-480), SDR Console (del TS-2000);
  * **Elecraft K3, K3S, KX3, KX2** (voce "K3/KX3"): potenza `PCnnn;`, Po o ALC `BG;` secondo lo strumento scelto sulla radio (`TM`), ROS `SW;`.
  * **Elecraft K4, K4D, K4HD** (voce "K4"): potenza `PCnnnH;`; con un solo comando `TM;` arrivano ALC, compressione, potenza diretta (W) e ROS;
  * **TCI** (TCI Client RX1/RX2): la potenza è il `drive` (0-100); Po (`tx_power`, in W) e ROS (`tx_swr`) li manda la radio durante la trasmissione;
  * **FlexRadio 6xxx**: potenza `transmit set rfpower=`, Po e ROS dagli strumenti FWDPWR e SWR della radio;
  * **PowerSDR e Thetis** (voci *FlexRadio/Apache PowerSDR* e *Thetis*): potenza `ZZPCnnn;`, Po in watt `ZZRM5;` e ROS `ZZRM8;` durante la trasmissione.
  Per Po in watt la barra usa la *Potenza massima* delle Impostazioni. Po e (con PowerSDR/Thetis) ROS vengono chiesti solo in trasmissione, anche con il PTT su RTS/DTR.

  Fino alla versione **2.76.7-ic8tem.28** le radio Yaesu, Kenwood ed Elecraft di questo elenco non erano riconosciute (la finestra diceva "non disponibile"): corretto nella **2.76.7-ic8tem.29**.

  Le scale degli strumenti sono quelle di Hamlib; la potenza in percentuale corrisponde ai watt per le radio da 100 W.
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
* **IC-9700**: Po al 100 % è la potenza massima della banda (100 W in 2 m, 75 W in 70 cm, 10 W in 23 cm). Con il CAT di MSHV e l'IC-9700 la *Potenza massima della radio* si imposta da sola a ogni cambio di banda (dalla versione **2.76.7-ic8tem.28**).
* Comandi CI-V usati: `14 0A` potenza, `15 11` Po, `15 12` ROS, `15 13` ALC.
* La finestra prende la misura della scheda aperta (dalla versione **2.76.7-ic8tem.25**): la scheda *Potenza e ROS* è piccola, *Impostazioni* e *CAT Log* la ingrandiscono quando le aprite e, tornando alla prima scheda, si rimpicciolisce.
* Impostazioni salvate in `settings/ms_pwrswr`.
* La finestra è tradotta nelle stesse lingue di MSHV.
