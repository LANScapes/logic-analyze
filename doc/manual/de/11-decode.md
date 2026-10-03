# Protokolldekoder

Ein Protokolldekoder liest die Daten einer Erfassung und findet die Frames eines Protokolls, zum Beispiel UART, I2C oder SPI. Die App zeigt das Ergebnis als neue Zeile über den Kanälen. Die App hat mehr als 100 Dekoder.

Um das Dekoder-Dock zu öffnen, klicken Sie in der Symbolleiste auf **Dekodieren** oder drücken Sie `D`. Das Dock hat zwei Teile:

- Die Dekoderliste mit dem Feld **Dekoder suchen...** oben.
- Die Liste **Dekodierergebnisse**. Diese Liste zeigt jedes Element des Dekoders als Textzeile.

![Das Dekoder-Dock](../figures/decoder-dock.png)
<!-- TODO: new screenshot -->

## Einen Dekoder hinzufügen

> [!NOTE]
> Ein Dekoder mit dem Präfix `0:` ist eine kleinere Version. Er zeigt die Bits nicht. Sie können kein höheres Protokoll auf ihm hinzufügen. Er dekodiert schneller und verwendet weniger Speicher.

1. Klicken Sie auf das Feld **Dekoder suchen...**. Die Liste der Dekoder öffnet sich.
2. Geben Sie einen Teil des Protokollnamens ein, zum Beispiel `I2C`. Die Liste zeigt nur die Dekoder, die mit dem Text übereinstimmen.
3. Klicken Sie auf den Dekoder. Das Fenster **Dekoderoptionen** öffnet sich.
4. Stellen Sie die Kanäle des Protokolls ein. Stellen Sie zum Beispiel **SCL** und **SDA** für I2C ein.
5. Stellen Sie die Protokolloptionen ein, zum Beispiel die Baudrate eines UART.
6. Wählen Sie die Ergebniszeilen aus, die die App zeigt.
7. Wenn notwendig, stellen Sie den Dekodierbereich ein. Siehe [Einen Teil der Erfassung dekodieren](#decode-region).
8. Klicken Sie auf **OK**.

Die App dekodiert die Daten und zeigt die Ergebnisse in einer neuen Zeile im Signalbereich.

Um weitere Dekoder hinzuzufügen, führen Sie das Verfahren für jeden Dekoder erneut aus.

![Die Dekoder-Schaltflächen: Die Einstellungsschaltfläche öffnet die Dekoderoptionen](../figures/decoder-buttons.png)

Um die Einstellungen eines Dekoders zu ändern, klicken Sie im Dock auf die Einstellungsschaltfläche dieses Dekoders.

## Einen gestapelten Dekoder hinzufügen

Manche Protokolle verwenden ein niedrigeres Protokoll. Zum Beispiel verwendet das Protokoll 24xx EEPROM das Protokoll I2C. Wenn Sie das höhere Protokoll hinzufügen, fügt die App auch die niedrigeren Protokolle hinzu.

1. Geben Sie im Feld **Dekoder suchen...** den Namen des höheren Protokolls ein, zum Beispiel `24xx`.
2. Klicken Sie auf den Dekoder.
3. Stellen Sie im Fenster **Dekoderoptionen** die Optionen für jede Protokollschicht ein.
4. Klicken Sie auf **OK**.

Die Ergebnisse zeigen die Frames des niedrigeren Protokolls und die Befehle und Daten des höheren Protokolls.

## Einen Teil der Erfassung dekodieren {#decode-region}

Normalerweise dekodiert die App alle Daten. Um nur einen Teil zu dekodieren, stellen Sie einen Start-Cursor und einen End-Cursor ein. Zum Beispiel können Sie das Rauschen bei einem Reset der Schaltung ignorieren. Ein kürzerer Bereich verringert auch die Dekodierzeit.

1. Fügen Sie zwei Cursor am Anfang und am Ende des Bereichs hinzu. Siehe [Messungen](10-measure.md).
2. Öffnen Sie das Fenster **Dekoderoptionen** des Dekoders.
3. Wählen Sie in der Liste **Start** den Start-Cursor aus.
4. Wählen Sie in der Liste **Ende** den End-Cursor aus.
5. Klicken Sie auf **OK**.

## Die Ergebnisliste lesen

Die Liste **Dekodierergebnisse** zeigt die Elemente des Dekoders in zeitlicher Reihenfolge. Klicken Sie auf eine Zeile, um den Signalverlauf zu diesem Element zu bewegen.

Um die Spalten zu ändern, die die Liste zeigt, klicken Sie oben in der Liste auf die Einstellungsschaltfläche.

## Einen Text in den Ergebnissen finden

1. Geben Sie einen Text in das Suchfeld der Liste **Dekodierergebnisse** ein.
2. Klicken Sie auf den rechten Pfeil, um zur nächsten Zeile mit dem Text zu gehen. Klicken Sie auf den linken Pfeil, um zur vorherigen Zeile zu gehen.

Der Signalverlauf bewegt sich zum Element jeder Zeile, die die Suche findet. Wenn Sie zuerst auf eine Zeile klicken, beginnt die Suche bei dieser Zeile.

![Suche in den Dekodierergebnissen](../figures/decoder-list-search.png)

Um eine Folge von Bytes zu finden, setzen Sie das Zeichen `-` zwischen die Bytes. Zum Beispiel findet `70-70-70` drei aufeinanderfolgende Bytes mit dem Wert 70.

![Suche nach einer Folge von Bytes](../figures/decoder-multibyte-search.png)

> [!NOTE]
> Die Suche nach einer Folge von Bytes funktioniert nur mit den Dekodern UART, I2C und SPI.

## Die Ergebnisse exportieren

1. Klicken Sie oben in der Liste **Dekodierergebnisse** auf die Speichern-Schaltfläche. Das Fenster **Protokollexport** öffnet sich.
2. Wählen Sie in **Exportformat** CSV oder TXT aus.
3. Wählen Sie jede Spalte aus, die Sie exportieren möchten. Die App schreibt alle Spalten in zeitlicher Reihenfolge in eine Datei.
4. Klicken Sie auf **OK**.
5. Wählen Sie den Ordner aus und geben Sie den Dateinamen ein.
6. Klicken Sie auf **Sichern**.

## Einen Dekoder löschen

![Einen Dekoder oder alle Dekoder löschen](../figures/decoder-delete.png)

- Um einen Dekoder zu löschen, klicken Sie auf die Schaltfläche **×** in der Zeile dieses Dekoders.
- Um alle Dekoder zu löschen, klicken Sie auf die Schaltfläche **×** oben im Dock, neben der Schaltfläche **+**.
