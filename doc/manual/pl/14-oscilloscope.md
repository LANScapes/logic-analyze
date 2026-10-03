# Tryb oscyloskopu i tryb akwizycji danych

Logic Analyze może też obsługiwać oscyloskopy DSCope firmy DreamSourceLab. DSCope ma dwa tryby urządzenia:

- **Oscyloskop**: dla sygnałów o stałym okresie i dla jednego warunku sygnału.
- **Akwizycja danych**: dla wolnych sygnałów przez długi czas, na przykład napięcia zasilania lub wyjścia czujnika.

Te tryby nie są dostępne w urządzeniach DSLogic. Ten rozdział podaje tylko główne procedury.

## Podłączenie DSCope

> [!WARNING]
> Nie podłączaj sond do napięcia sieciowego. Nie podłączaj sond do obwodu, który ma połączenie elektryczne z napięciem sieciowym. Napięcie może spowodować obrażenia lub śmierć.

> [!CAUTION]
> Masa sond, masa DSCope i masa komputera mają połączenie ze sobą. Podłączaj masę sondy tylko do punktu, który ma to samo napięcie co masa komputera. Różnica napięć może uszkodzić sprzęt.

1. Podłącz DSCope do komputera kablem USB.
2. Uruchom Logic Analyze. Upewnij się, że lista urządzeń pokazuje DSCope.
3. Podłącz sondy do wejść DSCope.
4. Ustaw przełącznik tłumienia na każdej sondzie.
5. Podłącz zacisk masy każdej sondy do masy obwodu.
6. Podłącz końcówkę sondy do sygnału.

## Opcje urządzenia

Kliknij **Opcje** › **Opcje urządzenia...** lub naciśnij `O`.

- **Tryb pracy**: **Normalny** do pomiarów. **Test wewnętrzny** służy tylko do testów urządzenia.
- **Ograniczenie pasma**: **Pełne pasmo** lub **20MHz**. Ograniczenie do 20 MHz zmniejsza szum o wysokiej częstotliwości.

## Kalibracja DSCope

Wzmocnienie i przesunięcie wejść zmieniają się z temperaturą i wilgotnością. Kalibruj DSCope, aby pomiary pozostały dokładne.

### Kalibracja automatyczna

> [!CAUTION]
> Przed kalibracją odłącz wszystkie sondy od wejść. Sygnał na wejściu podczas kalibracji daje niepoprawne wartości kalibracji.

1. Otwórz okno **Opcje urządzenia**.
2. Kliknij **Automatyczna kalibracja**.
3. Odłącz wszystkie sondy. Kliknij **OK**. Kalibracja trwa kilka minut.
4. Gdy kalibracja się zakończy, kliknij **Zapisz**, aby zachować wynik.

Aby zatrzymać kalibrację, kliknij **Przerwij**. Urządzenie używa wtedy poprzednich wartości kalibracji.

### Kalibracja ręczna

1. Otwórz okno **Opcje urządzenia**.
2. Kliknij **Kalibracja ręczna**.
3. Kliknij **Start** na pasku narzędzi.
4. Aby ustawić przesunięcie, podłącz sondę do masy. Aby ustawić wzmocnienie, podłącz sondę do sygnału o znanym napięciu.
5. Ustaw skalę pionową, którą chcesz skalibrować.
6. Przesuwaj suwak **VOFF** lub **VGAIN** kanału, aż przebieg będzie poprawny.
7. Wykonaj ponownie kroki 5 i 6 dla każdej skali pionowej.
8. Kliknij **Zapisz**.

Aby odrzucić zmiany, kliknij **Przerwij**. Aby używać zmian tylko do odłączenia urządzenia, kliknij **Zakończ**. Aby wrócić do wartości początkowych, kliknij **Resetuj**. Po resecie wykonaj ponownie kalibrację automatyczną.

## Ustawienia kanału

Każdy kanał ma te elementy sterujące po lewej stronie obszaru przebiegów:

- **Włącz**: włącza lub wyłącza kanał.
- **Skala pionowa**: napięcie na działkę. Okno ma 10 działek. Aby zmienić skalę, obróć kółko myszy na pokrętle lub kliknij górną lub dolną część pokrętła. Możesz też nacisnąć `0` lub `1`, aby wybrać pokrętło kanału, a potem nacisnąć `↑` lub `↓`.
- **Sprzężenie**: **DC** lub **AC**.
- **Tłumienie sondy**: ustaw **x1** lub **x10** zgodnie z przełącznikiem na sondzie.
- **AUTO**: ustawia skalę pionową, skalę poziomą i poziom wyzwalania dla sygnału na wejściu.

Aby przesunąć przebieg kanału w górę lub w dół, przeciągnij etykietę kanału.

## Skala pozioma

Wybierz czas na działkę z listy na pasku narzędzi. Możesz też obrócić kółko myszy w obszarze przebiegów.

## Start i stop

