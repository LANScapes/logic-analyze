# Pomiary

Możesz mierzyć przebieg myszą lub kursorami. Kursor to pionowa linia w danym czasie rejestracji.

## Pomiar impulsu wskaźnikiem

Ustaw wskaźnik na impulsie kanału. Pole obok wskaźnika pokazuje te wartości dla impulsu:

- **Szerokość**: czas trwania impulsu.
- **Okres**: czas od jednego zbocza do następnego zbocza w tym samym kierunku.
- **Częstotliwość**: 1 podzielone przez okres.
- **Wypełnienie**: czas stanu wysokiego jako procent okresu.

![Pomiar przy wskaźniku](../figures/hover-measurement.png)

Aby włączyć lub wyłączyć to pole, otwórz panel boczny pomiarów i użyj ustawienia **Włącz pomiar pływający**.

## Liczenie zboczy w obszarze

1. Ustaw wskaźnik na przebiegu kanału, między poziomem wysokim a poziomem niskim.
2. Przesuń wskaźnik na początek obszaru.
3. Kliknij lewym przyciskiem myszy.
4. Przesuń wskaźnik na koniec obszaru. Aplikacja pokazuje liczbę zboczy, zboczy narastających i zboczy opadających.
5. Kliknij ponownie lewym przyciskiem myszy, aby zakończyć pomiar.

## Pomiar czasu między dwoma zboczami

1. Ustaw wskaźnik na pierwszym zboczu.
2. Kliknij lewym przyciskiem myszy.
3. Przesuń wskaźnik na drugie zbocze. Aplikacja pokazuje czas i liczbę próbek między dwoma zboczami.
4. Kliknij ponownie lewym przyciskiem myszy, aby zakończyć pomiar.

![Czas między dwoma zboczami](../figures/edge-distance.png)

## Dodawanie kursora

Użyj jednej z tych metod:

- W obszarze przebiegów kliknij dwukrotnie lewym przyciskiem myszy w wybranym czasie. Jeśli wskaźnik jest blisko zbocza, kursor przeskakuje na zbocze.
- Na linijce czasu kliknij lewym przyciskiem myszy. Na linijce pojawia się strzałka. Kliknij strzałkę, aby dodać kursor.

![Dodawanie kursora z linijki czasu](../figures/ruler-insert-cursor.png)

Każdy kursor ma numer. Numery zaczynają się od 1.

## Przesuwanie kursora

Użyj jednej z tych metod:

- Ustaw wskaźnik na kursorze. Linia kursora staje się grubsza. Kliknij kursor. Przesuń mysz. Kliknij ponownie, aby upuścić kursor. Blisko zbocza kursor przeskakuje na zbocze.
- Na linijce czasu kliknij lewym przyciskiem myszy w nowym czasie. Linijka pokazuje numery wszystkich kursorów. Kliknij numer kursora, który chcesz przesunąć.

![Przesuwanie kursora z linijki czasu](../figures/ruler-move-cursor.png)

## Przejście do kursora

1. Na linijce czasu kliknij prawym przyciskiem myszy. Linijka pokazuje numery wszystkich kursorów.
2. Kliknij numer kursora. Przebieg przesuwa się do pozycji tego kursora.

![Przejście do kursora 3](../figures/ruler-jump-cursor.png)

## Pomiary kursorami

Aby otworzyć panel boczny pomiarów, kliknij **Pomiar** na pasku narzędzi lub naciśnij `M`. Panel boczny ma te grupy:

- **Odległość kursorów**: czas i liczba próbek między dwoma kursorami.
- **Zbocza**: liczba zboczy na jednym kanale między dwoma kursorami.
- **Kursory**: czas i numer próbki każdego kursora.

Aby dodać pomiar czasu, wykonaj te kroki:

1. W grupie **Odległość kursorów** kliknij przycisk **+**.
2. Kliknij pole początku i wybierz pierwszy kursor.
3. Kliknij pole końca i wybierz drugi kursor.

Panel boczny pokazuje wynik w kolumnie **Czas/próbki**.

Aby dodać liczenie zboczy, wykonaj te kroki:

1. W grupie **Zbocza** kliknij przycisk **+**.
2. Wybierz kursor początkowy i kursor końcowy.
3. Wybierz kanał.

Panel boczny pokazuje liczbę zboczy narastających, zboczy opadających i wszystkich zboczy.

Aby usunąć pomiar, kliknij przycisk **×** w jego wierszu.

## Usuwanie kursora

Użyj jednej z tych metod:

- Kliknij **×** na etykiecie kursora na linijce czasu.
- Kliknij przycisk **×** kursora w grupie **Kursory** panelu bocznego pomiarów.

Aplikacja nadaje nowe numery pozostałym kursorom.
