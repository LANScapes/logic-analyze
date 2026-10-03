# Metingen

U kunt de golfvorm meten met de muis of met cursors. Een cursor is een verticale lijn op een tijd in de opname.

## Een puls meten met de aanwijzer

Zet de aanwijzer op een puls van een kanaal. Een vak bij de aanwijzer toont deze waarden voor de puls:

- **Breedte**: de tijdsduur van de puls.
- **Periode**: de tijd van een flank tot de volgende flank in dezelfde richting.
- **Frequentie**: 1 gedeeld door de periode.
- **Duty cycle**: de hoge tijd als percentage van de periode.

![De meting bij de aanwijzer](../figures/hover-measurement.png)

Om dit vak in of uit te schakelen, opent u het meetdock en gebruikt u **Zwevende meting inschakelen**.

## De flanken in een gebied tellen

1. Zet de aanwijzer op de golfvorm van het kanaal, tussen het hoge niveau en het lage niveau.
2. Beweeg de aanwijzer naar het begin van het gebied.
3. Klik met de linkermuisknop.
4. Beweeg de aanwijzer naar het einde van het gebied. De app toont het aantal flanken, stijgende flanken en dalende flanken.
5. Klik nog een keer met de linkermuisknop om de meting te voltooien.

## De tijd tussen twee flanken meten

1. Zet de aanwijzer op de eerste flank.
2. Klik met de linkermuisknop.
3. Beweeg de aanwijzer naar de tweede flank. De app toont de tijd en het aantal samples tussen de twee flanken.
4. Klik nog een keer met de linkermuisknop om de meting te voltooien.

![De tijd tussen twee flanken](../figures/edge-distance.png)

## Een cursor toevoegen

Gebruik een van deze methoden:

- Dubbelklik in het golfvormgebied met de linkermuisknop op de tijd die u wilt. Als de aanwijzer dicht bij een flank is, springt de cursor naar de flank.
- Klik in de tijdliniaal met de linkermuisknop. Er verschijnt een pijl op de liniaal. Klik op de pijl om een cursor toe te voegen.

![Een cursor toevoegen vanuit de tijdliniaal](../figures/ruler-insert-cursor.png)

Elke cursor heeft een nummer. De nummers beginnen bij 1.

## Een cursor verplaatsen

Gebruik een van deze methoden:

- Zet de aanwijzer op de cursor. De cursorlijn wordt dikker. Klik op de cursor. Beweeg de muis. Klik nog een keer om de cursor los te laten. Dicht bij een flank springt de cursor naar de flank.
- Klik in de tijdliniaal met de linkermuisknop op de nieuwe tijd. De liniaal toont de nummers van alle cursors. Klik op het nummer van de cursor die u wilt verplaatsen.

![Een cursor verplaatsen vanuit de tijdliniaal](../figures/ruler-move-cursor.png)

## Naar een cursor gaan

1. Klik in de tijdliniaal met de rechtermuisknop. De liniaal toont de nummers van alle cursors.
2. Klik op het nummer van een cursor. De golfvorm beweegt naar de positie van die cursor.

![Naar cursor 3 gaan](../figures/ruler-jump-cursor.png)

## Meten met cursors

Om het meetdock te openen, klikt u op de werkbalk op **Meten** of drukt u op `M`. Het dock heeft deze groepen:

- **Cursorafstand**: de tijd en het aantal samples tussen twee cursors.
- **Flanken**: het aantal flanken op één kanaal tussen twee cursors.
- **Cursors**: de tijd en het samplenummer van elke cursor.

Voer deze stappen uit om een tijdmeting toe te voegen:

1. Klik in de groep **Cursorafstand** op de knop **+**.
2. Klik op het startveld en selecteer de eerste cursor.
3. Klik op het eindveld en selecteer de tweede cursor.

Het dock toont het resultaat in de kolom **Tijd/samples**.

Voer deze stappen uit om een flanktelling toe te voegen:

1. Klik in de groep **Flanken** op de knop **+**.
2. Selecteer de startcursor en de eindcursor.
3. Selecteer het kanaal.

Het dock toont het aantal stijgende flanken, dalende flanken en alle flanken.

Om een meting te verwijderen, klikt u op de knop **×** in de rij van die meting.

## Een cursor verwijderen

Gebruik een van deze methoden:

- Klik op de **×** op het cursorlabel in de tijdliniaal.
- Klik op de knop **×** van de cursor in de groep **Cursors** van het meetdock.

De app geeft de overige cursors nieuwe nummers.
