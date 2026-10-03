# Okno główne

## Części okna głównego

Okno główne ma te części:

- **Pasek narzędzi.** Pasek narzędzi ma elementy sterujące urządzeniem, rejestracją i narzędziami.
- **Obszar przebiegów.** Obszar przebiegów pokazuje jeden wiersz dla każdego kanału. Nad wierszami jest linijka czasu.
- **Etykiety kanałów.** Etykieta po lewej stronie każdego wiersza pokazuje numer kanału, nazwę i przyciski wyzwalania.
- **Panele boczne.** Panel boczny to panel z boku obszaru przebiegów. Narzędzia wyzwalania, dekoderów, pomiarów i wyszukiwania otwierają się w panelach bocznych.

![Okno główne w trybie analizatora logicznego](../figures/main-window.png)
<!-- TODO: new screenshot -->

## Pasek narzędzi

Pasek narzędzi ma te elementy, od początku do końca:

| Element | Funkcja |
| --- | --- |
| **Plik** | Menu do otwierania, zapisywania i eksportowania danych oraz do zapisywania sesji. Zobacz [Pliki i sesje](12-files.md). |
| Typ urządzenia | Etykieta, która pokazuje połączenie: **USB 2.0**, **USB 3.0**, **Demo** lub **Plik**. |
| Lista urządzeń | Urządzenie, którego używa aplikacja. Tutaj wybierz inne urządzenie lub urządzenie demonstracyjne. |
| Tryb urządzenia | **Analizator logiczny**, **Oscyloskop** lub **Akwizycja danych**. Lista pokazuje tylko tryby dostępne dla urządzenia. |
| Czas próbkowania | Czas trwania rejestracji. |
| Częstotliwość próbkowania | Liczba próbek na sekundę dla każdego kanału. |
| **Tryb** | Tryb rejestracji: **Pojedynczy**, **Powtarzany** lub **Pętla**. |
| **Start** | Uruchamia rejestrację. Podczas rejestracji ten przycisk zmienia się na **Stop**. |
| **Teraz** | Uruchamia rejestrację, która nie czeka na wyzwolenie. |
| **Wyzwalanie** | Otwiera panel boczny wyzwalania. |
| **Dekoduj** | Otwiera panel boczny dekoderów. |
| **Pomiar** | Otwiera panel boczny pomiarów. |
| **Szukaj** | Otwiera pasek wyszukiwania. |
| **Opcje** | Menu z pozycją **Opcje urządzenia...** i menu **Widok**. |
| **Pomoc** | Menu z językiem, tym podręcznikiem, stroną aktualizacji, opcjami logów i stroną zgłaszania problemów. |

Etykieta typu urządzenia pokazuje te wartości:

- **USB 3.0**: Urządzenie używa połączenia USB 3.0.
- **USB 2.0**: Urządzenie używa połączenia USB 2.0. Jeśli urządzenie ma połączenie USB 3.0, podłącz je do portu USB 3.0. Połączenie USB 2.0 zmniejsza maksymalną częstotliwość próbkowania w trybie strumieniowym.
- **Demo**: Urządzenie jest urządzeniem demonstracyjnym. Urządzenie demonstracyjne tworzy sygnały testowe. Używaj go, aby wypróbować funkcje aplikacji.
- **Plik**: Aplikacja pokazuje dane z pliku. Nie ma urządzenia.

## Skróty klawiszowe

| Klawisz | Funkcja |
| --- | --- |
| `S` | Uruchom lub zatrzymaj rejestrację. |
| `I` | Uruchom lub zatrzymaj rejestrację natychmiastową. W trybie oscyloskopu wykonaj jedną rejestrację i zatrzymaj. |
| `T` | Otwórz lub zamknij panel boczny wyzwalania. |
| `D` | Otwórz lub zamknij panel boczny dekoderów. |
| `M` | Otwórz lub zamknij panel boczny pomiarów. |
| `R` | Otwórz lub zamknij pasek wyszukiwania. |
| `O` | Otwórz okno **Opcje urządzenia**. |
| `Page Up` | Przesuń przebieg o jedną szerokość okna w lewo. |
| `Page Down` | Przesuń przebieg o jedną szerokość okna w prawo. |
| `←` | Powiększ. |
| `→` | Pomniejsz. |
| `0`, `1` | W trybie oscyloskopu wybierz lub zwolnij regulator skali kanału 0 lub kanału 1. |
| `↑`, `↓` | W trybie oscyloskopu zmień skalę pionową wybranego kanału. |

Skróty działają, gdy obszar przebiegów ma fokus klawiatury.
