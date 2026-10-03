# Einleitung

## Über Logic Analyze

Logic Analyze ist eine macOS-App für Logikanalysatoren von DreamSourceLab. LANScapes liefert die App. Die App stammt von DSView, einem Programm von DreamSourceLab. DSView verwendet Software aus dem sigrok-Projekt.

Die App zeichnet digitale Signale von einem DSLogic-Logikanalysator auf. Dann zeigt sie die Signale als Signalverläufe. Sie können die Signalverläufe messen und serielle Protokolle dekodieren. Sie können die Daten auch speichern und in andere Formate exportieren.

Dieses Handbuch verwendet das DSLogic Plus für die Beispiele. Andere DSLogic-Modelle arbeiten mit denselben Verfahren. Ihre Grenzwerte für Kanäle, Speicher und Abtastrate sind anders.

Die App enthält auch das Werkzeug `dslcap`. Dieses Werkzeug erfasst Daten ohne das Hauptfenster. Siehe [Das Werkzeug dslcap](13-dslcap.md).

## Über dieses Handbuch

Dieses Handbuch verwendet ASD-STE100 Simplified Technical English als Grundlage. Jeder Satz ist kurz. Jeder Schritt eines Verfahrens gibt eine Anweisung. Jeder Fachbegriff in diesem Handbuch hat nur eine Bedeutung. [Fachbegriffe und Fachverben](15-terms.md) enthält die Liste der Fachbegriffe.

Dieses Handbuch verwendet diese Textformate:

- **Fetter Text** zeigt eine Beschriftung in der App, zum Beispiel eine Schaltfläche, einen Menüeintrag oder ein Feld.
- `Codetext` zeigt eine Taste auf der Tastatur, einen Befehl, einen Dateinamen oder einen Wert, den Sie eingeben.
- Ein Pfad durch Menüs verwendet das Zeichen ›, zum Beispiel **Datei** › **Sichern...**.
- Eine Liste von nummerierten Schritten ist ein Verfahren. Führen Sie die Schritte in der angegebenen Reihenfolge aus.

## Sicherheitshinweise

Dieses Handbuch verwendet diese Kennzeichnungen für Sicherheitshinweise:

- **WARNUNG** kennzeichnet ein Risiko von Verletzung oder Tod.
- **VORSICHT** kennzeichnet ein Risiko von Schäden an Geräten oder ein Risiko für Ihre Daten.
- **HINWEIS** gibt Informationen, die Ihnen helfen. Die Information nach **HINWEIS** gibt keine Anweisung.

Ein Sicherheitshinweis steht vor dem Schritt, für den er gilt. Lesen Sie alle Sicherheitshinweise, bevor Sie ein Verfahren beginnen.

> [!WARNING]
> Schließen Sie die Messleitungen nicht an Netzspannung an. Schließen Sie die Messleitungen nicht an einen Stromkreis an, der eine elektrische Verbindung zur Netzspannung hat. Die Messleitungen haben eine elektrische Verbindung zum Computer. Die Spannung kann Verletzungen oder den Tod verursachen.

> [!CAUTION]
> Legen Sie an einen Kanaleingang keine Spannung an, die höher als der Grenzwert in der Gerätespezifikation ist. Eine zu hohe Spannung kann den Logikanalysator beschädigen.

> [!CAUTION]
> Die Masseleitungen des Logikanalysators haben über das USB-Kabel eine Verbindung zur Masse Ihres Computers. Schließen Sie die Masseleitungen nur an die Masse der Schaltung an, die Sie messen. Wenn die zwei Massen unterschiedliche Spannungen haben, kann ein hoher Strom fließen. Dieser Strom kann die Schaltung, den Logikanalysator und den Computer beschädigen.

## Systemanforderungen

Diese Ausrüstung ist notwendig:

- Ein Mac mit macOS. Die Versionshinweise geben die minimale macOS-Version an.
- Ein USB-Anschluss. Ein USB-3.0-Anschluss gibt die höchste Geschwindigkeit. Ein USB-2.0-Anschluss funktioniert auch.
- Ein DSLogic-Logikanalysator, sein USB-Kabel und sein Messkabel.

Sie können die App ohne Logikanalysator verwenden. Das Gerät **Demo** erzeugt Testsignale. Sie können auch eine Datendatei von einer früheren Erfassung öffnen.
