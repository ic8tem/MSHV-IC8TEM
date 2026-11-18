# Rumore in MSK144: noise blanker e notch

Menu **Decode**, attivi solo in MSK144 e MSK144ms. Sono **spenti di serie**.

## MSK144 Noise Blanker

Toglie i **disturbi impulsivi** (scariche, alimentatori switching, recinti elettrici, accensioni) dall'audio che arriva dalla radio, prima della decodifica.

Senza blanker, un impulso forte il rilevatore MSK144 lo prende per un ping: per ogni periodo prova al massimo 16 candidati, dai più forti, e gli impulsi possono rubare il posto ai ping deboli veri. In più rovinano i bit dei ping che colpiscono.

* **MSK144 Noise Blanker Off**: come MSHV originale.
* **MSK144 Noise Blanker (NR0V NB2)** — *consigliato*. Sul segnale analitico: trova gli impulsi più forti di 5 volte il livello mediano intorno, li azzera con un margine di 0,5 ms e raccordi morbidi. Metodo del noise blanker NB2 di WDSP (Warren Pratt NR0V, usato da Thetis e piHPSDR), cambiato per il meteor scatter: un evento il cui nucleo dura più di 3 ms (un ping, una scarica lunga) **non viene toccato**, e nemmeno l'inizio e la fine di un ping.
* **MSK144 Noise Blanker LPC (DL2FW)**: il blanker di Michael Wild DL2FW (firmware UHSDR del mcHF). Con la predizione lineare trova gli impulsi e **ricostruisce** i campioni colpiti dal segnale prima e dopo. Serve per confronto.

Nelle prove con segnali simulati (rumore, ping da +3 a +25 dB, segnale MSK144 continuo, impulsi forti e deboli) NB2 ha tolto l'87% degli impulsi senza toccare mai il resto; LPC il 68%, con rarissime correzioni sbagliate nei ping molto forti.

## MSK144 Carrier Notch (ZAP)

Toglie le **portanti fisse** (spurie del PC, dell'SDR, del transverter, alimentatori) con una tacca di 10 Hz.

Come lo ZAP del vecchio WSJT: una spuria è una riga stretta **stabile nel tempo**. MSHV guarda la mediana nel tempo di spettri da 0,17 s (i ping non contano) e toglie al massimo 8 righe ben sopra lo spettro vicino. In decodifica in tempo reale usa gli ultimi 5,5 secondi.

Fanno più danno le portanti vicino a **1000 e 2000 Hz**, dove il rilevatore MSK144 cerca i toni: alzano il rumore stimato per tutto il periodo.

Il notch **non interviene** quando c'è un segnale MSK presente quasi sempre (tropo, Es): anche il suo spettro è fatto di righe fisse, a 13,9 Hz (o 50 Hz per i messaggi brevi) una dall'altra.

## Come provarli

Apri un WAV salvato (RxWavs) con il blanker spento, poi riaprilo con il blanker acceso e confronta le decodifiche. Il noise blanker della radio (IC-7300, IC-7610, K3...) lavora prima dei filtri ed è di solito più efficace: questi servono quando quello della radio non basta, crea intermodulazione o con l'audio dell'SDR.
