# Daten erfassen

Bevor Sie eine Erfassung starten, stellen Sie diese Punkte ein:

1. Die Geräteoptionen. Siehe [Geräteoptionen](05-device-options.md).
2. Die Abtastrate und die Abtastdauer. Siehe [Abtastrate und Abtastdauer](06-sample-rate.md).
3. Den Trigger, wenn notwendig. Siehe [Trigger](07-trigger.md).
4. Den Erfassungsmodus. Siehe [Erfassungsmodi](#capture-modes).

## Eine Erfassung starten

Es gibt zwei Arten von Erfassung:

- **Start** startet eine Standard-Erfassung. Das Gerät wartet auf den Trigger, wenn Sie einen Trigger einstellen.
- **Sofort** startet eine Erfassung sofort. Das Gerät verwendet die Trigger-Einstellungen nicht.

Um eine Standard-Erfassung zu starten, klicken Sie auf **Start** oder drücken Sie `S`. Um eine Sofort-Erfassung zu starten, klicken Sie auf **Sofort** oder drücken Sie `I`. Während einer Erfassung wechselt die Schaltfläche zu **Stopp**. Klicken Sie auf **Stopp**, um die Erfassung zu stoppen.

### Ablauf einer Standard-Erfassung im Puffermodus

1. Sie klicken auf **Start**.
2. Die App sendet die Einstellungen an das Gerät.
3. Wenn es keinen Trigger gibt, beginnt das Gerät sofort mit der Aufzeichnung. Wenn es einen Trigger gibt, wartet das Gerät auf den Trigger.
4. Das Gerät zeichnet bis zum Ende der Abtastdauer auf oder bis sein Speicher voll ist.
5. Das Gerät sendet die Daten an den Computer.
6. Die App zeigt den Signalverlauf im Signalbereich.

### Ablauf einer Standard-Erfassung im Stream-Modus

1. Sie klicken auf **Start**.
2. Die App sendet die Einstellungen an das Gerät.
3. Wenn es einen Trigger gibt, wartet das Gerät auf den Trigger. Im Schleifenmodus verwendet das Gerät den Trigger nicht.
4. Das Gerät sendet die Daten während der Erfassung an den Computer.
5. Die App zeigt den Signalverlauf während der Erfassung.
6. Die Erfassung stoppt am Ende der Abtastdauer. Im Schleifenmodus läuft die Erfassung weiter, bis Sie auf **Stopp** klicken.

## Die Sofort-Erfassung verwenden

Die Sofort-Erfassung ist gleich wie die Standard-Erfassung, aber sie verwendet die Trigger-Einstellungen nicht. Verwenden Sie sie in diesen Fällen:

- Die Standard-Erfassung wartet lange, weil die Triggerbedingung nicht eintritt.
- Sie möchten die Signale zu diesem Zeitpunkt sehen.
- Sie möchten die Signale untersuchen, bevor Sie den Trigger ändern.

Wenn es kein Signal gibt, wartet eine Standard-Erfassung an der Triggerposition. Der Status zeigt **Warte auf Trigger!**. Eine Sofort-Erfassung zeichnet die Signale sofort auf.

## Erfassungsmodi {#capture-modes}

Um den Erfassungsmodus auszuwählen, klicken Sie in der Symbolleiste auf **Modus**. Dann wählen Sie einen dieser Einträge aus:

| Erfassungsmodus | Puffermodus | Stream-Modus |
| --- | --- | --- |
| **Einzeln** | Ja | Ja |
| **Wiederholt** | Ja | Ja |
| **Schleife** | Nein | Ja |

![Das Menü der Erfassungsmodi](../figures/capture-mode-menu.png)
<!-- TODO: new screenshot -->

### Einzeln

Das Gerät führt eine Erfassung aus. Dann stoppt die Erfassung.

Im Puffermodus zeigt die App den Signalverlauf nach der Erfassung. Im Stream-Modus zeigt die App den Signalverlauf während der Erfassung.

Verwenden Sie diesen Modus, um eine Signalbedingung oder den aktuellen Signalverlauf zu erfassen.

### Wiederholt

Das Gerät führt eine Erfassung aus. Dann startet es automatisch die nächste Erfassung. Dies geht weiter, bis Sie auf **Stopp** klicken.

Im Puffermodus zeigt die App ein Fenster für das Intervall zwischen den Erfassungen. Sie können einen Wert von 0,1 s bis 10 s einstellen.

Verwenden Sie diesen Modus, um eine Signalbedingung zu sehen, die oft eintritt. Verwenden Sie ihn zum Beispiel, um die Signale nach jedem Reset der Schaltung oder nach jedem Tastendruck zu sehen. Verwenden Sie ihn zusammen mit einem Trigger.

### Schleife

Dieser Modus ist nur im Stream-Modus verfügbar. Die Erfassung läuft weiter, bis Sie auf **Stopp** klicken. Wenn die Daten länger als die Abtastdauer sind, bewegen sich die ersten Daten links aus dem Fenster. Die neuesten Daten kommen rechts hinein. Die App verwirft die Daten, die sich hinausbewegen.

Verwenden Sie diesen Modus, wenn Sie den Zeitpunkt der Signalbedingung nicht kennen. Beobachten Sie den Signalverlauf während der Erfassung. Wenn Sie die Bedingung sehen, klicken Sie auf **Stopp**.

> [!NOTE]
> Im Schleifenmodus verwendet das Gerät die Trigger-Einstellungen nicht.

## Erfassungsstatus

Während einer Erfassung zeigt der Signalbereich den Status:

- **Warte auf Trigger!**: Das Gerät wartet auf die Triggerbedingung.
- **Getriggert!**: Der Trigger ist eingetreten.
- **% erfasst**: Der Prozentsatz der Erfassung, der vollständig ist.

Nach einer Erfassung zeigt der untere Rand des Signalbereichs die **Triggerzeit**.
