# Utenti LoTW

**Options › Network Configuration › Page 3**

## Scaricare la lista ARRL

Link al file: **<https://lotw.arrl.org/lotw-user-activity.csv>**

Due modi:

1. **Download from ARRL and Load** (dalla versione 2.76.7-ic8tem.5): MSHV scarica il file, lo controlla e lo carica. Serve la connessione a internet.
2. Cliccate il link nella pagina (o qui sopra), salvate il file e caricatelo con **Load LoTW CSV File**.

Il file viene copiato in `settings/database/lotw-user-activity.csv` e ricaricato a ogni avvio. Conviene aggiornarlo ogni tanto: ARRL lo rigenera di continuo.

## Impostazioni

* **Valid if last LoTW upload is on or after**: data minima dell'ultimo upload LoTW perché un nominativo conti come utente LoTW (predefinita: un anno fa).
* Spunta per mostrare o nascondere il **pallino nero**.
* Impostazioni salvate in `settings/ms_lotw`.

## Pallino nero

A destra della colonna del messaggio compare un pallino nero se la stazione che trasmette (es. K1ABC in `CQ K1ABC FN42` o in `IC8TEM K1ABC -10`) è nella lista con data valida. Vengono riconosciuti anche K1ABC/P, EA8/K1ABC ecc.

Vedi anche il filtro [[Show LoTW Users Only|Filtri]].
