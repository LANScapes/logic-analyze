# Das Werkzeug dslcap

Das Werkzeug `dslcap` erfasst Daten von einem DSLogic-Gerät ohne das Hauptfenster. Verwenden Sie es in Skripten und in automatischen Tests. Das Werkzeug schreibt die Abtastwerte in eine Binärdatei. Es schreibt ein JSON-Objekt mit dem Ergebnis in die Standardausgabe.

Das Werkzeug ist im App-Bundle:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Nur ein Programm kann das Gerät zur gleichen Zeit verwenden. Beenden Sie Logic Analyze, bevor Sie `dslcap` verwenden.

## Die Geräte auflisten

Um die Geräte aufzulisten, die die Bibliothek finden kann, geben Sie diesen Befehl ein:

```sh
dslcap --list
```

Um die USB-Kennung jedes angeschlossenen DSLogic-Geräts aufzulisten, geben Sie diesen Befehl ein:

```sh
dslcap --list-ids
```

Der Befehl `--list-ids` liest nur die Informationen, die macOS über USB-Geräte speichert. Er sendet keine Daten an das Gerät. Die Ausgabe gibt für jedes Gerät das Modell, den USB-Ort und eine Registry-Kennung an.

## Daten erfassen

Dieser Befehl erfasst 1000000 Abtastwerte auf den Kanälen 0 und 1 mit 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

Das Werkzeug schreibt die Abtastwerte in `/tmp/capture.bin`. Wenn es eine Datei mit diesem Namen gibt, stoppt das Werkzeug mit einem Fehler. Das Werkzeug ersetzt keine Datei.

Dies sind die Erfassungsoptionen:

| Option | Funktion | Anfangswert |
| --- | --- | --- |
| `--channels LIST` | Die Kanäle zum Aufzeichnen, zum Beispiel `0,1,2`. | `0` |
| `--samplerate HZ` | Die Abtastrate in Hz. | `10000000` |
| `--samples N` | Die Anzahl der Abtastwerte für jeden Kanal. | `1000000` |
| `--vth VOLTS` | Die Schwellenspannung. | `1.6` |
| `--mode MODE` | `buffer` oder `stream`. | `buffer` |
| `--trigger CH[:T]` | Ein Trigger auf Kanal CH. Verwenden Sie für T `R` (steigende Flanke), `F` (fallende Flanke), `C` (steigende oder fallende Flanke), `1` (High-Pegel) oder `0` (Low-Pegel). | Kein Trigger. `R`, wenn Sie nur CH angeben. |
| `--trigpos PERCENT` | Die Triggerposition als Prozentsatz der Abtastwerte. | `10` |
| `--timeout SEC` | Die maximale Zeit der Erfassung in Sekunden. | `30` |
| `--out PATH` | Der Pfad der Ausgabedatei ohne die Endung `.bin`. | Diese Option ist notwendig. |
| `--log-level N` | Die Menge der Bibliotheksmeldungen in der Standardfehlerausgabe, von 0 (keine) bis 5 (alle). | `1` |

Das Werkzeug prüft alle Optionen, bevor es das Gerät verwendet. Wenn eine Option nicht korrekt ist, stoppt das Werkzeug und gibt einen Fehler aus.

## Die Ausgabedatei

Die Datei `.bin` enthält die Kanäle in der Reihenfolge ihrer Nummern, beginnend mit der niedrigsten Nummer. Für jeden Kanal enthält die Datei alle Abtastwerte dieses Kanals. Jedes Byte enthält 8 Abtastwerte. Der erste Abtastwert ist das niedrigstwertige Bit. Die Daten jedes Kanals füllen eine ganze Anzahl von 8-Byte-Einheiten. Darum verwendet jeder Kanal `ceil(samples / 64) × 8` Bytes.

## Das JSON-Ergebnis

Das Werkzeug schreibt ein JSON-Objekt in eine Zeile. Nach einer korrekten Erfassung gibt das Objekt den Gerätenamen, die Abtastrate, die Anzahl der Abtastwerte und die Kanäle an. Es gibt auch die Schwellenspannung, den Modus, den Trigger, die Zeit der Erfassung und den Pfad der Datei `.bin` an. Wenn die Erfassung nicht korrekt ist, enthält das Objekt einen Schlüssel `error`. Dann erstellt das Werkzeug keine Datei `.bin`.

Verwenden Sie das Ergebnis nur, wenn der Exit-Status 0 ist und das JSON-Objekt vollständig ist.

## Exit-Status

| Status | Bedeutung |
| --- | --- |
| 0 | Die Erfassung ist vollständig. |
| 1 | Während des Vorgangs ist ein Fehler aufgetreten, zum Beispiel ein E/A-Fehler. |
| 2 | Eine Option ist nicht korrekt, oder eine Einstellung ist auf dem Gerät nicht verfügbar. |
| 3 | Die Erfassung ist nicht vollständig. |

## Optionen für Programme, die dslcap starten

- `--parent-fd N`: Das Werkzeug stoppt, wenn das Programm, das es gestartet hat, die Pipe mit dem Deskriptor N schließt. Dann entfernt das Werkzeug seine Ausgabedatei, wenn die Erfassung nicht vollständig ist.
- `--res DIR`: Der Ordner mit den Firmware-Dateien. Normalerweise findet das Werkzeug diesen Ordner automatisch. Sie können auch die Umgebungsvariable `DSLCAP_RES` setzen.
- `--res-manifest FD`: Das Werkzeug prüft den SHA-256-Wert jeder Firmware-Datei, bevor es die Datei an das Gerät sendet.

Die Datei `tools/dslcap/README.md` im Quellcode gibt alle Informationen zu diesen Optionen.
