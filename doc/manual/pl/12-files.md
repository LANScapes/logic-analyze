# Pliki i sesje

Kliknij **Plik** na pasku narzędzi, aby otworzyć menu pliku. Menu ma te pozycje:

- **Sesja**: menu do wczytywania i zapisywania sesji.
- **Otwórz...**: otwiera plik danych.
- **Zapisz...**: zapisuje dane rejestracji.
- **Eksportuj...**: eksportuje dane do innego formatu.
- **Zrzut ekranu...**: zapisuje obraz okna.

## Sesje

Plik sesji zawiera ustawienia, ale nie zawiera danych rejestracji. Sesja zawiera opcje urządzenia, włączone kanały, nazwy i kolory kanałów oraz ustawienia wyzwalania. Plik sesji ma rozszerzenie `.dsc`.

### Zapisywanie sesji

1. Kliknij **Plik** › **Sesja** › **Zapisz sesję**.
2. Wybierz folder i wpisz nazwę pliku.
3. Kliknij **Zachowaj**.

### Wczytywanie sesji

1. Kliknij **Plik** › **Sesja** › **Wczytaj sesję**.
2. Wybierz plik sesji.
3. Kliknij **Otwórz**.

### Powrót do ustawień początkowych

Kliknij **Plik** › **Sesja** › **Wczytaj sesję domyślną**. Aplikacja ustawia wszystkie ustawienia urządzenia na wartości początkowe.

Aplikacja automatycznie zapisuje ustawienia, gdy ją zakończysz. Gdy ponownie uruchomisz aplikację, wczytuje ona ustawienia z ostatniej sesji.

## Zapisywanie danych

1. Kliknij **Plik** › **Zapisz...**.
2. Wybierz folder i wpisz nazwę pliku.
3. Kliknij **Zachowaj**.

Aplikacja zapisuje dane i ustawienia w pliku z rozszerzeniem `.dsl`. Możesz ponownie otworzyć ten plik w Logic Analyze.

> [!CAUTION]
> Aplikacja nie zapisuje danych automatycznie. Zapisz dane, zanim uruchomisz nową rejestrację lub zakończysz aplikację. Nowa rejestracja zastępuje dane poprzedniej rejestracji.

## Otwieranie pliku danych

1. Kliknij **Plik** › **Otwórz...**.
2. Wybierz plik z rozszerzeniem `.dsl`.
3. Kliknij **Otwórz**.

Aplikacja pokazuje dane w obszarze przebiegów. Etykieta typu urządzenia pokazuje **Plik**.

## Eksportowanie danych

Eksport tworzy plik, który inne programy mogą odczytać.

1. Kliknij **Plik** › **Eksportuj...**. Otwiera się okno **Eksportuj**.
2. Kliknij **ścieżka**.
3. Wybierz folder, wpisz nazwę pliku i wybierz format.
4. Kliknij **Zachowaj**.
5. Jeśli format to CSV, wybierz **Dane oryginalne** lub **Dane skompresowane**. Dane skompresowane zawierają wiersz tylko przy każdej zmianie wartości.
6. Kliknij **OK**.

W trybie analizatora logicznego są dostępne te formaty:

| Format | Rozszerzenie | Zastosowanie |
| --- | --- | --- |
| CSV | `.csv` | Arkusze kalkulacyjne i skrypty. |
| VCD | `.vcd` | Programy do przebiegów, na przykład GTKWave. |
| Gnuplot | `.gnuplot` | Program Gnuplot. |
| srzip | `.srzip` | Programy sigrok, na przykład PulseView. |

W trybie oscyloskopu i w trybie akwizycji danych jest dostępny tylko format CSV.

![Okno eksportu dla CSV](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Zapisywanie obrazu okna

1. Kliknij **Plik** › **Zrzut ekranu...**.
2. Wybierz folder i wpisz nazwę pliku.
3. Wybierz PNG lub JPEG.
4. Kliknij **Zachowaj**.
