# Oscilloscoopmodus en data-acquisitiemodus

Logic Analyze kan ook de DSCope-oscilloscopen van DreamSourceLab bedienen. Een DSCope heeft twee apparaatmodi:

- **Oscilloscoop**: voor signalen met een constante periode en voor één signaalvoorwaarde.
- **Data-acquisitie**: voor langzame signalen over een lange tijd, bijvoorbeeld een voedingsspanning of de uitgang van een sensor.

Deze modi zijn niet beschikbaar op de DSLogic-apparaten. Dit hoofdstuk geeft alleen de belangrijkste procedures.

## De DSCope aansluiten

> [!WARNING]
> Sluit de meetprobes niet aan op netspanning. Sluit de meetprobes niet aan op een stroomkring die een elektrische verbinding met netspanning heeft. De spanning kan letsel of de dood veroorzaken.

> [!CAUTION]
> De massa van de meetprobes, de massa van de DSCope en de massa van de computer hebben een verbinding met elkaar. Sluit de massa van de meetprobe alleen aan op een punt met dezelfde spanning als de massa van de computer. Een spanningsverschil kan de apparatuur beschadigen.

1. Sluit de DSCope met de USB-kabel aan op de computer.
2. Start Logic Analyze. Controleer of de apparatenlijst de DSCope toont.
3. Sluit de meetprobes aan op de ingangen van de DSCope.
4. Stel de verzwakkingsschakelaar op elke meetprobe in.
5. Sluit de massaklem van elke meetprobe aan op de massa van de schakeling.
6. Sluit de punt van de meetprobe aan op het signaal.

## Apparaatopties

Klik op **Opties** › **Apparaatopties...** of druk op `O`.

- **Bedrijfsmodus**: **Normaal** voor metingen. **Interne test** is alleen voor tests van het apparaat.
- **Bandbreedtelimiet**: **Volledige bandbreedte** of **20MHz**. De limiet van 20 MHz verlaagt hoogfrequente ruis.

## De DSCope kalibreren

De versterking en de offset van de ingangen veranderen met de temperatuur en de luchtvochtigheid. Kalibreer de DSCope om de metingen nauwkeurig te houden.

### Automatische kalibratie

> [!CAUTION]
> Koppel vóór de kalibratie alle meetprobes los van de ingangen. Een signaal op een ingang tijdens de kalibratie geeft onjuiste kalibratiewaarden.

1. Open het venster **Apparaatopties**.
2. Klik op **Automatische kalibratie**.
3. Koppel alle meetprobes los. Klik op **OK**. De kalibratie duurt enkele minuten.
4. Als de kalibratie voltooid is, klikt u op **Opslaan** om het resultaat te bewaren.

Om de kalibratie te stoppen, klikt u op **Afbreken**. Het apparaat gebruikt dan de vorige kalibratiewaarden.

### Handmatige kalibratie

1. Open het venster **Apparaatopties**.
2. Klik op **Handmatige kalibratie**.
3. Klik op de werkbalk op **Start**.
4. Om de offset in te stellen, sluit u de meetprobe aan op massa. Om de versterking in te stellen, sluit u de meetprobe aan op een signaal met een bekende spanning.
5. Stel de verticale schaal in die u wilt kalibreren.
6. Beweeg de schuifregelaar **VOFF** of **VGAIN** van het kanaal tot de golfvorm juist is.
7. Voer de stappen 5 en 6 opnieuw uit voor elke verticale schaal.
8. Klik op **Opslaan**.

Om de wijzigingen te verwerpen, klikt u op **Afbreken**. Om de wijzigingen alleen te gebruiken tot u het apparaat loskoppelt, klikt u op **Afsluiten**. Om terug te gaan naar de beginwaarden, klikt u op **Herstellen**. Voer na het herstellen de automatische kalibratie opnieuw uit.

## Kanaalinstellingen

Elk kanaal heeft deze bedieningselementen links in het golfvormgebied:

- **Inschakelen**: schakelt het kanaal in of uit.
- **Verticale schaal**: de spanning per schaaldeel. Het venster heeft 10 schaaldelen. Om de schaal te wijzigen, draait u het muiswiel op de draaiknop of klikt u op het bovenste of het onderste deel van de draaiknop. U kunt ook op `0` of `1` drukken om de draaiknop van een kanaal te selecteren en daarna op `↑` of `↓` drukken.
- **Koppeling**: **DC** of **AC**.
- **Verzwakking van de meetprobe**: stel **x1** of **x10** in, gelijk aan de schakelaar op de meetprobe.
- **AUTO**: stelt de verticale schaal, de horizontale schaal en het triggerniveau in voor het signaal op de ingang.

Om de golfvorm van een kanaal omhoog of omlaag te bewegen, sleept u het kanaallabel.

## Horizontale schaal

