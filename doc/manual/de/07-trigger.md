# Trigger

Ein Trigger ist eine Bedingung in den Signalen. Wenn die Bedingung eintritt, markiert das Gerät diese Zeit als Triggerpunkt. Mit dem Trigger erfassen Sie den Teil des Signals, den Sie untersuchen möchten.

Die App hat zwei Arten von Trigger:

- **Einfacher Trigger**: Eine Flanke oder ein Pegel auf einem oder mehreren Kanälen.
- **Erweiterter Trigger**: Eine Folge von Bedingungen oder ein Wert auf einem seriellen Bus.

Um das Trigger-Dock zu öffnen, klicken Sie in der Symbolleiste auf **Trigger** oder drücken Sie `T`.

> [!NOTE]
> Wenn das Signal nicht mit der Triggerbedingung übereinstimmt, wartet die Erfassung weiter. Um das Signal ohne den Trigger zu sehen, klicken Sie auf **Sofort**. Um das Warten zu beenden, klicken Sie auf **Stopp**.

## Triggerposition

Die Einstellung **Triggerposition** legt fest, wo der Triggerpunkt in der Erfassung liegt. Der Wert ist ein Prozentsatz der Abtastdauer.

- Ein kleiner Wert, zum Beispiel 10 %, zeigt mehr vom Signal nach dem Trigger.
- Ein großer Wert, zum Beispiel 90 %, zeigt mehr vom Signal vor dem Trigger.

Die Triggerposition verwendet den Speicher des Geräts. Darum können Sie sie nur im Puffermodus einstellen. Im Stream-Modus ist die Triggerposition immer etwa 1 %.

![Triggerposition 10 % (links) und 90 % (rechts)](../figures/trigger-position.png)
<!-- TODO: new screenshot -->

## Einfacher Trigger

Jede Kanalbeschriftung im Signalbereich hat fünf Trigger-Schaltflächen. Von links nach rechts sind die Schaltflächen:

1. Steigende Flanke
2. High-Pegel
3. Fallende Flanke
4. Low-Pegel
5. Steigende oder fallende Flanke

![Die Trigger-Schaltflächen auf einer Kanalbeschriftung](../figures/simple-trigger-buttons.png)

Um einen einfachen Trigger einzustellen, führen Sie diese Schritte aus:

1. Öffnen Sie das Trigger-Dock.
2. Wählen Sie **Einfacher Trigger** aus.
3. Klicken Sie auf der Beschriftung eines Kanals auf die gewünschte Trigger-Schaltfläche. Die Schaltfläche erscheint in einer anderen Farbe.
4. Um den Trigger von einem Kanal zu entfernen, klicken Sie erneut auf dieselbe Schaltfläche.
5. Stellen Sie die **Triggerposition** ein.

Wenn Sie einen Trigger auf mehr als einem Kanal einstellen, müssen alle Bedingungen beim selben Abtastwert eintreten (logisches UND).

## Erweiterter Trigger

> [!NOTE]
> Der erweiterte Trigger ist nur im Puffermodus verfügbar. Um ihn zu verwenden, stellen Sie **Betriebsmodus** auf **Puffermodus**. Siehe [Geräteoptionen](05-device-options.md).

Um den erweiterten Trigger zu verwenden, wählen Sie im Trigger-Dock **Erweiterter Trigger** aus. Dann wählen Sie die Registerkarte **Stufentrigger** oder **Serieller Trigger** aus.

### Werte für jeden Kanal

Der Stufentrigger und der serielle Trigger verwenden eine Zeile mit 16 Zeichen. Jedes Zeichen ist die Bedingung für einen Kanal. Das Zeichen rechts ist Kanal 0. Das Zeichen links ist Kanal 15.

| Zeichen | Bedingung |
| --- | --- |
| `X` | Alle Werte (der Kanal hat keine Wirkung). |
| `0` | Low-Pegel. |
| `1` | High-Pegel. |
| `R` | Steigende Flanke. |
| `F` | Fallende Flanke. |
| `C` | Steigende oder fallende Flanke. |

### Stufentrigger

Ein Stufentrigger ist eine Folge von Bedingungen. Jede Bedingung ist eine Stufe. Das Gerät prüft zuerst Stufe 0. Wenn die Bedingung einer Stufe eintritt, geht das Gerät zur nächsten Stufe. Der Trigger tritt ein, wenn die letzte Stufe vollständig ist. Sie können bis zu 16 Stufen verwenden.

Jede Stufe hat diese Einstellungen:

- Zwei Zeilen mit Kanalbedingungen.
- Für jede Zeile `==` oder `!=`. Mit `==` tritt die Bedingung ein, wenn die Kanäle mit der Zeile übereinstimmen. Mit `!=` tritt die Bedingung ein, wenn die Kanäle nicht mit der Zeile übereinstimmen.
- **Und** oder **Oder**. Diese Einstellung verbindet die zwei Zeilen.
- **Zähler**: Die Anzahl, wie oft die Bedingung eintreten muss, bevor die Stufe vollständig ist.
- **Fortlaufend**: Wenn Sie dieses Kontrollkästchen auswählen, muss die Bedingung in Abtastwerten eintreten, die ohne Unterbrechung aufeinander folgen.

