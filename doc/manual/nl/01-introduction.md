# Inleiding

## Over Logic Analyze

Logic Analyze is een macOS-app voor logische analysers van DreamSourceLab. LANScapes levert de app. De app komt van DSView, een programma van DreamSourceLab. DSView gebruikt software van het sigrok-project.

De app neemt digitale signalen op van een logische analyser van het type DSLogic. Daarna toont de app de signalen als golfvormen. U kunt de golfvormen meten en seriële protocollen decoderen. U kunt de gegevens ook opslaan en naar andere formaten exporteren.

Deze handleiding gebruikt de DSLogic Plus voor de voorbeelden. Andere DSLogic-modellen werken met dezelfde procedures. Hun grenzen voor kanalen, geheugen en samplefrequentie zijn anders.

De app bevat ook het hulpprogramma `dslcap`. Dit hulpprogramma neemt gegevens op zonder het hoofdvenster. Zie [Het hulpprogramma dslcap](13-dslcap.md).

## Over deze handleiding

Deze handleiding is gebaseerd op ASD-STE100 Simplified Technical English. Elke zin is kort. Elke stap van een procedure geeft één instructie. Elke vakterm in deze handleiding heeft maar één betekenis. [Vaktermen en vakwerkwoorden](15-terms.md) geeft de lijst van de vaktermen.

Deze handleiding gebruikt deze tekstopmaak:

- **Vette tekst** toont een label in de app, bijvoorbeeld een knop, een menu-item of een veld.
- `Codetekst` toont een toets op het toetsenbord, een opdracht, een bestandsnaam of een waarde die u typt.
- Een pad door menu's gebruikt het teken ›, bijvoorbeeld **Bestand** › **Opslaan...**.
- Een lijst met genummerde stappen is een procedure. Voer de stappen uit in de gegeven volgorde.

## Veiligheidsinstructies

Deze handleiding gebruikt deze labels voor veiligheidsinstructies:

- **WAARSCHUWING** geeft een risico op letsel of dood aan.
- **LET OP** geeft een risico op schade aan apparatuur of een risico voor uw gegevens aan.
- **OPMERKING** geeft informatie die u helpt. De informatie na **OPMERKING** geeft geen instructie.

Een veiligheidsinstructie staat vóór de stap waarvoor zij geldt. Lees alle veiligheidsinstructies voordat u een procedure begint.

> [!WARNING]
> Sluit de meetdraden niet aan op netspanning. Sluit de meetdraden niet aan op een stroomkring die een elektrische verbinding met netspanning heeft. De meetdraden hebben een elektrische verbinding met de computer. De spanning kan letsel of de dood veroorzaken.

> [!CAUTION]
> Zet op een kanaalingang geen spanning die hoger is dan de grens in de specificatie van het apparaat. Een te hoge spanning kan de logische analyser beschadigen.

> [!CAUTION]
> De massadraden van de logische analyser hebben via de USB-kabel een verbinding met de massa van uw computer. Sluit de massadraden alleen aan op de massa van de schakeling die u meet. Als de twee massa's verschillende spanningen hebben, kan een grote stroom lopen. Deze stroom kan de schakeling, de logische analyser en de computer beschadigen.

## Systeemvereisten

Deze apparatuur is nodig:

- Een Mac met macOS. De release-informatie geeft de minimale macOS-versie.
- Een USB-poort. Een USB 3.0-poort geeft de hoogste snelheid. Een USB 2.0-poort werkt ook.
- Een logische analyser van het type DSLogic, de USB-kabel en de meetkabel.

U kunt de app zonder logische analyser gebruiken. Het apparaat **Demo** maakt testsignalen. U kunt ook een gegevensbestand van een eerdere opname openen.
