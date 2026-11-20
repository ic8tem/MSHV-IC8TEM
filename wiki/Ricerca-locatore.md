# Ricerca del locatore

Dalla versione **2.76.7-ic8tem.28**. Quando mettete un nominativo in **TO RADIO** (a mano o con un clic su una decodifica), MSHV cerca il suo locatore e lo scrive in **LOCATOR**. Prima guardava solo il messaggio cliccato e il database di MSHV (`settings/database/msloc_db.dbmh`): se lì non c'era, il locatore restava vuoto e accanto compariva *DB:NA*.

## Ordine della ricerca

1. Il **messaggio cliccato**, se contiene il locatore (per esempio `CQ WA2LMC FM05`).
2. Il **database di MSHV**: etichetta `DB:`.
3. Le **decodifiche degli ultimi 20 periodi**: il locatore che la stazione stessa ha trasmesso (`CQ WA2LMC FM05`, `IC8TEM WA2LMC FM05`, `PA3XYZ G4ABC R 570007 IO91NP` nei contest VHF; mai `RR73`). Etichetta `RX:`. 20 periodi sono 5 minuti in FT8, 2,5 in FT4, 10 in MSK144 da 30 s, 20 in JT65. Contano solo le decodifiche in diretta, non quelle dei file aperti.
4. **CALL3.TXT di WSJT-X Improved** (lista terrestre su SourceForge): `C3:`.
5. **CALL3.TXT EME di W7GJ**: `EME:`.
6. **grid_data.bin di JTDX** (su SourceForge): `JTDX:`.
7. **In rete**, servizio di G7VRD (`api.g7vrd.co.uk`, ancora in prova): `WEB:`. Viene chiesto solo se nessun'altra fonte ha il locatore, 1,2 secondi dopo l'ultima modifica del nominativo; intanto l'etichetta mostra `WEB:...`.

Vince la prima fonte che conosce il nominativo. Se ha un locatore di 4 caratteri e una fonte successiva ha lo stesso quadrato con 6 caratteri, MSHV usa quello di 6: per esempio decodifica `FM05` e CALL3 `FM05XX` danno `C3:FM05XX`.

## L'etichetta accanto a LOCATOR

Dice da dove viene il locatore: `DB:KN23SF`, `RX:FM05`, `C3:FN42AB`, `EME:JO62OR`, `JTDX:ON48`, `WEB:EN34`, oppure `DB:NA` se nessuno lo conosce. Tenendo il mouse sopra l'etichetta si vede cosa dice ogni fonte per quel nominativo.

Il titolo **LOCATOR** resta arancione finché il locatore non è nel database di MSHV: **ADD** ve lo salva, come prima. Il locatore trovato va anche nel log del QSO; con Multi Answer, se il messaggio non lo aveva, si cercano il database e le fonti dalla 3 alla 6.

## Impostazioni

*Options › Network Configuration › Page 4*: ogni fonte si accende o si spegne, e accanto si vede quanti nominativi ha la lista e quando è stata scaricata. Con **Download the lists and update them every 14 days** (acceso di serie) MSHV scarica le liste da solo, 30 secondi dopo l'avvio se mancano o hanno più di 14 giorni, e poi controlla ogni 6 ore; **Update Now** le scarica subito. Una lista scaricata viene controllata prima di prendere il posto di quella vecchia: se il file non è valido (per esempio una pagina web al posto del file), resta quella vecchia e la pagina scrive l'errore.

| Fonte | File in `settings/database/locsources` | Da dove |
|---|---|---|
| CALL3.TXT terrestre (WSJT-X Improved) | `CALL3_WSJT-X_Improved.TXT` | <https://sourceforge.net/projects/wsjt-x-improved/files/Additional%20Files/> |
| CALL3.TXT EME (W7GJ) | `CALL3_EME_W7GJ.TXT` | <https://www.bigskyspaces.com/w7gj/> |
| grid_data.bin (JTDX) | `grid_data.bin` | <https://sourceforge.net/projects/jtdx/files/> |
| G7VRD, risposte già ricevute | `g7vrd_cache.txt` | <https://g7vrd.co.uk/public-apis> |

Il servizio in rete riceve solo il nominativo cercato. Le risposte restano 30 giorni, le risposte "non trovato" un giorno; se il servizio chiede di rallentare (HTTP 429) MSHV non chiede niente per 15 minuti, dopo un altro errore per 2 minuti. I nominativi con `/` non vengono chiesti in rete.

Le liste scaricate servono solo a questa ricerca: il database di MSHV e la lista della ricerca profonda JT65 restano quelli di prima. Impostazioni in `settings/ms_locsrc`.
