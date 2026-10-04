# Die App installieren und starten

## Die App installieren

<!-- edition: download -->
1. Öffnen Sie die Release-Seite des Projekts: <https://github.com/LANScapes/logic-analyze/releases>.
2. Laden Sie die ZIP-Datei des neuesten Release herunter.
3. Öffnen Sie die ZIP-Datei im Finder. Der Finder entpackt **Logic Analyze.app**.
4. Ziehen Sie **Logic Analyze.app** in den Ordner **Programme**.
<!-- end edition -->
<!-- edition: appstore -->
1. Öffnen Sie den App Store auf dem Mac.
2. Suchen Sie Logic Analyze im App Store.
3. Installieren Sie die App. Der App Store legt **Logic Analyze.app** in den Ordner **Programme**.
<!-- end edition -->

Die App enthält alle notwendigen Bibliotheken, die Firmware und die Protokolldekoder. Sie installieren unter macOS keinen Treiber.

## Die App starten

1. Schließen Sie den Logikanalysator an den Computer an. Siehe [Das DSLogic Plus anschließen](04-connect.md).
2. Öffnen Sie **Logic Analyze** im Ordner **Programme** oder im Launchpad.
3. Wenn macOS eine Meldung zur App zeigt, klicken Sie auf **Öffnen**.

Beim ersten Start kann die App das Fenster **Dokument** zeigen. Klicken Sie auf **Öffnen**, um dieses Handbuch zu lesen. Klicken Sie auf **Ignorieren**, um das Fenster zu schließen. Klicken Sie auf **Nicht mehr anzeigen**, wenn Sie dieses Fenster nicht mehr sehen möchten.

## Die App aktualisieren

<!-- edition: download -->
1. Klicken Sie auf **Hilfe** › **Update**. Die App öffnet die Release-Seite in Ihrem Webbrowser.
2. Wenn die Release-Seite eine neuere Version zeigt, laden Sie diese herunter.
3. Beenden Sie Logic Analyze.
4. Ersetzen Sie **Logic Analyze.app** im Ordner **Programme** durch die neue Version.

Die App behält Ihre Einstellungen, wenn Sie sie ersetzen.
<!-- end edition -->
<!-- edition: appstore -->
Der App Store aktualisiert die App. So suchen Sie sofort nach einem Update:

1. Öffnen Sie den App Store auf dem Mac.
2. Öffnen Sie die Liste der Updates.
3. Wenn Logic Analyze in der Liste steht, aktualisieren Sie die App.

Die App behält Ihre Einstellungen, wenn sie aktualisiert wird.
<!-- end edition -->

## Dieses Handbuch öffnen

Klicken Sie auf **Hilfe** › **Handbuch...**. Die App öffnet dieses Handbuch in der Sprache der Benutzeroberfläche. Wenn das Handbuch in dieser Sprache nicht verfügbar ist, öffnet die App das englische Handbuch.

## Die Sprache ändern

1. Klicken Sie auf **Hilfe** › **Sprache**.
2. Wählen Sie eine Sprache aus der Liste aus.

Die Benutzeroberfläche wechselt in die Sprache, die Sie auswählen.

## Das Design ändern

1. Klicken Sie auf **Optionen** › **Anzeige** › **Themen**.
2. Wählen Sie **Dunkel** oder **Hell** aus.

## Die Symbolleiste verschieben

Sie können die Symbolleiste oben, unten, links oder rechts im Fenster anordnen. Wenn die Symbolleiste links oder rechts ist, können 16 Kanäle die volle Höhe des Fensters verwenden.

1. Setzen Sie den Zeiger auf den Griff am Anfang der Symbolleiste.
2. Halten Sie die Maustaste gedrückt.
3. Ziehen Sie die Symbolleiste an einen Rand des Fensters.
4. Lassen Sie die Maustaste los.

Die App behält die Position der Symbolleiste beim nächsten Start.

## Anzeigeoptionen

Um die Anzeigeoptionen zu ändern, klicken Sie auf **Optionen** › **Anzeige** › **Optionen**. Das Fenster **Anzeigeoptionen** zeigt diese Einstellungen:

| Einstellung | Funktion |
| --- | --- |
| **Signal durch Ziehen mit der Maus scrollen** | Wenn Sie den Signalverlauf schnell ziehen und loslassen, bewegt sich der Signalverlauf weiter. Dann wird er langsamer und hält an. |
| **Neueste Daten beim Stoppen im Wiederholmodus übernehmen** | Wenn Sie eine Erfassung im Modus **Wiederholt** stoppen, zeigt die App die Daten der letzten Erfassung, die nicht vollständig ist. |
| **Automatisch zu den neuesten Daten scrollen** | Im Stream-Modus bewegt sich der Signalverlauf, um die neuesten Daten zu zeigen. |
| **Triggerposition mittig anzeigen** | Im Oszilloskop-Modus zeigt die App die Triggerposition in der Mitte des Fensters. |
| **Sitzung in der Titelleiste anzeigen** | Die Titelleiste zeigt den Namen der Sitzungsdatei. |
| **Schriftgröße** | Die Größe des Textes im Signalbereich. |

## Log-Optionen

Die App kann eine Logdatei aufzeichnen. Die Logdatei hilft, die Ursache eines Problems zu finden.

1. Klicken Sie auf **Hilfe** › **Log-Optionen**.
2. Wählen Sie in **Log-Level** einen Wert von 0 bis 5 aus. Ein höherer Wert zeichnet mehr Meldungen auf.
3. Wählen Sie **In Datei speichern** aus.
4. Um neue Meldungen am Ende der vorhandenen Logdatei hinzuzufügen, wählen Sie **Anhängemodus** aus.
5. Klicken Sie auf **OK**.

Um die Logdatei anzusehen, klicken Sie im Fenster **Log-Optionen** auf **Öffnen**. Um die Logdatei zu löschen, klicken Sie auf **Leeren**.

## Ein Problem melden

Klicken Sie auf **Hilfe** › **Fehlerbericht**. Die App öffnet die Issue-Seite des Projekts in Ihrem Webbrowser. Geben Sie in Ihrem Bericht die App-Version, die macOS-Version und das Gerätemodell an. Fügen Sie wenn möglich die Logdatei hinzu.
