# Abtastrate und Abtastdauer

Die Symbolleiste hat zwei Listen für die Länge der Erfassung. Die obere Liste ist die Abtastdauer. Die untere Liste ist die Abtastrate.

- Die **Abtastdauer** ist die Zeitdauer der Erfassung.
- Die **Abtastrate** ist die Anzahl der Abtastwerte pro Sekunde für jeden Kanal.

Die verfügbaren Werte ändern sich mit dem Gerät, der USB-Verbindung, dem Betriebsmodus und dem Kanalmodus.

## Maximale Abtastdauer

**Puffermodus.** Der Speicher des Geräts begrenzt die Abtastdauer. Verwenden Sie diese Formel:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

Das DSLogic Plus hat 256 Mbit Speicher. Dies sind zwei Beispiele:

- Bei 100 MHz mit 16 Kanälen ist die maximale Abtastdauer etwa 167,77 ms.
- Bei 400 MHz mit 1 Kanal ist die maximale Abtastdauer etwa 671,09 ms.

**Stream-Modus.** Der Speicher des Computers begrenzt die Abtastdauer. Die App kann 16 G Abtastwerte für jeden Kanal halten. Dies sind zwei Beispiele:

- Bei 1 MHz ist die maximale Abtastdauer etwa 4,77 Stunden.
- Bei 100 MHz ist die maximale Abtastdauer etwa 2,86 Minuten.

## Die Abtastrate auswählen

Stellen Sie die Abtastrate auf das 4- bis 10-Fache der höchsten Frequenz im Signal ein.

Beim 4-Fachen der Signalfrequenz zeichnet die App jede Flanke auf. Aber die Zeit jeder Flanke hat einen Fehler von bis zu 25 % der Signalperiode. Beim 10-Fachen der Signalfrequenz sinkt der Fehler auf 10 %.

Der Zeitfehler einer Flanke ist gleich einer Abtastperiode oder kleiner. Zum Beispiel ist die Abtastperiode bei 100 MHz 10 ns. Darum ist der Fehler jeder Flanke ±10 ns oder kleiner.

![Die Wirkung der Abtastrate auf den aufgezeichneten Signalverlauf](../figures/sample-rate-effect.png)

Dies sind typische Werte:

| Signal | Typische Abtastrate |
| --- | --- |
| UART mit 115200 Baud | 2 MHz |
| I2C mit 400 kHz | 4 MHz bis 10 MHz |
| SPI mit 40 MHz | 400 MHz |

## Keine zu hohe Abtastrate verwenden

Eine höhere Abtastrate gibt einen genaueren Signalverlauf. Aber eine hohe Abtastrate hat auch diese Probleme:

1. Die App zeichnet mehr Daten pro Sekunde auf. Darum wird die maximale Abtastdauer kleiner. Die App braucht auch mehr Zeit, um die Daten zu zeigen und zu dekodieren.
2. Ein langsames Signal kann langsame Flanken haben. Bei einer hohen Abtastrate kann die App kleine Impulse an der Schwelle während jeder langsamen Flanke aufzeichnen. Diese Impulse können Fehler in den Dekodern verursachen.

Wenn Sie unerwünschte kurze Impulse auf langsamen Signalen sehen, verringern Sie die Abtastrate. Sie können auch **Filterziele** auf **1 Abtasttakt** einstellen. Siehe [Geräteoptionen](05-device-options.md).

## Die Abtastrate und die Dauer einstellen

1. Stellen Sie den Betriebsmodus und den Kanalmodus ein. Siehe [Geräteoptionen](05-device-options.md).
2. Wählen Sie in der unteren Liste der Symbolleiste die Abtastrate aus.
3. Wählen Sie in der oberen Liste der Symbolleiste die Abtastdauer aus.

> [!NOTE]
> Wenn Sie den Kanalmodus ändern, kann die App die Abtastrate ändern. Prüfen Sie die Abtastrate nach jeder Änderung der Geräteoptionen erneut.
