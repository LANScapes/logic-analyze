# Opcje urządzenia

## Otwieranie opcji urządzenia

1. Kliknij na pasku narzędzi **Opcje** › **Opcje urządzenia...**. Możesz też nacisnąć `O`.
2. Zmień ustawienia w oknie **Opcje urządzenia**.
3. Kliknij **OK**.

Ustawienia w oknie są inne dla każdego modelu urządzenia. Ten rozdział podaje ustawienia DSLogic Plus.

> [!NOTE]
> Nie możesz zmienić opcji urządzenia podczas rejestracji.

![Okno Opcje urządzenia](../figures/device-options.png)
<!-- TODO: new screenshot -->

## Tryb pracy

Ustawienie **Tryb pracy** wybiera, jak urządzenie wysyła dane do komputera.

**Tryb buforowy.** Urządzenie przechowuje próbki w swojej pamięci wewnętrznej podczas rejestracji. Po rejestracji urządzenie wysyła dane do komputera przez USB. Pamięć jest szybsza niż USB. Dlatego tryb buforowy daje najwyższe częstotliwości próbkowania. Pojemność pamięci ogranicza długość rejestracji. Używaj trybu buforowego dla szybkich sygnałów i krótkich rejestracji.

**Tryb strumieniowy.** Urządzenie wysyła próbki do komputera podczas rejestracji. Pamięć komputera ogranicza długość rejestracji. Możesz widzieć dane podczas rejestracji. Prędkość połączenia USB ogranicza częstotliwość próbkowania. Używaj trybu strumieniowego dla wolnych sygnałów i długich rejestracji.

**Test wewnętrzny.** Ten tryb służy tylko do testów urządzenia. Nie używaj go do pomiarów.

## Opcje zatrzymania

Ustawienie **Opcje zatrzymania** dotyczy tylko trybu buforowego. Ustala, jak działa aplikacja, gdy zatrzymasz rejestrację przed końcem.

- **Zatrzymaj natychmiast**: Aplikacja nie pobiera danych z urządzenia. Aplikacja nie pokazuje danych.
- **Prześlij zebrane dane**: Aplikacja pobiera dane, które urządzenie zarejestrowało przed zatrzymaniem. Aplikacja pokazuje te dane.

## Poziom progowy

Ustawienie **Poziom progowy** to napięcie, które oddziela poziom niski od poziomu wysokiego. Sygnał powyżej progu to poziom wysoki. Sygnał poniżej progu to poziom niski.

Możesz ustawić wartość od 0,0 V do 5,0 V z krokiem 0,1 V. Ustaw próg na około 50% napięcia logiki obwodu. Dla obwodu 3,3 V ustaw około 1,6 V.

## Ustawienia filtra

Ustawienie **Ustawienia filtra** usuwa krótkie impulsy z danych.

- **Brak**: Aplikacja pokazuje wszystkie próbki.
- **1 takt próbkowania**: Aplikacja usuwa każdy impuls krótszy niż jeden okres próbkowania.

## Maksymalna wysokość

Ustawienie **Maks. wysokość** ustala maksymalną wysokość każdego wiersza kanału w obszarze przebiegów. **1X** to jedna jednostka wysokości. Używaj większej wartości, gdy pokazujesz tylko małą liczbę kanałów.

## Włączenie kompresji RLE

Gdy wybierzesz **Włącz kompresję RLE**, urządzenie kompresuje dane w swojej pamięci (kodowanie długości serii). To ustawienie dotyczy tylko trybu buforowego. Jeśli sygnały mają małą liczbę zboczy, urządzenie może przechować w pamięci dłuższą rejestrację. Jeśli sygnały mają dużo zboczy, kompresja nie zwiększa długości.

## Użycie zegara zewnętrznego

Gdy wybierzesz **Użyj zegara zewnętrznego**, urządzenie próbkuje kanały przy każdym zboczu zegara na przewodzie CK. Urządzenie nie używa swojego zegara wewnętrznego. Używaj tego ustawienia, aby zarejestrować magistralę, która ma sygnał zegara.

## Próbkowanie na opadającym zboczu zegara

To ustawienie działa tylko z ustawieniem **Użyj zegara zewnętrznego**. Zwykle urządzenie próbkuje kanały na zboczu narastającym zegara. Gdy wybierzesz **Próbkuj na opadającym zboczu zegara**, urządzenie próbkuje kanały na zboczu opadającym zegara.

## Tryb kanałów

Tryb kanałów ustala liczbę kanałów, których urządzenie może używać. Ustala też maksymalną częstotliwość próbkowania. Mniejsza liczba kanałów daje wyższą maksymalną częstotliwość próbkowania. Wybierz tryb kanałów, który pasuje do liczby i częstotliwości twoich sygnałów.

Dla DSLogic Plus tryby kanałów są takie:

| Tryb pracy | Tryb kanałów | Maksymalna częstotliwość próbkowania |
| --- | --- | --- |
| Tryb buforowy | Kanały od 0 do 15 | 100 MHz |
| Tryb buforowy | Kanały od 0 do 7 | 200 MHz |
| Tryb buforowy | Kanały od 0 do 3 | 400 MHz |
| Tryb strumieniowy | 16 kanałów | 20 MHz |
| Tryb strumieniowy | 12 kanałów | 25 MHz |
| Tryb strumieniowy | 6 kanałów | 50 MHz |
| Tryb strumieniowy | 3 kanały | 100 MHz |

## Włączanie i wyłączanie kanałów

Pod trybami kanałów okno pokazuje pole wyboru dla każdego kanału.

1. Zaznacz pole wyboru każdego kanału, którego używasz.
2. Wyczyść pole wyboru każdego kanału, którego nie używasz.
3. Aby zaznaczyć wszystkie kanały, kliknij **Wszystkie wł.**. Aby wyczyścić wszystkie kanały, kliknij **Wszystkie wył.**.

W trybie strumieniowym mniejsza liczba włączonych kanałów może pozwolić na wyższą częstotliwość próbkowania.
