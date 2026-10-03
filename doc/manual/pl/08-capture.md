# Rejestracja danych

Zanim uruchomisz rejestrację, ustaw te elementy:

1. Opcje urządzenia. Zobacz [Opcje urządzenia](05-device-options.md).
2. Częstotliwość próbkowania i czas próbkowania. Zobacz [Częstotliwość próbkowania i czas próbkowania](06-sample-rate.md).
3. Wyzwalanie, jeśli to potrzebne. Zobacz [Wyzwalanie](07-trigger.md).
4. Tryb rejestracji. Zobacz [Tryby rejestracji](#capture-modes).

## Uruchomienie rejestracji

Są dwa rodzaje rejestracji:

- **Start** uruchamia rejestrację standardową. Urządzenie czeka na wyzwolenie, jeśli ustawisz wyzwalanie.
- **Teraz** uruchamia rejestrację natychmiast. Urządzenie nie używa ustawień wyzwalania.

Aby uruchomić rejestrację standardową, kliknij **Start** lub naciśnij `S`. Aby uruchomić rejestrację natychmiastową, kliknij **Teraz** lub naciśnij `I`. Podczas rejestracji przycisk zmienia się na **Stop**. Kliknij **Stop**, aby zatrzymać rejestrację.

### Przebieg rejestracji standardowej w trybie buforowym

1. Klikasz **Start**.
2. Aplikacja wysyła ustawienia do urządzenia.
3. Jeśli nie ma wyzwalania, urządzenie natychmiast zaczyna rejestrować. Jeśli jest wyzwalanie, urządzenie czeka na wyzwolenie.
4. Urządzenie rejestruje do końca czasu próbkowania lub do zapełnienia pamięci.
5. Urządzenie wysyła dane do komputera.
6. Aplikacja pokazuje przebieg w obszarze przebiegów.

### Przebieg rejestracji standardowej w trybie strumieniowym

1. Klikasz **Start**.
2. Aplikacja wysyła ustawienia do urządzenia.
3. Jeśli jest wyzwalanie, urządzenie czeka na wyzwolenie. W trybie pętli urządzenie nie używa wyzwalania.
4. Urządzenie wysyła dane do komputera podczas rejestracji.
5. Aplikacja pokazuje przebieg podczas rejestracji.
6. Rejestracja zatrzymuje się na końcu czasu próbkowania. W trybie pętli rejestracja trwa, dopóki nie klikniesz **Stop**.

## Używanie rejestracji natychmiastowej

Rejestracja natychmiastowa jest taka sama jak rejestracja standardowa, ale nie używa ustawień wyzwalania. Używaj jej w tych sytuacjach:

- Rejestracja standardowa długo czeka, ponieważ warunek wyzwalania nie występuje.
- Chcesz zobaczyć sygnały w tej chwili.
- Chcesz zbadać sygnały, zanim zmienisz wyzwalanie.

Jeśli nie ma sygnału, rejestracja standardowa czeka w pozycji wyzwalania. Status pokazuje **Oczekiwanie na wyzwolenie!**. Rejestracja natychmiastowa rejestruje sygnały od razu.

## Tryby rejestracji {#capture-modes}

Aby wybrać tryb rejestracji, kliknij **Tryb** na pasku narzędzi. Następnie wybierz jedną z tych pozycji:

| Tryb rejestracji | Tryb buforowy | Tryb strumieniowy |
| --- | --- | --- |
| **Pojedynczy** | Tak | Tak |
| **Powtarzany** | Tak | Tak |
| **Pętla** | Nie | Tak |

![Menu trybów rejestracji](../figures/capture-mode-menu.png)
<!-- TODO: new screenshot -->

### Pojedynczy

Urządzenie wykonuje jedną rejestrację. Potem rejestracja się zatrzymuje.

W trybie buforowym aplikacja pokazuje przebieg po rejestracji. W trybie strumieniowym aplikacja pokazuje przebieg podczas rejestracji.

Używaj tego trybu, aby zarejestrować jeden warunek sygnału lub przebieg w tej chwili.

### Powtarzany

Urządzenie wykonuje rejestrację. Potem automatycznie uruchamia następną rejestrację. Trwa to, dopóki nie klikniesz **Stop**.

W trybie buforowym aplikacja pokazuje okno dla odstępu między rejestracjami. Możesz ustawić wartość od 0,1 s do 10 s.

Używaj tego trybu, aby zobaczyć warunek sygnału, który występuje wiele razy. Używaj go na przykład, aby zobaczyć sygnały po każdym resecie obwodu lub po każdym naciśnięciu przycisku. Używaj go razem z wyzwalaniem.

### Pętla

Ten tryb jest dostępny tylko w trybie strumieniowym. Rejestracja trwa, dopóki nie klikniesz **Stop**. Gdy dane są dłuższe niż czas próbkowania, pierwsze dane wychodzą z okna po lewej stronie. Najnowsze dane wchodzą po prawej stronie. Aplikacja odrzuca dane, które wychodzą z okna.

Używaj tego trybu, gdy nie znasz czasu wystąpienia warunku sygnału. Obserwuj przebieg podczas rejestracji. Gdy zobaczysz warunek, kliknij **Stop**.

> [!NOTE]
> W trybie pętli urządzenie nie używa ustawień wyzwalania.

## Status rejestracji

Podczas rejestracji obszar przebiegów pokazuje status:

- **Oczekiwanie na wyzwolenie!**: Urządzenie czeka na warunek wyzwalania.
- **Wyzwolono!**: Nastąpiło wyzwolenie.
- **% zebrano**: Procent rejestracji, który jest zakończony.

Po rejestracji dół obszaru przebiegów pokazuje **Czas wyzwolenia**.
