# Dateien und Sitzungen

Klicken Sie in der Symbolleiste auf **Datei**, um das Dateimenü zu öffnen. Das Menü hat diese Einträge:

- **Sitzung**: ein Menü, um Sitzungen zu laden und zu speichern.
- **Öffnen...**: eine Datendatei öffnen.
- **Sichern...**: die Daten der Erfassung speichern.
- **Export...**: die Daten in ein anderes Format exportieren.
- **Screenshot...**: ein Bild des Fensters speichern.

## Sitzungen

Eine Sitzungsdatei enthält die Einstellungen, aber nicht die Daten der Erfassung. Eine Sitzung enthält die Geräteoptionen, die aktivierten Kanäle, die Kanalnamen und -farben und die Trigger-Einstellungen. Eine Sitzungsdatei hat die Endung `.dsc`.

### Eine Sitzung speichern

1. Klicken Sie auf **Datei** › **Sitzung** › **Sitzung sichern**.
2. Wählen Sie den Ordner aus und geben Sie den Dateinamen ein.
3. Klicken Sie auf **Sichern**.

### Eine Sitzung laden

1. Klicken Sie auf **Datei** › **Sitzung** › **Sitzung laden**.
2. Wählen Sie die Sitzungsdatei aus.
3. Klicken Sie auf **Öffnen**.

### Zu den Anfangseinstellungen zurückgehen

Klicken Sie auf **Datei** › **Sitzung** › **Standardsitzung laden**. Die App setzt alle Einstellungen des Geräts auf ihre Anfangswerte.

Die App speichert die Einstellungen automatisch, wenn Sie sie beenden. Wenn Sie die App erneut starten, lädt sie die Einstellungen der letzten Sitzung.

## Die Daten speichern

1. Klicken Sie auf **Datei** › **Sichern...**.
2. Wählen Sie den Ordner aus und geben Sie den Dateinamen ein.
3. Klicken Sie auf **Sichern**.

Die App speichert die Daten und die Einstellungen in einer Datei mit der Endung `.dsl`. Sie können diese Datei erneut in Logic Analyze öffnen.

> [!CAUTION]
> Die App speichert die Daten nicht automatisch. Speichern Sie die Daten, bevor Sie eine neue Erfassung starten oder die App beenden. Eine neue Erfassung ersetzt die Daten der vorherigen Erfassung.

## Eine Datendatei öffnen

1. Klicken Sie auf **Datei** › **Öffnen...**.
2. Wählen Sie eine Datei mit der Endung `.dsl` aus.
3. Klicken Sie auf **Öffnen**.

Die App zeigt die Daten im Signalbereich. Die Beschriftung des Gerätetyps zeigt **Datei**.

## Die Daten exportieren

Der Export erstellt eine Datei, die andere Programme lesen können.

1. Klicken Sie auf **Datei** › **Export...**. Das Fenster **Exportieren** öffnet sich.
2. Klicken Sie auf **Pfad**.
3. Wählen Sie den Ordner aus, geben Sie den Dateinamen ein und wählen Sie das Format aus.
4. Klicken Sie auf **Sichern**.
5. Wenn das Format CSV ist, wählen Sie **Originaldaten** oder **Komprimierte Daten** aus. Komprimierte Daten enthalten nur bei jeder Wertänderung eine Zeile.
6. Klicken Sie auf **OK**.

Im Logikanalysator-Modus sind diese Formate verfügbar:

| Format | Endung | Verwendung |
| --- | --- | --- |
| CSV | `.csv` | Tabellenkalkulationen und Skripte. |
| VCD | `.vcd` | Programme für Signalverläufe, zum Beispiel GTKWave. |
| Gnuplot | `.gnuplot` | Das Programm Gnuplot. |
| srzip | `.srzip` | sigrok-Programme, zum Beispiel PulseView. |

Im Oszilloskop-Modus und im Datenerfassungsmodus ist nur CSV verfügbar.

![Das Export-Fenster für CSV](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Ein Bild des Fensters speichern

1. Klicken Sie auf **Datei** › **Screenshot...**.
2. Wählen Sie den Ordner aus und geben Sie den Dateinamen ein.
3. Wählen Sie PNG oder JPEG aus.
4. Klicken Sie auf **Sichern**.
