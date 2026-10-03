# Częstotliwość próbkowania i czas próbkowania

Pasek narzędzi ma dwie listy dla długości rejestracji. Górna lista to czas próbkowania. Dolna lista to częstotliwość próbkowania.

- **Czas próbkowania** to czas trwania rejestracji.
- **Częstotliwość próbkowania** to liczba próbek na sekundę dla każdego kanału.

Dostępne wartości zmieniają się zależnie od urządzenia, połączenia USB, trybu pracy i trybu kanałów.

## Maksymalny czas próbkowania

**Tryb buforowy.** Pamięć urządzenia ogranicza czas próbkowania. Użyj tego wzoru:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

DSLogic Plus ma 256 Mbit pamięci. Oto dwa przykłady:

- Przy 100 MHz i 16 kanałach maksymalny czas próbkowania wynosi około 167,77 ms.
- Przy 400 MHz i 1 kanale maksymalny czas próbkowania wynosi około 671,09 ms.

**Tryb strumieniowy.** Pamięć komputera ogranicza czas próbkowania. Aplikacja może przechować 16 G próbek dla każdego kanału. Oto dwa przykłady:

- Przy 1 MHz maksymalny czas próbkowania wynosi około 4,77 godziny.
- Przy 100 MHz maksymalny czas próbkowania wynosi około 2,86 minuty.

## Wybór częstotliwości próbkowania

Ustaw częstotliwość próbkowania na 4 do 10 razy większą niż najwyższa częstotliwość w sygnale.

Przy 4-krotności częstotliwości sygnału aplikacja rejestruje każde zbocze. Ale czas każdego zbocza ma błąd do 25% okresu sygnału. Przy 10-krotności częstotliwości sygnału błąd spada do 10%.

Błąd czasu zbocza jest równy jednemu okresowi próbkowania lub mniejszy. Na przykład przy 100 MHz okres próbkowania wynosi 10 ns. Dlatego błąd każdego zbocza wynosi ±10 ns lub mniej.

![Wpływ częstotliwości próbkowania na zarejestrowany przebieg](../figures/sample-rate-effect.png)

Oto typowe wartości:

| Sygnał | Typowa częstotliwość próbkowania |
| --- | --- |
| UART przy 115200 bodów | 2 MHz |
| I2C przy 400 kHz | Od 4 MHz do 10 MHz |
| SPI przy 40 MHz | 400 MHz |

## Nie używaj zbyt wysokiej częstotliwości próbkowania

Wyższa częstotliwość próbkowania daje dokładniejszy przebieg. Ale wysoka częstotliwość próbkowania ma też te problemy:

1. Aplikacja rejestruje więcej danych na sekundę. Dlatego maksymalny czas próbkowania się zmniejsza. Aplikacja potrzebuje też więcej czasu, aby pokazać i zdekodować dane.
2. Wolny sygnał może mieć wolne zbocza. Przy wysokiej częstotliwości próbkowania aplikacja może zarejestrować małe impulsy przy progu podczas każdego wolnego zbocza. Te impulsy mogą spowodować błędy w dekoderach.

Jeśli widzisz niepożądane krótkie impulsy na wolnych sygnałach, zmniejsz częstotliwość próbkowania. Możesz też ustawić **Ustawienia filtra** na **1 takt próbkowania**. Zobacz [Opcje urządzenia](05-device-options.md).

## Ustawianie częstotliwości próbkowania i czasu

1. Ustaw tryb pracy i tryb kanałów. Zobacz [Opcje urządzenia](05-device-options.md).
2. Wybierz częstotliwość próbkowania z dolnej listy na pasku narzędzi.
3. Wybierz czas próbkowania z górnej listy na pasku narzędzi.

> [!NOTE]
> Gdy zmienisz tryb kanałów, aplikacja może zmienić częstotliwość próbkowania. Sprawdź częstotliwość próbkowania ponownie po każdej zmianie opcji urządzenia.
