# Oszilloskop-Modus und Datenerfassungsmodus

Logic Analyze kann auch die DSCope-Oszilloskope von DreamSourceLab bedienen. Ein DSCope hat zwei Gerätemodi:

- **Oszilloskop**: für Signale mit einer konstanten Periode und für eine einzelne Signalbedingung.
- **Datenerfassung**: für langsame Signale über eine lange Zeit, zum Beispiel eine Versorgungsspannung oder den Ausgang eines Sensors.

Diese Modi sind auf den DSLogic-Geräten nicht verfügbar. Dieses Kapitel gibt nur die wichtigsten Verfahren.

## Das DSCope anschließen

> [!WARNING]
> Schließen Sie die Tastköpfe nicht an Netzspannung an. Schließen Sie die Tastköpfe nicht an einen Stromkreis an, der eine elektrische Verbindung zur Netzspannung hat. Die Spannung kann Verletzungen oder den Tod verursachen.

> [!CAUTION]
> Die Masse der Tastköpfe, die Masse des DSCope und die Masse des Computers haben eine Verbindung miteinander. Schließen Sie die Masse des Tastkopfs nur an einen Punkt an, der die gleiche Spannung wie die Masse des Computers hat. Ein Spannungsunterschied kann die Geräte beschädigen.

1. Schließen Sie das DSCope mit dem USB-Kabel an den Computer an.
2. Starten Sie Logic Analyze. Stellen Sie sicher, dass die Geräteliste das DSCope zeigt.
3. Schließen Sie die Tastköpfe an die Eingänge des DSCope an.
4. Stellen Sie den Abschwächungsschalter an jedem Tastkopf ein.
5. Schließen Sie die Masseklemme jedes Tastkopfs an die Masse der Schaltung an.
6. Schließen Sie die Tastkopfspitze an das Signal an.

## Geräteoptionen

Klicken Sie auf **Optionen** › **Geräteoptionen...** oder drücken Sie `O`.

- **Betriebsmodus**: **Normal** für Messungen. **Interner Test** ist nur für Tests des Geräts.
- **Bandbreitenbegrenzung**: **Volle Bandbreite** oder **20MHz**. Die Begrenzung auf 20 MHz verringert hochfrequentes Rauschen.

## Das DSCope kalibrieren

Die Verstärkung und der Offset der Eingänge ändern sich mit der Temperatur und der Luftfeuchtigkeit. Kalibrieren Sie das DSCope, damit die Messungen genau bleiben.

### Automatische Kalibrierung

> [!CAUTION]
> Trennen Sie vor der Kalibrierung alle Tastköpfe von den Eingängen. Ein Signal an einem Eingang während der Kalibrierung gibt falsche Kalibrierwerte.

1. Öffnen Sie das Fenster **Geräteoptionen**.
2. Klicken Sie auf **Automatische Kalibrierung**.
3. Trennen Sie alle Tastköpfe. Klicken Sie auf **OK**. Die Kalibrierung dauert einige Minuten.
4. Wenn die Kalibrierung vollständig ist, klicken Sie auf **Speichern**, um das Ergebnis zu behalten.

Um die Kalibrierung zu stoppen, klicken Sie auf **Abbrechen**. Das Gerät verwendet dann die vorherigen Kalibrierwerte.

### Manuelle Kalibrierung

1. Öffnen Sie das Fenster **Geräteoptionen**.
2. Klicken Sie auf **Manuelle Kalibrierung**.
3. Klicken Sie in der Symbolleiste auf **Start**.
4. Um den Offset einzustellen, schließen Sie den Tastkopf an Masse an. Um die Verstärkung einzustellen, schließen Sie den Tastkopf an ein Signal mit bekannter Spannung an.
5. Stellen Sie die vertikale Skala ein, die Sie kalibrieren möchten.
6. Bewegen Sie den Schieberegler **VOFF** oder **VGAIN** des Kanals, bis der Signalverlauf korrekt ist.
7. Führen Sie die Schritte 5 und 6 für jede vertikale Skala erneut aus.
8. Klicken Sie auf **Speichern**.

Um die Änderungen zu verwerfen, klicken Sie auf **Abbrechen**. Um die Änderungen nur zu verwenden, bis Sie das Gerät trennen, klicken Sie auf **Beenden**. Um zu den Anfangswerten zurückzugehen, klicken Sie auf **Zurücksetzen**. Führen Sie nach dem Zurücksetzen die automatische Kalibrierung erneut aus.

## Kanaleinstellungen

Jeder Kanal hat diese Bedienelemente links im Signalbereich:

- **Aktivieren**: schaltet den Kanal ein oder aus.
- **Vertikale Skala**: die Spannung pro Teilung. Das Fenster hat 10 Teilungen. Um die Skala zu ändern, drehen Sie das Mausrad über dem Drehknopf oder klicken Sie auf den oberen oder unteren Teil des Drehknopfs. Sie können auch `0` oder `1` drücken, um den Drehknopf eines Kanals auszuwählen, und dann `↑` oder `↓` drücken.
- **Kopplung**: **DC** oder **AC**.
- **Tastkopfabschwächung**: Stellen Sie **x1** oder **x10** passend zum Schalter am Tastkopf ein.
- **AUTO**: stellt die vertikale Skala, die horizontale Skala und den Triggerpegel für das Signal am Eingang ein.

Um den Signalverlauf eines Kanals nach oben oder unten zu bewegen, ziehen Sie die Kanalbeschriftung.

## Horizontale Skala

Wählen Sie die Zeit pro Teilung in der Liste der Symbolleiste aus. Sie können auch das Mausrad im Signalbereich drehen.

## Starten und stoppen

