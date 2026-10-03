# Het hulpprogramma dslcap

Het hulpprogramma `dslcap` neemt gegevens op van een DSLogic-apparaat zonder het hoofdvenster. Gebruik het in scripts en in automatische tests. Het hulpprogramma schrijft de samples naar een binair bestand. Het schrijft één JSON-object met het resultaat naar de standaarduitvoer.

Het hulpprogramma staat in de app-bundel:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Maar één programma tegelijk kan het apparaat gebruiken. Stop Logic Analyze voordat u `dslcap` gebruikt.

## De apparaten weergeven

Typ deze opdracht om de apparaten weer te geven die de bibliotheek kan vinden:

```sh
dslcap --list
```

Typ deze opdracht om de USB-identificatie van elk aangesloten DSLogic-apparaat weer te geven:

```sh
dslcap --list-ids
```

De opdracht `--list-ids` leest alleen de informatie die macOS over USB-apparaten bewaart. Hij stuurt geen gegevens naar het apparaat. De uitvoer geeft voor elk apparaat het model, de USB-locatie en een registry-identificatie.

## Gegevens opnemen

Deze opdracht neemt 1000000 samples op de kanalen 0 en 1 op bij 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

Het hulpprogramma schrijft de samples naar `/tmp/capture.bin`. Als er een bestand met deze naam is, stopt het hulpprogramma met een fout. Het hulpprogramma vervangt geen bestand.

Dit zijn de opname-opties:

| Optie | Functie | Beginwaarde |
| --- | --- | --- |
| `--channels LIST` | De kanalen om op te nemen, bijvoorbeeld `0,1,2`. | `0` |
| `--samplerate HZ` | De samplefrequentie in Hz. | `10000000` |
| `--samples N` | Het aantal samples voor elk kanaal. | `1000000` |
| `--vth VOLTS` | De drempelspanning. | `1.6` |
| `--mode MODE` | `buffer` of `stream`. | `buffer` |
| `--trigger CH[:T]` | Een trigger op kanaal CH. Gebruik voor T `R` (stijgende flank), `F` (dalende flank), `C` (stijgende of dalende flank), `1` (hoog niveau) of `0` (laag niveau). | Geen trigger. `R` als u alleen CH geeft. |
| `--trigpos PERCENT` | De triggerpositie als percentage van de samples. | `10` |
| `--timeout SEC` | De maximale tijd van de opname in seconden. | `30` |
| `--out PATH` | Het pad van het uitvoerbestand, zonder de extensie `.bin`. | Deze optie is nodig. |
| `--log-level N` | De hoeveelheid meldingen van de bibliotheek op de standaardfoutuitvoer, van 0 (geen) tot 5 (alle). | `1` |

Het hulpprogramma controleert alle opties voordat het het apparaat gebruikt. Als een optie niet juist is, stopt het hulpprogramma en geeft het een fout.

## Het uitvoerbestand

Het bestand `.bin` bevat de kanalen in de volgorde van hun nummers, vanaf het laagste nummer. Voor elk kanaal bevat het bestand alle samples van dat kanaal. Elke byte bevat 8 samples. De eerste sample is de minst significante bit. De gegevens van elk kanaal vullen een geheel aantal eenheden van 8 bytes. Daarom gebruikt elk kanaal `ceil(samples / 64) × 8` bytes.

## Het JSON-resultaat

Het hulpprogramma schrijft één JSON-object op één regel. Na een juiste opname geeft het object de naam van het apparaat, de samplefrequentie, het aantal samples en de kanalen. Het geeft ook de drempelspanning, de modus, de trigger, de tijd van de opname en het pad van het bestand `.bin`. Als de opname niet juist is, bevat het object een sleutel `error`. Dan maakt het hulpprogramma geen bestand `.bin`.

Gebruik het resultaat alleen als de exitstatus 0 is en het JSON-object volledig is.

## Exitstatus

| Status | Betekenis |
| --- | --- |
| 0 | De opname is voltooid. |
| 1 | Er is een fout opgetreden tijdens de bewerking, bijvoorbeeld een I/O-fout. |
| 2 | Een optie is niet juist, of een instelling is niet beschikbaar op het apparaat. |
| 3 | De opname is niet voltooid. |

## Opties voor programma's die dslcap starten

- `--parent-fd N`: Het hulpprogramma stopt als het programma dat het heeft gestart de pipe met descriptor N sluit. Dan verwijdert het hulpprogramma het uitvoerbestand als de opname niet voltooid is.
- `--res DIR`: De map met de firmwarebestanden. Normaal vindt het hulpprogramma deze map automatisch. U kunt ook de omgevingsvariabele `DSLCAP_RES` instellen.
- `--res-manifest FD`: Het hulpprogramma controleert de SHA-256-waarde van elk firmwarebestand voordat het het bestand naar het apparaat stuurt.

Het bestand `tools/dslcap/README.md` in de broncode geeft alle informatie over deze opties.
