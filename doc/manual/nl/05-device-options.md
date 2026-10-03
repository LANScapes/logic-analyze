# Apparaatopties

## De apparaatopties openen

1. Klik op de werkbalk op **Opties** › **Apparaatopties...**. U kunt ook op `O` drukken.
2. Wijzig de instellingen in het venster **Apparaatopties**.
3. Klik op **OK**.

De instellingen in het venster zijn voor elk apparaatmodel anders. Dit hoofdstuk geeft de instellingen van de DSLogic Plus.

> [!NOTE]
> U kunt de apparaatopties niet wijzigen tijdens een opname.

![Het venster Apparaatopties](../figures/device-options.png)
<!-- TODO: new screenshot -->

## Bedrijfsmodus

De instelling **Bedrijfsmodus** selecteert hoe het apparaat gegevens naar de computer stuurt.

**Buffermodus.** Het apparaat bewaart de samples tijdens de opname in het interne geheugen. Na de opname stuurt het apparaat de gegevens via USB naar de computer. Het geheugen is sneller dan USB. Daarom geeft de buffermodus de hoogste samplefrequenties. De capaciteit van het geheugen begrenst de lengte van de opname. Gebruik de buffermodus voor snelle signalen en korte opnamen.

**Streammodus.** Het apparaat stuurt de samples tijdens de opname naar de computer. Het geheugen van de computer begrenst de lengte van de opname. U kunt de gegevens tijdens de opname zien. De snelheid van de USB-verbinding begrenst de samplefrequentie. Gebruik de streammodus voor langzame signalen en lange opnamen.

**Interne test.** Deze modus is alleen voor tests van het apparaat. Gebruik deze modus niet voor metingen.

## Stopopties

De instelling **Stopopties** geldt alleen voor de buffermodus. Zij bepaalt wat de app doet als u een opname vóór het einde stopt.

- **Direct stoppen**: De app haalt de gegevens niet op van het apparaat. De app toont geen gegevens.
- **Vastgelegde gegevens uploaden**: De app haalt de gegevens op die het apparaat vóór de stop heeft opgenomen. De app toont deze gegevens.

## Drempelniveau

De instelling **Drempelniveau** is de spanning die een laag niveau van een hoog niveau scheidt. Een signaal boven de drempel is een hoog niveau. Een signaal onder de drempel is een laag niveau.

U kunt een waarde van 0,0 V tot 5,0 V instellen in stappen van 0,1 V. Stel de drempel in op ongeveer 50% van de logische spanning van de schakeling. Stel voor een schakeling van 3,3 V ongeveer 1,6 V in.

## Filterdoelen

De instelling **Filterdoelen** verwijdert korte pulsen uit de gegevens.

- **Geen**: De app toont alle samples.
- **1 sampleklok**: De app verwijdert elke puls die korter is dan één sampleperiode.

## Maximale hoogte

De instelling **Max. hoogte** bepaalt de maximale hoogte van elke kanaalrij in het golfvormgebied. **1X** is één eenheid van hoogte. Gebruik een grotere waarde als u maar een klein aantal kanalen toont.

## RLE-compressie inschakelen

Als u **RLE-compressie inschakelen** selecteert, comprimeert het apparaat de gegevens in het geheugen (run-length encoding). Deze instelling geldt alleen voor de buffermodus. Als de signalen een klein aantal flanken hebben, kan het apparaat een langere opname in het geheugen houden. Als de signalen veel flanken hebben, geeft de compressie geen grotere lengte.

## Externe klok gebruiken

Als u **Externe klok gebruiken** selecteert, samplet het apparaat de kanalen bij elke klokflank op de draad CK. Het apparaat gebruikt de interne klok niet. Gebruik deze instelling om een bus op te nemen die een kloksignaal heeft.

## Dalende klokflank gebruiken

Deze instelling geldt alleen met **Externe klok gebruiken**. Normaal samplet het apparaat de kanalen op de stijgende flank van de klok. Als u **Dalende klokflank gebruiken** selecteert, samplet het apparaat de kanalen op de dalende flank van de klok.

## Kanaalmodus

De kanaalmodus bepaalt het aantal kanalen dat het apparaat kan gebruiken. Hij bepaalt ook de maximale samplefrequentie. Een kleiner aantal kanalen geeft een hogere maximale samplefrequentie. Selecteer de kanaalmodus die past bij het aantal en de frequentie van uw signalen.

Voor de DSLogic Plus zijn dit de kanaalmodi:

| Bedrijfsmodus | Kanaalmodus | Maximale samplefrequentie |
| --- | --- | --- |
| Buffermodus | Kanalen 0 tot 15 | 100 MHz |
| Buffermodus | Kanalen 0 tot 7 | 200 MHz |
| Buffermodus | Kanalen 0 tot 3 | 400 MHz |
| Streammodus | 16 kanalen | 20 MHz |
| Streammodus | 12 kanalen | 25 MHz |
| Streammodus | 6 kanalen | 50 MHz |
| Streammodus | 3 kanalen | 100 MHz |

## Kanalen in- en uitschakelen

Onder de kanaalmodi toont het venster een selectievakje voor elk kanaal.

1. Selecteer het selectievakje van elk kanaal dat u gebruikt.
2. Wis het selectievakje van elk kanaal dat u niet gebruikt.
3. Om alle kanalen te selecteren, klikt u op **Alle aan**. Om alle kanalen te wissen, klikt u op **Alle uit**.

In de streammodus kan een kleiner aantal ingeschakelde kanalen een hogere samplefrequentie mogelijk maken.
