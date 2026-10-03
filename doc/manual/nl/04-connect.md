# De DSLogic Plus aansluiten

## De USB-kabel aansluiten

> [!NOTE]
> Gebruik de USB-kabel die bij het apparaat hoort, of een korte USB-kabel van goede kwaliteit. Sluit de kabel direct aan op een poort van de computer. Een USB-hub of een lange kabel kan fouten in een opname veroorzaken.

1. Sluit de USB-kabel aan op de DSLogic Plus.
2. Sluit het andere einde van de USB-kabel aan op een USB-poort van de computer.
3. Controleer of het lampje op de DSLogic Plus aangaat. Voordat de app start, is het lampje rood.
4. Start Logic Analyze.
5. Controleer of het lampje groen wordt.
6. Controleer of de apparatenlijst op de werkbalk **DSLogic Plus** toont.

![De USB-verbinding](../figures/usb-connection.png)

Als de apparatenlijst het apparaat niet toont, voer dan deze stappen uit:

1. Koppel de USB-kabel los van de computer.
2. Wacht 5 seconden.
3. Sluit de USB-kabel aan op een andere USB-poort.
4. Als de apparatenlijst het apparaat na stap 3 niet toont, stop de app en start de app opnieuw.

> [!NOTE]
> Maar één programma tegelijk kan het apparaat gebruiken. Als `dslcap` of een ander programma het apparaat gebruikt, vindt de app het apparaat niet.

## De meetkabel aansluiten

De meetkabel heeft 16 kanaaldraden. Elke kanaaldraad heeft een afscherming, een signaaleinde en een massa-einde. De kleuren van de draden geven de kanalen 0 tot 15 aan. Nog een draad heeft deze signalen:

- **CK**: De ingang voor een externe klok. Gebruik deze alleen met de instelling **Externe klok gebruiken**.
- **TI**: De ingang voor een extern triggersignaal.
- **TO**: De uitgang voor het triggersignaal. Het apparaat stuurt een puls op TO als de trigger optreedt.

Normaal sluit u de draden CK, TI en TO niet aan.

![De meetkabel en de kanalen](../figures/probe-cable-channels.png)

1. Sluit de meetkabel aan op de ingangsconnector van de DSLogic Plus.
2. Druk de connector volledig in het apparaat.

## De kanalen aansluiten op de schakeling

> [!WARNING]
> Sluit de meetdraden niet aan op netspanning. Sluit de meetdraden niet aan op een stroomkring die een elektrische verbinding met netspanning heeft. De spanning kan letsel of de dood veroorzaken.

> [!CAUTION]
> Controleer voordat u een massadraad aansluit of de massa van de schakeling en de massa van de computer dezelfde spanning hebben. Een spanningsverschil kan een grote stroom veroorzaken. Deze stroom kan de apparatuur beschadigen.

1. Schakel de voeding van de schakeling die u meet uit.
2. Sluit ten minste één massadraad aan op de massa van de schakeling.
3. Sluit elke kanaaldraad die u gebruikt aan op een signaal in de schakeling.
4. Controleer of geen enkele meetdraad een ander contact raakt.
5. Schakel de voeding van de schakeling in.

![Massaverbindingen: één gemeenschappelijke massa (links) of één massa voor elk kanaal (rechts)](../figures/probe-grounding.png)

Voor signalen met een frequentie lager dan 5 MHz is één massadraad voor alle kanalen voldoende. Voor signalen met een hogere frequentie sluit u het massa-einde van elke kanaaldraad aan op de massa dicht bij het signaal. Korte massaverbindingen geven schone signaalflanken.

## De DSLogic Plus loskoppelen

> [!CAUTION]
> Koppel de USB-kabel niet los tijdens een opname. Als u de kabel loskoppelt, kunnen de gegevens van de opname fouten hebben.

1. Stop de opname. Klik op **Stop** als deze knop op de werkbalk staat.
2. Schakel de voeding van de schakeling uit.
3. Koppel de meetdraden los van de schakeling.
4. Koppel de USB-kabel los.
