# Wprowadzenie

## O aplikacji Logic Analyze

Logic Analyze to aplikacja dla systemu macOS do analizatorów logicznych firmy DreamSourceLab. Aplikację dostarcza LANScapes. Aplikacja pochodzi z programu DSView firmy DreamSourceLab. DSView używa oprogramowania z projektu sigrok.

Aplikacja rejestruje sygnały cyfrowe z analizatora logicznego DSLogic. Następnie pokazuje sygnały jako przebiegi. Możesz mierzyć przebiegi i dekodować protokoły szeregowe. Możesz też zapisać dane i eksportować je do innych formatów.

Ten podręcznik używa urządzenia DSLogic Plus w przykładach. Inne modele DSLogic działają według tych samych procedur. Ich limity kanałów, pamięci i częstotliwości próbkowania są inne.

Aplikacja zawiera też narzędzie `dslcap`. To narzędzie rejestruje dane bez okna głównego. Zobacz [Narzędzie dslcap](13-dslcap.md).

## O tym podręczniku

Ten podręcznik opiera się na ASD-STE100 Simplified Technical English. Każde zdanie jest krótkie. Każdy krok procedury podaje jedno polecenie. Każdy termin techniczny w tym podręczniku ma tylko jedno znaczenie. [Terminy i czasowniki techniczne](15-terms.md) podaje listę terminów technicznych.

Ten podręcznik używa tych formatów tekstu:

- **Pogrubiony tekst** pokazuje etykietę w aplikacji, na przykład przycisk, pozycję menu lub pole.
- `Tekst kodu` pokazuje klawisz na klawiaturze, polecenie, nazwę pliku lub wartość, którą wpisujesz.
- Ścieżka przez menu używa znaku ›, na przykład **Plik** › **Zapisz...**.
- Lista numerowanych kroków to procedura. Wykonaj kroki w podanej kolejności.

## Instrukcje bezpieczeństwa

Ten podręcznik używa tych etykiet dla instrukcji bezpieczeństwa:

- **OSTRZEŻENIE** oznacza ryzyko obrażeń lub śmierci.
- **UWAGA** oznacza ryzyko uszkodzenia sprzętu lub ryzyko dla twoich danych.
- **INFORMACJA** podaje informacje, które ci pomagają. Informacja po słowie **INFORMACJA** nie podaje polecenia.

Instrukcja bezpieczeństwa stoi przed krokiem, którego dotyczy. Przeczytaj wszystkie instrukcje bezpieczeństwa, zanim zaczniesz procedurę.

> [!WARNING]
> Nie podłączaj przewodów pomiarowych do napięcia sieciowego. Nie podłączaj przewodów pomiarowych do obwodu, który ma połączenie elektryczne z napięciem sieciowym. Przewody pomiarowe mają połączenie elektryczne z komputerem. Napięcie może spowodować obrażenia lub śmierć.

> [!CAUTION]
> Nie podawaj na wejście kanału napięcia wyższego niż limit w specyfikacji urządzenia. Zbyt wysokie napięcie może uszkodzić analizator logiczny.

> [!CAUTION]
> Przewody masy analizatora logicznego mają przez kabel USB połączenie z masą twojego komputera. Podłączaj przewody masy tylko do masy obwodu, który mierzysz. Jeśli te dwie masy mają różne napięcia, może popłynąć duży prąd. Ten prąd może uszkodzić obwód, analizator logiczny i komputer.

## Wymagania systemowe

Ten sprzęt jest potrzebny:

- Komputer Mac z systemem macOS. Informacje o wydaniu podają minimalną wersję macOS.
- Port USB. Port USB 3.0 daje najwyższą prędkość. Port USB 2.0 też działa.
- Analizator logiczny DSLogic, jego kabel USB i jego kabel pomiarowy.

Możesz używać aplikacji bez analizatora logicznego. Urządzenie **Demo** tworzy sygnały testowe. Możesz też otworzyć plik danych z poprzedniej rejestracji.
