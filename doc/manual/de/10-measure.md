# Messungen

Sie können den Signalverlauf mit der Maus oder mit Cursorn messen. Ein Cursor ist eine senkrechte Linie bei einer Zeit in der Erfassung.

## Einen Impuls mit dem Zeiger messen

Setzen Sie den Zeiger auf einen Impuls eines Kanals. Ein Feld nahe dem Zeiger zeigt diese Werte für den Impuls:

- **Breite**: die Zeitdauer des Impulses.
- **Periode**: die Zeit von einer Flanke bis zur nächsten Flanke derselben Richtung.
- **Frequenz**: 1 geteilt durch die Periode.
- **Tastverhältnis**: die High-Zeit als Prozentsatz der Periode.

![Die Messung am Zeiger](../figures/hover-measurement.png)

Um dieses Feld ein- oder auszuschalten, öffnen Sie das Mess-Dock und verwenden Sie **Schwebende Messung aktivieren**.

## Die Flanken in einem Bereich zählen

1. Setzen Sie den Zeiger auf den Signalverlauf des Kanals, zwischen den High-Pegel und den Low-Pegel.
2. Bewegen Sie den Zeiger zum Anfang des Bereichs.
3. Klicken Sie mit der linken Maustaste.
4. Bewegen Sie den Zeiger zum Ende des Bereichs. Die App zeigt die Anzahl der Flanken, der steigenden Flanken und der fallenden Flanken.
5. Klicken Sie erneut mit der linken Maustaste, um die Messung abzuschließen.

## Die Zeit zwischen zwei Flanken messen

1. Setzen Sie den Zeiger auf die erste Flanke.
2. Klicken Sie mit der linken Maustaste.
3. Bewegen Sie den Zeiger zur zweiten Flanke. Die App zeigt die Zeit und die Anzahl der Abtastwerte zwischen den zwei Flanken.
4. Klicken Sie erneut mit der linken Maustaste, um die Messung abzuschließen.

![Die Zeit zwischen zwei Flanken](../figures/edge-distance.png)

## Einen Cursor hinzufügen

Verwenden Sie eine dieser Methoden:

- Doppelklicken Sie im Signalbereich mit der linken Maustaste bei der gewünschten Zeit. Wenn der Zeiger nahe an einer Flanke ist, springt der Cursor auf die Flanke.
- Klicken Sie im Zeitlineal mit der linken Maustaste. Ein Pfeil erscheint auf dem Lineal. Klicken Sie auf den Pfeil, um einen Cursor hinzuzufügen.

![Einen Cursor im Zeitlineal hinzufügen](../figures/ruler-insert-cursor.png)

Jeder Cursor hat eine Nummer. Die Nummern beginnen bei 1.

## Einen Cursor verschieben

Verwenden Sie eine dieser Methoden:

- Setzen Sie den Zeiger auf den Cursor. Die Cursorlinie wird dicker. Klicken Sie auf den Cursor. Bewegen Sie die Maus. Klicken Sie erneut, um den Cursor abzulegen. Nahe an einer Flanke springt der Cursor auf die Flanke.
- Klicken Sie im Zeitlineal mit der linken Maustaste bei der neuen Zeit. Das Lineal zeigt die Nummern aller Cursor. Klicken Sie auf die Nummer des Cursors, den Sie verschieben möchten.

![Einen Cursor im Zeitlineal verschieben](../figures/ruler-move-cursor.png)

## Zu einem Cursor gehen

1. Klicken Sie im Zeitlineal mit der rechten Maustaste. Das Lineal zeigt die Nummern aller Cursor.
2. Klicken Sie auf die Nummer eines Cursors. Der Signalverlauf bewegt sich zur Position dieses Cursors.

![Zu Cursor 3 gehen](../figures/ruler-jump-cursor.png)

## Mit Cursorn messen

Um das Mess-Dock zu öffnen, klicken Sie in der Symbolleiste auf **Messen** oder drücken Sie `M`. Das Dock hat diese Gruppen:

- **Cursorabstand**: die Zeit und die Anzahl der Abtastwerte zwischen zwei Cursorn.
- **Flanken**: die Anzahl der Flanken auf einem Kanal zwischen zwei Cursorn.
- **Cursor**: die Zeit und die Abtastwertnummer jedes Cursors.

Um eine Zeitmessung hinzuzufügen, führen Sie diese Schritte aus:

1. Klicken Sie in der Gruppe **Cursorabstand** auf die Schaltfläche **+**.
2. Klicken Sie auf das Startfeld und wählen Sie den ersten Cursor aus.
3. Klicken Sie auf das Endfeld und wählen Sie den zweiten Cursor aus.

Das Dock zeigt das Ergebnis in der Spalte **Zeit/Abtastwerte**.

Um eine Flankenzählung hinzuzufügen, führen Sie diese Schritte aus:

1. Klicken Sie in der Gruppe **Flanken** auf die Schaltfläche **+**.
2. Wählen Sie den Start-Cursor und den End-Cursor aus.
3. Wählen Sie den Kanal aus.

Das Dock zeigt die Anzahl der steigenden Flanken, der fallenden Flanken und aller Flanken.

Um eine Messung zu entfernen, klicken Sie auf die Schaltfläche **×** in ihrer Zeile.

## Einen Cursor löschen

Verwenden Sie eine dieser Methoden:

- Klicken Sie auf das **×** an der Cursor-Beschriftung im Zeitlineal.
- Klicken Sie auf die Schaltfläche **×** des Cursors in der Gruppe **Cursor** des Mess-Docks.

Die App gibt den verbleibenden Cursorn neue Nummern.
