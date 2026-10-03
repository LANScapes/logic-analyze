# Protocoldecoders

Een protocoldecoder leest de gegevens van een opname en vindt de frames van een protocol, bijvoorbeeld UART, I2C of SPI. De app toont het resultaat als een nieuwe rij boven de kanalen. De app heeft meer dan 100 decoders.

Om het decoderdock te openen, klikt u op de werkbalk op **Decoderen** of drukt u op `D`. Het dock heeft twee delen:

- De decoderlijst, met bovenaan het veld **Decoder zoeken...**.
- De lijst **Decoderingsresultaten**. Deze lijst toont elk item van de decoder als een rij tekst.

![Het decoderdock](../figures/decoder-dock.png)
<!-- TODO: new screenshot -->

## Een decoder toevoegen

> [!NOTE]
> Een decoder met het voorvoegsel `0:` is een kleinere versie. Hij toont de bits niet. U kunt er geen hoger protocol op toevoegen. Hij decodeert sneller en gebruikt minder geheugen.

1. Klik op het veld **Decoder zoeken...**. De lijst van decoders opent.
2. Typ een deel van de protocolnaam, bijvoorbeeld `I2C`. De lijst toont alleen de decoders die overeenkomen met de tekst.
3. Klik op de decoder. Het venster **Decoderopties** opent.
4. Stel de kanalen van het protocol in. Stel bijvoorbeeld **SCL** en **SDA** in voor I2C.
5. Stel de protocolopties in, bijvoorbeeld de baudrate van een UART.
6. Selecteer de rijen met resultaten die de app toont.
7. Stel indien nodig het decodeergebied in. Zie [Een deel van de opname decoderen](#decode-region).
8. Klik op **OK**.

De app decodeert de gegevens en toont de resultaten op een nieuwe rij in het golfvormgebied.

Om meer decoders toe te voegen, voert u de procedure opnieuw uit voor elke decoder.

![De decoderknoppen: de instellingenknop opent de decoderopties](../figures/decoder-buttons.png)

Om de instellingen van een decoder te wijzigen, klikt u in het dock op de instellingenknop van die decoder.

## Een gestapelde decoder toevoegen

Sommige protocollen gebruiken een lager protocol. Het protocol 24xx EEPROM gebruikt bijvoorbeeld I2C. Als u het hogere protocol toevoegt, voegt de app ook de lagere protocollen toe.

1. Typ in het veld **Decoder zoeken...** de naam van het hogere protocol, bijvoorbeeld `24xx`.
2. Klik op de decoder.
3. Stel in het venster **Decoderopties** de opties voor elke protocollaag in.
4. Klik op **OK**.

De resultaten tonen de frames van het lagere protocol en de opdrachten en gegevens van het hogere protocol.

## Een deel van de opname decoderen {#decode-region}

Normaal decodeert de app alle gegevens. Om alleen een deel te decoderen, stelt u een startcursor en een eindcursor in. U kunt bijvoorbeeld de ruis bij een reset van de schakeling negeren. Een korter gebied verkort ook de decodeertijd.

1. Voeg twee cursors toe aan het begin en aan het einde van het gebied. Zie [Metingen](10-measure.md).
2. Open het venster **Decoderopties** van de decoder.
3. Selecteer in de lijst **Start** de startcursor.
4. Selecteer in de lijst **Einde** de eindcursor.
5. Klik op **OK**.

## De resultatenlijst lezen

De lijst **Decoderingsresultaten** toont de items van de decoder in volgorde van tijd. Klik op een rij om de golfvorm naar dat item te bewegen.

Om de kolommen te wijzigen die de lijst toont, klikt u bovenaan de lijst op de instellingenknop.

## Een tekst in de resultaten vinden

1. Typ een tekst in het zoekveld van de lijst **Decoderingsresultaten**.
2. Klik op de rechterpijl om naar de volgende rij met de tekst te gaan. Klik op de linkerpijl om naar de vorige rij te gaan.

De golfvorm beweegt naar het item van elke rij die de zoekfunctie vindt. Als u eerst op een rij klikt, begint het zoeken bij die rij.

![Zoeken in de decoderingsresultaten](../figures/decoder-list-search.png)

Om een reeks bytes te vinden, zet u het teken `-` tussen de bytes. `70-70-70` vindt bijvoorbeeld drie opeenvolgende bytes met de waarde 70.

![Zoeken naar een reeks bytes](../figures/decoder-multibyte-search.png)

> [!NOTE]
> Het zoeken naar een reeks bytes werkt alleen met de decoders UART, I2C en SPI.

## De resultaten exporteren

1. Klik bovenaan de lijst **Decoderingsresultaten** op de opslagknop. Het venster **Protocol exporteren** opent.
2. Selecteer in **Exportformaat** CSV of TXT.
3. Selecteer elke kolom die u wilt exporteren. De app zet alle kolommen in één bestand, in volgorde van tijd.
4. Klik op **OK**.
5. Selecteer de map en typ de bestandsnaam.
6. Klik op **Bewaar**.

## Een decoder verwijderen

![Eén decoder of alle decoders verwijderen](../figures/decoder-delete.png)

- Om één decoder te verwijderen, klikt u op de knop **×** in de rij van die decoder.
- Om alle decoders te verwijderen, klikt u op de knop **×** bovenaan het dock, naast de knop **+**.