- Kliknij **Start** lub naciśnij `S`, aby uruchomić ciągłą rejestrację. Kliknij **Stop**, aby ją zatrzymać.
- Kliknij **Pojedynczy** lub naciśnij `I`, aby zarejestrować jeden przebieg i zatrzymać.

## Wyzwalanie

Kliknij **Wyzwalanie** lub naciśnij `T`, aby otworzyć panel boczny wyzwalania. Panel boczny ma te ustawienia:

- **Pozycja wyzwalania**: pozycja punktu wyzwolenia w rejestracji jako procent.
- **Czas blokady**: czas po wyzwoleniu, w którym urządzenie ignoruje nowe wyzwolenia. Używaj go, aby uzyskać stabilny przebieg z grup impulsów.
- **Czułość wyzwalania**: zmiana napięcia potrzebna do wyzwolenia. Większa wartość ignoruje więcej szumu.
- **Źródła wyzwalania**: **Auto**, **Kanał 0**, **Kanał 1**, **Kanał 0 && 1** lub **Kanał 0 | 1**.
- **Typy wyzwalania**: **Zbocze narastające** lub **Zbocze opadające**.

Aby ustawić poziom wyzwalania, kliknij etykietę poziomu wyzwalania kanału. Przesuń mysz. Kliknij ponownie, aby ustalić poziom.

## Pomiary

### Pomiary automatyczne

Dół obszaru przebiegów ma 10 pól dla pomiarów automatycznych.

1. Kliknij pole pomiaru.
2. Wybierz kanał.
3. Wybierz pomiar. Aby wyczyścić pole, kliknij **Resetuj**.

Aplikacja zachowuje te ustawienia na następne uruchomienie.

### Kursory

- Aby dodać kursor czasu, kliknij linijkę czasu. Możesz też kliknąć prawym przyciskiem myszy w obszarze przebiegów i wybrać **Dodaj kursor Y**.
- Aby dodać kursor napięcia, kliknij prawym przyciskiem myszy w obszarze przebiegów i wybierz **Dodaj kursor X**. Każdy kursor napięcia ma dwie poziome linie. Etykieta między liniami pokazuje różnicę napięć.
- Aby zmierzyć czas między dwoma kursorami, użyj grupy **Odległość kursorów** w panelu bocznym pomiarów.

### Pomiar wskaźnikiem

Po zatrzymaniu rejestracji ustaw wskaźnik na przebiegu. Aplikacja pokazuje napięcie próbki przy wskaźniku.

Aby zmierzyć czas, kliknij dwukrotnie w pustym obszarze przebiegu. Kliknij drugi punkt. Kliknij trzeci punkt, aby zobaczyć częstotliwość, okres i wypełnienie. Kliknij prawym przyciskiem myszy, aby anulować.

## Widmo (FFT)

1. Kliknij **Funkcja** › **FFT**.
2. Zaznacz **Włącz FFT**.
3. Ustaw **Długość FFT**, **Interwał próbek**, **Źródło FFT** i **Okno FFT**.
4. Ustaw **Tryb osi Y** i **Zakres DBV**.
5. Kliknij **OK**.

Widmo pojawia się pod przebiegiem. Obróć kółko myszy w widmie, aby powiększyć skalę częstotliwości. Przeciągnij widmo, aby je przesunąć. Ustaw wskaźnik na widmie, aby zobaczyć częstotliwość i amplitudę.

## Kanał matematyczny

1. Kliknij **Funkcja** › **Matem.**.
2. Zaznacz **Włącz**.
3. Wybierz **Rodzaj działania**: **Dodawanie**, **Odejmowanie**, **Mnożenie** lub **Dzielenie**.
4. Wybierz **1. źródło** i **2. źródło**.
5. Kliknij **OK**.

## Krzywa Lissajous

1. Kliknij **Opcje** › **Widok** › **Lissajous**.
2. Zaznacz **Włącz**.
3. Wybierz kanał dla **Oś X** i dla **Oś Y**.
4. Kliknij **OK**.

## Tryb akwizycji danych

1. Na liście trybów urządzenia na pasku narzędzi wybierz **Akwizycja danych**.
2. Otwórz okno **Opcje urządzenia**.
3. Dla każdego kanału ustaw **Włącz**, **Sprzężenie** i **V/dz.**.
4. Aby pokazać inną jednostkę, ustaw **Jednostka mapowania**, **Min. mapowania** i **Maks. mapowania**. Na przykład pokaż wyjście czujnika temperatury w °C.
5. Kliknij **OK**.
6. Wybierz częstotliwość próbkowania i czas próbkowania na pasku narzędzi.
7. Kliknij **Start** lub naciśnij `S`.

Nie możesz zmienić ustawień kanałów podczas rejestracji. Przy najwyższej częstotliwości próbkowania 10 MHz maksymalny czas próbkowania wynosi około 10 sekund. Przy 1 kHz rejestracja może trwać jedną dobę.

Tryb akwizycji danych używa kalibracji trybu oscyloskopu. Jeśli kanał pokazuje przesunięcie, skalibruj urządzenie w trybie oscyloskopu.