Selecteer de tijd per schaaldeel in de lijst op de werkbalk. U kunt ook het muiswiel in het golfvormgebied draaien.

## Starten en stoppen

- Klik op **Start** of druk op `S` om een doorlopende opname te starten. Klik op **Stop** om de opname te stoppen.
- Klik op **Enkel** of druk op `I` om één golfvorm op te nemen en te stoppen.

## Trigger

Klik op **Trigger** of druk op `T` om het triggerdock te openen. Het dock heeft deze instellingen:

- **Triggerpositie**: de positie van het triggerpunt in de opname, als percentage.
- **Holdofftijd**: de tijd na een trigger waarin het apparaat nieuwe triggers negeert. Gebruik deze om een stabiele golfvorm van groepen pulsen te krijgen.
- **Triggergevoeligheid**: de spanningsverandering die nodig is voor een trigger. Een grotere waarde negeert meer ruis.
- **Triggerbronnen**: **Auto**, **Kanaal 0**, **Kanaal 1**, **Kanaal 0 && 1** of **Kanaal 0 | 1**.
- **Triggertypen**: **Stijgende flank** of **Dalende flank**.

Om het triggerniveau in te stellen, klikt u op het label van het triggerniveau van het kanaal. Beweeg de muis. Klik nog een keer om het niveau vast te zetten.

## Metingen

### Automatische metingen

De onderkant van het golfvormgebied heeft 10 vakken voor automatische metingen.

1. Klik op een meetvak.
2. Selecteer het kanaal.
3. Selecteer de meting. Om het vak te wissen, klikt u op **Herstellen**.

De app behoudt deze instellingen voor de volgende start.

### Cursors

- Om een tijdcursor toe te voegen, klikt u op de tijdliniaal. U kunt ook met de rechtermuisknop in het golfvormgebied klikken en **Y-cursor toevoegen** selecteren.
- Om een spanningscursor toe te voegen, klikt u met de rechtermuisknop in het golfvormgebied en selecteert u **X-cursor toevoegen**. Elke spanningscursor heeft twee horizontale lijnen. Het label tussen de lijnen toont het spanningsverschil.
- Om de tijd tussen twee cursors te meten, gebruikt u de groep **Cursorafstand** in het meetdock.

### Meten met de aanwijzer

Nadat u de opname hebt gestopt, zet u de aanwijzer op de golfvorm. De app toont de spanning van de sample bij de aanwijzer.

Om een tijd te meten, dubbelklikt u in een leeg gebied van de golfvorm. Klik op het tweede punt. Klik op het derde punt om de frequentie, de periode en de duty cycle te zien. Klik met de rechtermuisknop om te annuleren.

## Spectrum (FFT)

1. Klik op **Functie** › **FFT**.
2. Selecteer **FFT inschakelen**.
3. Stel **FFT-lengte**, **Sample-interval**, **FFT-bron** en **FFT-venster** in.
4. Stel **Y-asmodus** en **DBV-bereik** in.
5. Klik op **OK**.

Het spectrum verschijnt onder de golfvorm. Draai het muiswiel in het spectrum om de frequentieschaal te zoomen. Sleep het spectrum om het te bewegen. Zet de aanwijzer op het spectrum om de frequentie en de amplitude te zien.

## Rekenkanaal

1. Klik op **Functie** › **Wiskunde**.
2. Selecteer **Inschakelen**.
3. Selecteer het **Bewerkingstype**: **Optellen**, **Aftrekken**, **Vermenigvuldigen** of **Delen**.
4. Selecteer de **1e bron** en de **2e bron**.
5. Klik op **OK**.

## Lissajousfiguur

1. Klik op **Opties** › **Weergave** › **Lissajous**.
2. Selecteer **Inschakelen**.
3. Selecteer het kanaal voor de **X-as** en de **Y-as**.
4. Klik op **OK**.

## Data-acquisitiemodus

1. Selecteer in de lijst van apparaatmodi op de werkbalk **Data-acquisitie**.
2. Open het venster **Apparaatopties**.
3. Stel voor elk kanaal **Inschakelen**, **Koppeling** en **Volt/div** in.
4. Om een andere eenheid te tonen, stelt u **Toewijzingseenheid**, **Toewijzing min.** en **Toewijzing max.** in. Toon bijvoorbeeld de uitgang van een temperatuursensor in °C.
5. Klik op **OK**.
6. Selecteer op de werkbalk de samplefrequentie en de sampleduur.
7. Klik op **Start** of druk op `S`.

U kunt de kanaalinstellingen tijdens de opname niet wijzigen. Bij de hoogste samplefrequentie van 10 MHz is de maximale sampleduur ongeveer 10 seconden. Bij 1 kHz kan de opname een dag doorgaan.

De data-acquisitiemodus gebruikt de kalibratie van de oscilloscoopmodus. Als een kanaal een offset toont, kalibreert u het apparaat in de oscilloscoopmodus.
