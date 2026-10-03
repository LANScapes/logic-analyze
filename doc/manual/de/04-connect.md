# Das DSLogic Plus anschließen

## Das USB-Kabel anschließen

> [!NOTE]
> Verwenden Sie das USB-Kabel, das mit dem Gerät geliefert wurde, oder ein kurzes USB-Kabel von guter Qualität. Schließen Sie das Kabel direkt an einen Anschluss am Computer an. Ein USB-Hub oder ein langes Kabel kann Fehler in einer Erfassung verursachen.

1. Schließen Sie das USB-Kabel an das DSLogic Plus an.
2. Schließen Sie das andere Ende des USB-Kabels an einen USB-Anschluss des Computers an.
3. Stellen Sie sicher, dass die Anzeige am DSLogic Plus aufleuchtet. Bevor die App startet, leuchtet die Anzeige rot.
4. Starten Sie Logic Analyze.
5. Stellen Sie sicher, dass die Anzeige zu Grün wechselt.
6. Stellen Sie sicher, dass die Geräteliste in der Symbolleiste **DSLogic Plus** zeigt.

![Die USB-Verbindung](../figures/usb-connection.png)

Wenn die Geräteliste das Gerät nicht zeigt, führen Sie diese Schritte aus:

1. Trennen Sie das USB-Kabel vom Computer.
2. Warten Sie 5 Sekunden.
3. Schließen Sie das USB-Kabel an einen anderen USB-Anschluss an.
4. Wenn die Geräteliste das Gerät nach Schritt 3 nicht zeigt, beenden Sie die App und starten Sie sie erneut.

> [!NOTE]
> Nur ein Programm kann das Gerät zur gleichen Zeit verwenden. Wenn `dslcap` oder ein anderes Programm das Gerät verwendet, findet die App das Gerät nicht.

## Das Messkabel anschließen

Das Messkabel hat 16 Kanalleitungen. Jede Kanalleitung hat eine Abschirmung, ein Signalende und ein Masseende. Die Farben der Leitungen kennzeichnen die Kanäle 0 bis 15. Eine weitere Leitung hat diese Signale:

- **CK**: Der Eingang für einen externen Takt. Verwenden Sie ihn nur mit der Einstellung **Externen Takt verwenden**.
- **TI**: Der Eingang für ein externes Triggersignal.
- **TO**: Der Ausgang für das Triggersignal. Das Gerät sendet einen Impuls auf TO, wenn der Trigger eintritt.

Normalerweise schließen Sie die Leitungen CK, TI und TO nicht an.

![Das Messkabel und seine Kanäle](../figures/probe-cable-channels.png)

1. Schließen Sie das Messkabel an den Eingangsstecker des DSLogic Plus an.
2. Drücken Sie den Stecker vollständig in das Gerät.

## Die Kanäle an die Schaltung anschließen

> [!WARNING]
> Schließen Sie die Messleitungen nicht an Netzspannung an. Schließen Sie die Messleitungen nicht an einen Stromkreis an, der eine elektrische Verbindung zur Netzspannung hat. Die Spannung kann Verletzungen oder den Tod verursachen.

> [!CAUTION]
> Bevor Sie eine Masseleitung anschließen, stellen Sie sicher, dass die Masse der Schaltung und die Masse des Computers die gleiche Spannung haben. Ein Spannungsunterschied kann einen hohen Strom verursachen. Dieser Strom kann die Geräte beschädigen.

1. Trennen Sie die Stromversorgung von der Schaltung, die Sie messen.
2. Schließen Sie mindestens eine Masseleitung an die Masse der Schaltung an.
3. Schließen Sie jede Kanalleitung, die Sie verwenden, an ein Signal der Schaltung an.
4. Stellen Sie sicher, dass keine Messleitung einen anderen Kontakt berührt.
5. Schließen Sie die Stromversorgung an die Schaltung an.

![Masseverbindungen: eine gemeinsame Masse (links) oder eine Masse für jeden Kanal (rechts)](../figures/probe-grounding.png)

Für Signale mit einer Frequenz unter 5 MHz genügt eine Masseleitung für alle Kanäle. Für Signale mit einer höheren Frequenz schließen Sie das Masseende jeder Kanalleitung an die Masse nahe ihrem Signal an. Kurze Masseverbindungen geben saubere Signalflanken.

## Das DSLogic Plus trennen

> [!CAUTION]
> Trennen Sie das USB-Kabel nicht während einer Erfassung. Wenn Sie es trennen, können die Daten der Erfassung Fehler haben.

1. Stoppen Sie die Erfassung. Klicken Sie auf **Stopp**, wenn die Schaltfläche in der Symbolleiste erscheint.
2. Trennen Sie die Stromversorgung von der Schaltung.
3. Trennen Sie die Messleitungen von der Schaltung.
4. Trennen Sie das USB-Kabel.
