# -*- coding: utf-8 -*-
# MSHV-IC8TEM: translations of the strings added by IC8TEM, in the same
# languages as MSHV. Used by make_qm.py to build src/HvTranslations/ic8tem_*.qm
# (loaded by MSHV next to the original mshv_*.qm).
#
# Each entry: (context, English source, {language file id: translation}).
# Language ids = the suffixes of src/HvTranslations/mshv_<id>.qm

LANGS = ["bg", "ru", "zh", "zhhk", "eses", "caes", "ptpt", "roro", "dadk",
         "plpl", "frfr", "ptbr", "nbno", "itit", "cscz"]

P = "HvPowerSwrW"
M = "Main_Ms"

def T(bg, ru, zh, zhhk, eses, caes, ptpt, roro, dadk, plpl, frfr, ptbr, nbno, itit, cscz):
    return dict(zip(LANGS, [bg, ru, zh, zhhk, eses, caes, ptpt, roro, dadk, plpl, frfr, ptbr, nbno, itit, cscz]))

MESSAGES = [
(P, "Power and SWR", T(
    "Мощност и КСВ", "Мощность и КСВ", "功率和驻波比", "功率及駐波比", "Potencia y ROE", "Potència i ROE",
    "Potência e ROE", "Putere și SWR", "Effekt og SWR", "Moc i WFS", "Puissance et ROS", "Potência e ROE",
    "Effekt og SWR", "Potenza e ROS", "Výkon a PSV")),
(M, "View Power and SWR", T(
    "Покажи мощност и КСВ", "Показать мощность и КСВ", "查看功率和驻波比", "查看功率及駐波比", "Ver potencia y ROE",
    "Mostra potència i ROE", "Ver potência e ROE", "Arată putere și SWR", "Vis effekt og SWR", "Pokaż moc i WFS",
    "Afficher puissance et ROS", "Ver potência e ROE", "Vis effekt og SWR", "Mostra Potenza e ROS", "Zobrazit výkon a PSV")),
(P, "Settings", T(
    "Настройки", "Настройки", "设置", "設定", "Ajustes", "Configuració", "Definições", "Setări", "Indstillinger",
    "Ustawienia", "Paramètres", "Configurações", "Innstillinger", "Impostazioni", "Nastavení")),
(P, "Output Power", T(
    "Изходна мощност", "Выходная мощность", "输出功率", "輸出功率", "Potencia de salida", "Potència de sortida",
    "Potência de saída", "Putere de ieșire", "Udgangseffekt", "Moc wyjściowa", "Puissance de sortie", "Potência de saída",
    "Utgangseffekt", "Potenza di uscita", "Výstupní výkon")),
(P, "Measurements while transmitting", T(
    "Измервания при предаване", "Измерения при передаче", "发射时的测量", "發射時的測量", "Medidas en transmisión",
    "Mesures en transmissió", "Medidas em transmissão", "Măsurători la emisie", "Målinger under sending",
    "Pomiary podczas nadawania", "Mesures en émission", "Medidas em transmissão", "Målinger under sending",
    "Misure in trasmissione", "Měření při vysílání")),
(P, "Decrease power", T(
    "Намали мощността", "Уменьшить мощность", "降低功率", "降低功率", "Bajar potencia", "Baixa la potència",
    "Diminuir potência", "Scade puterea", "Sænk effekten", "Zmniejsz moc", "Diminuer la puissance", "Diminuir potência",
    "Senk effekten", "Diminuisci potenza", "Snížit výkon")),
(P, "Increase power", T(
    "Увеличи мощността", "Увеличить мощность", "提高功率", "提高功率", "Subir potencia", "Puja la potència",
    "Aumentar potência", "Crește puterea", "Øg effekten", "Zwiększ moc", "Augmenter la puissance", "Aumentar potência",
    "Øk effekten", "Aumenta potenza", "Zvýšit výkon")),
(P, "Save for this band", T(
    "Запази за тази лента", "Сохранить для этого диапазона", "为此波段保存", "為此波段儲存", "Guardar para esta banda",
    "Desa per a aquesta banda", "Guardar para esta banda", "Salvează pentru această bandă", "Gem for dette bånd",
    "Zapisz dla tego pasma", "Enregistrer pour cette bande", "Salvar para esta banda", "Lagre for dette båndet",
    "Salva per questa banda", "Uložit pro toto pásmo")),
(P, "Band memory", T(
    "Памет за лентата", "Память диапазона", "波段记忆", "波段記憶", "Memoria de banda", "Memòria de banda",
    "Memória de banda", "Memorie bandă", "Båndhukommelse", "Pamięć pasma", "Mémoire de bande", "Memória de banda",
    "Båndminne", "Memoria banda", "Paměť pásma")),
(P, "Band", T(
    "Лента", "Диапазон", "波段", "波段", "Banda", "Banda", "Banda", "Bandă", "Bånd", "Pasmo", "Bande", "Banda",
    "Bånd", "Banda", "Pásmo")),
(P, "of", T(
    "от", "из", "/", "/", "de", "de", "de", "din", "af", "z", "sur", "de", "av", "di", "z")),
(P, "Tune", T(
    "Настройка", "Настройка", "调谐", "調諧", "Sintonía", "Sintonia", "Sintonia", "Acord", "Tune", "Strojenie",
    "Accord", "Sintonia", "Tune", "Tune", "Ladění")),
(P, "Stop Tune", T(
    "Спри настройката", "Остановить настройку", "停止调谐", "停止調諧", "Parar sintonía", "Atura la sintonia",
    "Parar sintonia", "Oprește acordul", "Stop tune", "Zatrzymaj strojenie", "Arrêter l'accord", "Parar sintonia",
    "Stopp tune", "Ferma Tune", "Zastavit ladění")),
(P, "Read from radio", T(
    "Прочети от радиото", "Прочитать с трансивера", "从电台读取", "從電台讀取", "Leer de la radio", "Llegeix de la ràdio",
    "Ler do rádio", "Citește din stație", "Læs fra radioen", "Odczytaj z radia", "Lire depuis la radio", "Ler do rádio",
    "Les fra radioen", "Leggi dalla radio", "Načíst z rádia")),
(P, "SWR", T(
    "КСВ", "КСВ", "驻波比", "駐波比", "ROE", "ROE", "ROE", "SWR", "SWR", "WFS", "ROS", "ROE", "SWR", "ROS", "PSV")),
(P, "Unlock", T(
    "Отключи", "Разблокировать", "解锁", "解鎖", "Desbloquear", "Desbloqueja", "Desbloquear", "Deblochează",
    "Lås op", "Odblokuj", "Débloquer", "Desbloquear", "Lås opp", "Sblocca", "Odblokovat")),
(P, "TX unlocked", T(
    "TX отключено", "TX разблокирован", "发射已解锁", "發射已解鎖", "TX desbloqueado", "TX desblocat", "TX desbloqueado",
    "TX deblocat", "TX låst op", "TX odblokowany", "TX débloqué", "TX desbloqueado", "TX låst opp", "TX sbloccato",
    "TX odblokováno")),
(P, "TX STOPPED", T(
    "TX СПРЯНО", "TX ОСТАНОВЛЕН", "发射已停止", "發射已停止", "TX DETENIDO", "TX ATURAT", "TX PARADO", "TX OPRIT",
    "TX STOPPET", "TX ZATRZYMANY", "TX ARRÊTÉ", "TX PARADO", "TX STOPPET", "TX FERMATO", "TX ZASTAVENO")),
(P, "check antenna and cables", T(
    "проверете антената и кабелите", "проверьте антенну и кабели", "请检查天线和电缆", "請檢查天線及電纜",
    "revise antena y cables", "reviseu l'antena i els cables", "verifique a antena e os cabos", "verificați antena și cablurile",
    "kontrollér antenne og kabler", "sprawdź antenę i kable", "vérifiez l'antenne et les câbles", "verifique a antena e os cabos",
    "kontroller antenne og kabler", "controlla antenna e cavi", "zkontrolujte anténu a kabely")),
(P, "TX stays blocked until you press Unlock.", T(
    "TX остава блокирано, докато не натиснете Отключи.", "TX остаётся заблокированным, пока не нажмёте «Разблокировать».",
    "在按下“解锁”之前发射保持锁定。", "按下「解鎖」之前發射保持鎖定。", "El TX queda bloqueado hasta que pulse Desbloquear.",
    "El TX queda bloquejat fins que premeu Desbloqueja.", "O TX fica bloqueado até premir Desbloquear.",
    "TX rămâne blocat până apăsați Deblochează.", "TX forbliver låst, indtil du trykker Lås op.",
    "TX pozostaje zablokowany, dopóki nie naciśniesz Odblokuj.", "Le TX reste bloqué jusqu'à ce que vous appuyiez sur Débloquer.",
    "O TX fica bloqueado até você clicar em Desbloquear.", "TX forblir låst til du trykker Lås opp.",
    "Il TX resta bloccato finché non premi Sblocca.", "TX zůstane zablokováno, dokud nestisknete Odblokovat.")),
(P, "TX blocked by the SWR protection: press Unlock", T(
    "TX е блокирано от защитата по КСВ: натиснете Отключи", "TX заблокирован защитой по КСВ: нажмите «Разблокировать»",
    "发射被驻波比保护锁定：请按“解锁”", "發射被駐波比保護鎖定：請按「解鎖」", "TX bloqueado por la protección de ROE: pulse Desbloquear",
    "TX bloquejat per la protecció de ROE: premeu Desbloqueja", "TX bloqueado pela proteção de ROE: prima Desbloquear",
    "TX blocat de protecția SWR: apăsați Deblochează", "TX låst af SWR-beskyttelsen: tryk Lås op",
    "TX zablokowany przez ochronę WFS: naciśnij Odblokuj", "TX bloqué par la protection ROS : appuyez sur Débloquer",
    "TX bloqueado pela proteção de ROE: clique em Desbloquear", "TX låst av SWR-beskyttelsen: trykk Lås opp",
    "TX bloccato dalla protezione ROS: premi Sblocca", "TX zablokováno ochranou PSV: stiskněte Odblokovat")),
(P, "Stop TX if SWR is above", T(
    "Спри TX, ако КСВ е над", "Остановить TX, если КСВ выше", "驻波比高于以下值时停止发射", "駐波比高於以下值時停止發射",
    "Detener TX si la ROE supera", "Atura el TX si la ROE supera", "Parar TX se a ROE for superior a",
    "Oprește TX dacă SWR depășește", "Stop TX hvis SWR er over", "Zatrzymaj TX, gdy WFS przekracza",
    "Arrêter le TX si le ROS dépasse", "Parar TX se a ROE passar de", "Stopp TX hvis SWR er over",
    "Ferma il TX se il ROS supera", "Zastavit TX, pokud PSV překročí")),
(P, "for more than", T(
    "за повече от", "дольше чем", "持续超过", "持續超過", "durante más de", "durant més de", "durante mais de",
    "mai mult de", "i mere end", "przez ponad", "pendant plus de", "por mais de", "i mer enn", "per più di", "déle než")),
(P, "Not available: choose an Icom radio (CI-V on USB or Icom LAN) in Rig Control and start the CAT.", T(
    "Не е налично: изберете радио Icom (CI-V по USB или Icom LAN) в Rig Control и стартирайте CAT.",
    "Недоступно: выберите трансивер Icom (CI-V по USB или Icom LAN) в Rig Control и запустите CAT.",
    "不可用：请在 Rig Control 中选择 Icom 电台（USB 上的 CI-V 或 Icom LAN）并启动 CAT。",
    "不可用：請在 Rig Control 中選擇 Icom 電台（USB 上的 CI-V 或 Icom LAN）並啟動 CAT。",
    "No disponible: elija una radio Icom (CI-V por USB o Icom LAN) en Rig Control e inicie el CAT.",
    "No disponible: trieu una ràdio Icom (CI-V per USB o Icom LAN) a Rig Control i engegueu el CAT.",
    "Não disponível: escolha um rádio Icom (CI-V por USB ou Icom LAN) em Rig Control e inicie o CAT.",
    "Indisponibil: alegeți o stație Icom (CI-V pe USB sau Icom LAN) în Rig Control și porniți CAT.",
    "Ikke tilgængelig: vælg en Icom-radio (CI-V via USB eller Icom LAN) i Rig Control og start CAT.",
    "Niedostępne: wybierz radio Icom (CI-V przez USB lub Icom LAN) w Rig Control i uruchom CAT.",
    "Non disponible : choisissez une radio Icom (CI-V sur USB ou Icom LAN) dans Rig Control et démarrez le CAT.",
    "Não disponível: escolha um rádio Icom (CI-V por USB ou Icom LAN) em Rig Control e inicie o CAT.",
    "Ikke tilgjengelig: velg en Icom-radio (CI-V via USB eller Icom LAN) i Rig Control og start CAT.",
    "Non disponibile: scegli una radio Icom (CI-V su USB o Icom LAN) in Rig Control e avvia il CAT.",
    "Nedostupné: zvolte rádio Icom (CI-V přes USB nebo Icom LAN) v Rig Control a spusťte CAT.")),
(P, "Maximum power of the radio", T(
    "Максимална мощност на радиото", "Максимальная мощность трансивера", "电台最大功率", "電台最大功率",
    "Potencia máxima de la radio", "Potència màxima de la ràdio", "Potência máxima do rádio", "Puterea maximă a stației",
    "Radioens maksimale effekt", "Maksymalna moc radia", "Puissance maximale de la radio", "Potência máxima do rádio",
    "Radioens maksimale effekt", "Potenza massima della radio", "Maximální výkon rádia")),
(P, "Read the meters every", T(
    "Чети измервателите на всеки", "Читать измерители каждые", "读取仪表间隔", "讀取儀表間隔", "Leer los medidores cada",
    "Llegeix els mesuradors cada", "Ler os medidores a cada", "Citește instrumentele la fiecare", "Læs målerne hver",
    "Odczytuj mierniki co", "Lire les mesures toutes les", "Ler os medidores a cada", "Les målerne hver",
    "Leggi gli strumenti ogni", "Číst měřidla každých")),
(P, "Read only while transmitting", T(
    "Чети само при предаване", "Читать только при передаче", "仅在发射时读取", "僅在發射時讀取", "Leer solo al transmitir",
    "Llegeix només en transmetre", "Ler apenas durante a transmissão", "Citește doar la emisie", "Læs kun under sending",
    "Odczytuj tylko podczas nadawania", "Lire seulement en émission", "Ler apenas durante a transmissão",
    "Les bare under sending", "Leggi solo durante la trasmissione", "Číst jen při vysílání")),
(P, "Power per band", T(
    "Мощност за лента", "Мощность по диапазонам", "各波段功率", "各波段功率", "Potencia por banda", "Potència per banda",
    "Potência por banda", "Putere pe bandă", "Effekt pr. bånd", "Moc dla pasma", "Puissance par bande", "Potência por banda",
    "Effekt per bånd", "Potenza per banda", "Výkon pro pásmo")),
(P, "Power %", T(
    "Мощност %", "Мощность %", "功率 %", "功率 %", "Potencia %", "Potència %", "Potência %", "Putere %", "Effekt %",
    "Moc %", "Puissance %", "Potência %", "Effekt %", "Potenza %", "Výkon %")),
(P, "Tune %", T(
    "Настройка %", "Настройка %", "调谐 %", "調諧 %", "Sintonía %", "Sintonia %", "Sintonia %", "Acord %", "Tune %",
    "Strojenie %", "Accord %", "Sintonia %", "Tune %", "Tune %", "Ladění %")),
(P, "Set the stored power when the band changes", T(
    "Задай запазената мощност при смяна на лентата", "Устанавливать сохранённую мощность при смене диапазона",
    "切换波段时设置保存的功率", "切換波段時設定儲存的功率", "Aplicar la potencia guardada al cambiar de banda",
    "Aplica la potència desada en canviar de banda", "Aplicar a potência guardada ao mudar de banda",
    "Setează puterea memorată la schimbarea benzii", "Indstil den gemte effekt ved båndskift",
    "Ustaw zapisaną moc przy zmianie pasma", "Appliquer la puissance mémorisée au changement de bande",
    "Aplicar a potência salva ao trocar de banda", "Sett lagret effekt ved båndbytte",
    "Imposta la potenza memorizzata al cambio di banda", "Nastavit uložený výkon při změně pásma")),
(P, "Separate memory for each mode (e.g. 2 m MSK144 and 2 m FT8)", T(
    "Отделна памет за всеки вид (напр. 2 m MSK144 и 2 m FT8)", "Отдельная память для каждого вида (напр. 2 m MSK144 и 2 m FT8)",
    "每种模式单独记忆（例如 2 m MSK144 和 2 m FT8）", "每種模式單獨記憶（例如 2 m MSK144 及 2 m FT8）",
    "Memoria separada para cada modo (p. ej. 2 m MSK144 y 2 m FT8)", "Memòria separada per a cada mode (p. ex. 2 m MSK144 i 2 m FT8)",
    "Memória separada para cada modo (ex. 2 m MSK144 e 2 m FT8)", "Memorie separată pentru fiecare mod (ex. 2 m MSK144 și 2 m FT8)",
    "Separat hukommelse for hver mode (f.eks. 2 m MSK144 og 2 m FT8)", "Osobna pamięć dla każdego trybu (np. 2 m MSK144 i 2 m FT8)",
    "Mémoire séparée pour chaque mode (ex. 2 m MSK144 et 2 m FT8)", "Memória separada para cada modo (ex. 2 m MSK144 e 2 m FT8)",
    "Eget minne for hver mode (f.eks. 2 m MSK144 og 2 m FT8)", "Memoria separata per ogni modo (es. 2 m MSK144 e 2 m FT8)",
    "Samostatná paměť pro každý druh provozu (např. 2 m MSK144 a 2 m FT8)")),
(P, "Protection", T(
    "Защита", "Защита", "保护", "保護", "Protección", "Protecció", "Proteção", "Protecție", "Beskyttelse", "Ochrona",
    "Protection", "Proteção", "Beskyttelse", "Protezione", "Ochrana")),
(P, "Warn if ALC is above 50 % (audio too high in digital modes)", T(
    "Предупреди, ако ALC е над 50 % (твърде силен звук при цифровите видове)",
    "Предупреждать, если ALC выше 50 % (слишком высокий уровень звука в цифровых видах)",
    "ALC 高于 50 % 时警告（数字模式音频过高）", "ALC 高於 50 % 時警告（數碼模式音頻過高）",
    "Avisar si el ALC supera el 50 % (audio demasiado alto en modos digitales)",
    "Avisa si l'ALC supera el 50 % (àudio massa alt en modes digitals)",
    "Avisar se o ALC passar de 50 % (áudio demasiado alto nos modos digitais)",
    "Avertizează dacă ALC depășește 50 % (audio prea mare în modurile digitale)",
    "Advar hvis ALC er over 50 % (for højt lydniveau i digitale modes)",
    "Ostrzegaj, gdy ALC przekracza 50 % (za głośne audio w emisjach cyfrowych)",
    "Avertir si l'ALC dépasse 50 % (audio trop fort en modes numériques)",
    "Avisar se o ALC passar de 50 % (áudio alto demais nos modos digitais)",
    "Varsle hvis ALC er over 50 % (for høyt lydnivå i digitale moder)",
    "Avvisa se l'ALC supera il 50 % (audio troppo alto nei modi digitali)",
    "Varovat, pokud ALC překročí 50 % (příliš silný zvuk v digitálních druzích provozu)")),
(P, "Warn if Po stays below 50 % of the set power", T(
    "Предупреди, ако Po остава под 50 % от зададената мощност", "Предупреждать, если Po ниже 50 % заданной мощности",
    "Po 低于设定功率 50 % 时警告", "Po 低於設定功率 50 % 時警告", "Avisar si la Po queda por debajo del 50 % de la potencia ajustada",
    "Avisa si la Po queda per sota del 50 % de la potència fixada", "Avisar se a Po ficar abaixo de 50 % da potência definida",
    "Avertizează dacă Po rămâne sub 50 % din puterea setată", "Advar hvis Po forbliver under 50 % af den indstillede effekt",
    "Ostrzegaj, gdy Po pozostaje poniżej 50 % ustawionej mocy", "Avertir si la Po reste sous 50 % de la puissance réglée",
    "Avisar se a Po ficar abaixo de 50 % da potência definida", "Varsle hvis Po holder seg under 50 % av innstilt effekt",
    "Avvisa se la Po resta sotto il 50 % della potenza impostata", "Varovat, pokud Po zůstane pod 50 % nastaveného výkonu")),
(P, "With a transverter the radio measures the drive power and the SWR towards the transverter, not those at the antenna.", T(
    "С трансвертер радиото измерва мощността на възбуждане и КСВ към трансвертера, а не тези в антената.",
    "С трансвертером трансивер измеряет мощность раскачки и КСВ на входе трансвертера, а не в антенне.",
    "使用变频器时，电台测量的是驱动功率和到变频器的驻波比，而不是天线处的数值。",
    "使用變頻器時，電台測量的是推動功率及到變頻器的駐波比，而不是天線處的數值。",
    "Con un transverter la radio mide la potencia de excitación y la ROE hacia el transverter, no las de la antena.",
    "Amb un transverter la ràdio mesura la potència d'excitació i la ROE cap al transverter, no les de l'antena.",
    "Com um transverter o rádio mede a potência de excitação e a ROE para o transverter, não as da antena.",
    "Cu un transverter stația măsoară puterea de excitație și SWR spre transverter, nu pe cele de la antenă.",
    "Med en transverter måler radioen styreeffekten og SWR mod transverteren, ikke ved antennen.",
    "Z transwerterem radio mierzy moc sterującą i WFS w kierunku transwertera, a nie na antenie.",
    "Avec un transverter la radio mesure la puissance d'attaque et le ROS vers le transverter, pas ceux de l'antenne.",
    "Com um transverter o rádio mede a potência de excitação e a ROE para o transverter, não as da antena.",
    "Med en transverter måler radioen styreeffekten og SWR mot transverteren, ikke ved antennen.",
    "Con un transverter la radio misura la potenza di pilotaggio e il ROS verso il transverter, non quelli in antenna.",
    "S transvertorem rádio měří budicí výkon a PSV směrem k transvertoru, ne na anténě.")),
(P, "Po is below half of the set power", T(
    "Po е под половината от зададената мощност", "Po ниже половины заданной мощности", "Po 低于设定功率的一半",
    "Po 低於設定功率的一半", "La Po está por debajo de la mitad de la potencia ajustada",
    "La Po és per sota de la meitat de la potència fixada", "A Po está abaixo de metade da potência definida",
    "Po este sub jumătate din puterea setată", "Po er under halvdelen af den indstillede effekt",
    "Po jest poniżej połowy ustawionej mocy", "La Po est inférieure à la moitié de la puissance réglée",
    "A Po está abaixo da metade da potência definida", "Po er under halvparten av innstilt effekt",
    "La Po è sotto la metà della potenza impostata", "Po je pod polovinou nastaveného výkonu")),
(P, "ALC above 50 %: lower the audio level", T(
    "ALC над 50 %: намалете нивото на звука", "ALC выше 50 %: уменьшите уровень звука", "ALC 高于 50 %：请降低音频电平",
    "ALC 高於 50 %：請降低音頻電平", "ALC por encima del 50 %: baje el nivel de audio", "ALC per sobre del 50 %: abaixeu el nivell d'àudio",
    "ALC acima de 50 %: baixe o nível de áudio", "ALC peste 50 %: reduceți nivelul audio", "ALC over 50 %: sænk lydniveauet",
    "ALC powyżej 50 %: zmniejsz poziom audio", "ALC au-dessus de 50 % : baissez le niveau audio",
    "ALC acima de 50 %: abaixe o nível de áudio", "ALC over 50 %: senk lydnivået", "ALC sopra il 50 %: abbassa il livello audio",
    "ALC nad 50 %: snižte úroveň zvuku")),
]
