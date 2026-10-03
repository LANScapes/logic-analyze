# Narzędzie dslcap

Narzędzie `dslcap` rejestruje dane z urządzenia DSLogic bez okna głównego. Używaj go w skryptach i w testach automatycznych. Narzędzie zapisuje próbki do pliku binarnego. Zapisuje jeden obiekt JSON z wynikiem na standardowe wyjście.

Narzędzie jest w pakiecie aplikacji:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Tylko jeden program jednocześnie może używać urządzenia. Zakończ Logic Analyze, zanim użyjesz `dslcap`.

## Wyświetlanie listy urządzeń

Aby wyświetlić listę urządzeń, które biblioteka może znaleźć, wpisz to polecenie:

```sh
dslcap --list
```

Aby wyświetlić identyfikator USB każdego podłączonego urządzenia DSLogic, wpisz to polecenie:

```sh
dslcap --list-ids
```

Polecenie `--list-ids` odczytuje tylko informacje, które macOS przechowuje o urządzeniach USB. Nie wysyła danych do urządzenia. Wynik podaje dla każdego urządzenia model, lokalizację USB i identyfikator rejestru.

## Rejestracja danych

To polecenie rejestruje 1000000 próbek na kanałach 0 i 1 przy 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

Narzędzie zapisuje próbki do pliku `/tmp/capture.bin`. Jeśli istnieje plik o tej nazwie, narzędzie zatrzymuje się z błędem. Narzędzie nie zastępuje pliku.

Oto opcje rejestracji:

| Opcja | Funkcja | Wartość początkowa |
| --- | --- | --- |
| `--channels LIST` | Kanały do zarejestrowania, na przykład `0,1,2`. | `0` |
| `--samplerate HZ` | Częstotliwość próbkowania w Hz. | `10000000` |
| `--samples N` | Liczba próbek dla każdego kanału. | `1000000` |
| `--vth VOLTS` | Napięcie progowe. | `1.6` |
| `--mode MODE` | `buffer` lub `stream`. | `buffer` |
| `--trigger CH[:T]` | Wyzwalanie na kanale CH. Dla T użyj `R` (zbocze narastające), `F` (zbocze opadające), `C` (zbocze narastające lub opadające), `1` (poziom wysoki) lub `0` (poziom niski). | Brak wyzwalania. `R`, jeśli podasz tylko CH. |
| `--trigpos PERCENT` | Pozycja wyzwalania jako procent próbek. | `10` |
| `--timeout SEC` | Maksymalny czas rejestracji w sekundach. | `30` |
| `--out PATH` | Ścieżka pliku wyjściowego bez rozszerzenia `.bin`. | Ta opcja jest wymagana. |
| `--log-level N` | Ilość komunikatów biblioteki na standardowym wyjściu błędów, od 0 (brak) do 5 (wszystkie). | `1` |

Narzędzie sprawdza wszystkie opcje, zanim użyje urządzenia. Jeśli opcja jest niepoprawna, narzędzie zatrzymuje się i podaje błąd.

## Plik wyjściowy

Plik `.bin` zawiera kanały w kolejności ich numerów, od najniższego numeru. Dla każdego kanału plik zawiera wszystkie próbki tego kanału. Każdy bajt zawiera 8 próbek. Pierwsza próbka to najmniej znaczący bit. Dane każdego kanału wypełniają pełną liczbę jednostek 8-bajtowych. Dlatego każdy kanał używa `ceil(samples / 64) × 8` bajtów.

## Wynik JSON

Narzędzie zapisuje jeden obiekt JSON w jednym wierszu. Po poprawnej rejestracji obiekt podaje nazwę urządzenia, częstotliwość próbkowania, liczbę próbek i kanały. Podaje też napięcie progowe, tryb, wyzwalanie, czas rejestracji i ścieżkę pliku `.bin`. Jeśli rejestracja jest niepoprawna, obiekt zawiera klucz `error`. Wtedy narzędzie nie tworzy pliku `.bin`.

Używaj wyniku tylko wtedy, gdy status wyjścia wynosi 0, a obiekt JSON jest kompletny.

## Status wyjścia

| Status | Znaczenie |
| --- | --- |
| 0 | Rejestracja jest zakończona. |
| 1 | Podczas operacji wystąpił błąd, na przykład błąd wejścia/wyjścia. |
| 2 | Opcja jest niepoprawna lub ustawienie jest niedostępne w urządzeniu. |
| 3 | Rejestracja nie została zakończona. |

## Opcje dla programów, które uruchamiają dslcap

- `--parent-fd N`: Narzędzie zatrzymuje się, gdy program, który je uruchomił, zamyka potok z deskryptorem N. Wtedy narzędzie usuwa swój plik wyjściowy, jeśli rejestracja nie jest zakończona.
- `--res DIR`: Folder z plikami oprogramowania układowego. Zwykle narzędzie znajduje ten folder automatycznie. Możesz też ustawić zmienną środowiskową `DSLCAP_RES`.
- `--res-manifest FD`: Narzędzie sprawdza wartość SHA-256 każdego pliku oprogramowania układowego, zanim wyśle plik do urządzenia.

Plik `tools/dslcap/README.md` w kodzie źródłowym podaje wszystkie informacje o tych opcjach.