![Die Einstellungen des Stufentriggers](../figures/stage-trigger-panel.png)
<!-- TODO: new screenshot -->

Um einen Stufentrigger einzustellen, führen Sie diese Schritte aus:

1. Wählen Sie in **Anzahl Triggerstufen** die Anzahl der Stufen aus.
2. Klicken Sie in der Liste der Stufen rechts auf Stufe 0.
3. Geben Sie die Kanalbedingungen in der ersten Zeile ein.
4. Wenn notwendig, geben Sie die Kanalbedingungen in der zweiten Zeile ein und wählen Sie **Und** oder **Oder** aus.
5. Geben Sie einen Wert in **Zähler** ein.
6. Führen Sie die Schritte 2 bis 5 für jede weitere Stufe erneut aus.

Dies sind drei Beispiele.

**Beispiel 1.** Trigger, wenn Kanal 0 länger als 1000 Abtastwerte high bleibt:

1. Stellen Sie **Anzahl Triggerstufen** auf 1.
2. Geben Sie in Stufe 0 in der ersten Zeile `1` für Kanal 0 ein.
3. Wählen Sie **Fortlaufend** aus.
4. Stellen Sie **Zähler** auf 1000.

![Beispiel 1](../figures/stage-example-level-count.png)

**Beispiel 2.** Trigger bei einer steigenden Flanke auf Kanal 0 oder einer fallenden Flanke auf Kanal 1:

1. Stellen Sie **Anzahl Triggerstufen** auf 1.
2. Geben Sie in Stufe 0 in der ersten Zeile `R` für Kanal 0 ein.
3. Geben Sie in der zweiten Zeile `F` für Kanal 1 ein.
4. Wählen Sie **Oder** aus.

![Beispiel 2](../figures/stage-example-or.png)

**Beispiel 3.** Trigger bei einer steigenden Flanke auf Kanal 0, dann 100 fallenden Flanken auf Kanal 1, dann einem High-Pegel auf Kanal 2:

1. Stellen Sie **Anzahl Triggerstufen** auf 3.
2. Geben Sie in Stufe 0 `R` für Kanal 0 ein.
3. Geben Sie in Stufe 1 `F` für Kanal 1 ein. Stellen Sie **Zähler** auf 100.
4. Geben Sie in Stufe 2 `1` für Kanal 2 ein.

![Beispiel 3](../figures/stage-example-sequence.png)

### Serieller Trigger

Ein serieller Trigger findet einen Datenwert auf einem seriellen Bus. Er arbeitet wie ein Schieberegister. Dies sind die Einstellungen:

- **Startbedingung**: Die Bedingung, die den seriellen Trigger startet.
- **Stoppbedingung**: Die Bedingung, die das Schieberegister löscht.
- **Taktbedingung**: Die Bedingung, die ein Bit zum Schieberegister hinzufügt.
- **Datenkanal**: Der Kanal, der die Daten überträgt.
- **Datenbits**: Die Anzahl der Bits im Wert.
- **Datenwert**: Der Wert, der den Trigger auslöst.

Nach der Startbedingung liest das Gerät den Datenkanal bei jeder Taktbedingung. Das Gerät schiebt dieses Bit in das Schieberegister. Wenn die letzten Bits des Schieberegisters gleich **Datenwert** sind, tritt der Trigger ein. Wenn die Stoppbedingung eintritt, löscht das Gerät das Schieberegister.

![Die Einstellungen des seriellen Triggers](../figures/serial-trigger-panel.png)
<!-- TODO: new screenshot -->

**Beispiel 4.** Trigger, wenn der Wert `010000100` auf einem I2C-Bus auftritt. Kanal 0 ist SCL und Kanal 1 ist SDA.

1. Stellen Sie **Startbedingung** auf eine fallende Flanke auf SDA, während SCL high ist: `F1` in den zwei Zeichen rechts.
2. Stellen Sie **Stoppbedingung** auf eine steigende Flanke auf SDA, während SCL high ist: `R1`.
3. Stellen Sie **Taktbedingung** auf eine steigende Flanke auf SCL: `R` für Kanal 0.
4. Stellen Sie **Datenkanal** auf 1.
5. Stellen Sie **Datenbits** auf 9.
6. Geben Sie `010000100` in **Datenwert** ein.

![Beispiel 4](../figures/serial-example-i2c.png)

**Beispiel 5.** Trigger, wenn der Wert `0x1234` auf MOSI eines SPI-Busses auftritt. Kanal 0 ist CS#, Kanal 1 ist CLK, Kanal 2 ist MISO und Kanal 3 ist MOSI.

1. Stellen Sie **Startbedingung** auf eine fallende Flanke auf CS#: `F` für Kanal 0.
2. Stellen Sie **Stoppbedingung** auf eine steigende Flanke auf CS#: `R` für Kanal 0.
3. Stellen Sie **Taktbedingung** auf eine steigende Flanke auf CLK: `R` für Kanal 1.
4. Stellen Sie **Datenkanal** auf 3.
5. Stellen Sie **Datenbits** auf 16.
6. Geben Sie `0001001000110100` in **Datenwert** ein.

![Beispiel 5](../figures/serial-example-spi.png)

Um den Wert hexadezimal einzugeben, wählen Sie **Eingabe im Hex-Format** aus. Dann geben Sie den Wert im Feld **Hex** ein.