- Klicken Sie auf **Start** oder drücken Sie `S`, um eine fortlaufende Erfassung zu starten. Klicken Sie auf **Stopp**, um sie zu stoppen.
- Klicken Sie auf **Einzeln** oder drücken Sie `I`, um einen Signalverlauf zu erfassen und zu stoppen.

## Trigger

Klicken Sie auf **Trigger** oder drücken Sie `T`, um das Trigger-Dock zu öffnen. Das Dock hat diese Einstellungen:

- **Triggerposition**: die Position des Triggerpunkts in der Erfassung als Prozentsatz.
- **Holdoff-Zeit**: die Zeit nach einem Trigger, in der das Gerät neue Trigger ignoriert. Verwenden Sie sie, um einen stabilen Signalverlauf von Impulsgruppen zu erhalten.
- **Triggerempfindlichkeit**: die Spannungsänderung, die für einen Trigger notwendig ist. Ein größerer Wert ignoriert mehr Rauschen.
- **Triggerquellen**: **Auto**, **Kanal 0**, **Kanal 1**, **Kanal 0 && 1** oder **Kanal 0 | 1**.
- **Triggertypen**: **Steigende Flanke** oder **Fallende Flanke**.

Um den Triggerpegel einzustellen, klicken Sie auf die Triggerpegel-Beschriftung des Kanals. Bewegen Sie die Maus. Klicken Sie erneut, um den Pegel festzulegen.

## Messungen

### Automatische Messungen

Der untere Rand des Signalbereichs hat 10 Felder für automatische Messungen.

1. Klicken Sie auf ein Messfeld.
2. Wählen Sie den Kanal aus.
3. Wählen Sie die Messung aus. Um das Feld zu leeren, klicken Sie auf **Zurücksetzen**.

Die App behält diese Einstellungen für den nächsten Start.

### Cursor

- Um einen Zeit-Cursor hinzuzufügen, klicken Sie auf das Zeitlineal. Sie können auch im Signalbereich mit der rechten Maustaste klicken und **Y-Cursor hinzufügen** auswählen.
- Um einen Spannungs-Cursor hinzuzufügen, klicken Sie im Signalbereich mit der rechten Maustaste und wählen Sie **X-Cursor hinzufügen** aus. Jeder Spannungs-Cursor hat zwei waagerechte Linien. Die Beschriftung zwischen den Linien zeigt die Spannungsdifferenz.
- Um die Zeit zwischen zwei Cursorn zu messen, verwenden Sie die Gruppe **Cursorabstand** im Mess-Dock.

### Mit dem Zeiger messen

Nachdem Sie die Erfassung gestoppt haben, setzen Sie den Zeiger auf den Signalverlauf. Die App zeigt die Spannung des Abtastwerts am Zeiger.

Um eine Zeit zu messen, doppelklicken Sie in einen leeren Bereich des Signalverlaufs. Klicken Sie auf den zweiten Punkt. Klicken Sie auf den dritten Punkt, um die Frequenz, die Periode und das Tastverhältnis zu sehen. Klicken Sie mit der rechten Maustaste, um abzubrechen.

## Spektrum (FFT)

1. Klicken Sie auf **Funktion** › **FFT**.
2. Wählen Sie **FFT aktivieren** aus.
3. Stellen Sie **FFT-Länge**, **Abtastintervall**, **FFT-Quelle** und **FFT-Fenster** ein.
4. Stellen Sie **Y-Achsen-Modus** und **dBV-Bereich** ein.
5. Klicken Sie auf **OK**.

Das Spektrum erscheint unter dem Signalverlauf. Drehen Sie das Mausrad im Spektrum, um die Frequenzskala zu zoomen. Ziehen Sie das Spektrum, um es zu bewegen. Setzen Sie den Zeiger auf das Spektrum, um die Frequenz und die Amplitude zu sehen.

## Mathematischer Kanal

1. Klicken Sie auf **Funktion** › **Mathe**.
2. Wählen Sie **Aktivieren** aus.
3. Wählen Sie die **Rechenart** aus: **Addieren**, **Subtrahieren**, **Multiplizieren** oder **Dividieren**.
4. Wählen Sie die **1. Quelle** und die **2. Quelle** aus.
5. Klicken Sie auf **OK**.

## Lissajous-Figur

1. Klicken Sie auf **Optionen** › **Anzeige** › **Lissajous**.
2. Wählen Sie **Aktivieren** aus.
3. Wählen Sie den Kanal für die **X-Achse** und die **Y-Achse** aus.
4. Klicken Sie auf **OK**.

## Datenerfassungsmodus

1. Wählen Sie in der Liste der Gerätemodi in der Symbolleiste **Datenerfassung** aus.
2. Öffnen Sie das Fenster **Geräteoptionen**.
3. Stellen Sie für jeden Kanal **Aktivieren**, **Kopplung** und **Volt/Div** ein.
4. Um eine andere Einheit zu zeigen, stellen Sie **Zuordnungseinheit**, **Zuordnung Min** und **Zuordnung Max** ein. Zeigen Sie zum Beispiel den Ausgang eines Temperatursensors in °C.
5. Klicken Sie auf **OK**.
6. Wählen Sie in der Symbolleiste die Abtastrate und die Abtastdauer aus.
7. Klicken Sie auf **Start** oder drücken Sie `S`.

Sie können die Kanaleinstellungen während der Erfassung nicht ändern. Bei der höchsten Abtastrate von 10 MHz ist die maximale Abtastdauer etwa 10 Sekunden. Bei 1 kHz kann die Erfassung einen Tag lang laufen.

Der Datenerfassungsmodus verwendet die Kalibrierung des Oszilloskop-Modus. Wenn ein Kanal einen Offset zeigt, kalibrieren Sie das Gerät im Oszilloskop-Modus.
