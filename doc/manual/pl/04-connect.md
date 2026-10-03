# Podłączenie DSLogic Plus

## Podłączenie kabla USB

> [!NOTE]
> Używaj kabla USB dostarczonego z urządzeniem lub krótkiego kabla USB dobrej jakości. Podłącz kabel bezpośrednio do portu komputera. Koncentrator USB lub długi kabel może spowodować błędy w rejestracji.

1. Podłącz kabel USB do DSLogic Plus.
2. Podłącz drugi koniec kabla USB do portu USB komputera.
3. Upewnij się, że wskaźnik na DSLogic Plus się zaświeca. Zanim aplikacja się uruchomi, wskaźnik świeci na czerwono.
4. Uruchom Logic Analyze.
5. Upewnij się, że wskaźnik zmienia kolor na zielony.
6. Upewnij się, że lista urządzeń na pasku narzędzi pokazuje **DSLogic Plus**.

![Połączenie USB](../figures/usb-connection.png)

Jeśli lista urządzeń nie pokazuje urządzenia, wykonaj te kroki:

1. Odłącz kabel USB od komputera.
2. Poczekaj 5 sekund.
3. Podłącz kabel USB do innego portu USB.
4. Jeśli lista urządzeń nie pokazuje urządzenia po kroku 3, zakończ aplikację i uruchom ją ponownie.

> [!NOTE]
> Tylko jeden program jednocześnie może używać urządzenia. Jeśli `dslcap` lub inny program używa urządzenia, aplikacja nie może go znaleźć.

## Podłączenie kabla pomiarowego

Kabel pomiarowy ma 16 przewodów kanałów. Każdy przewód kanału ma ekran, koniec sygnałowy i koniec masy. Kolory przewodów oznaczają kanały od 0 do 15. Jeszcze jeden przewód ma te sygnały:

- **CK**: Wejście zewnętrznego zegara. Używaj go tylko z ustawieniem **Użyj zegara zewnętrznego**.
- **TI**: Wejście zewnętrznego sygnału wyzwalania.
- **TO**: Wyjście sygnału wyzwalania. Urządzenie wysyła impuls na TO, gdy następuje wyzwolenie.

Zwykle nie podłączasz przewodów CK, TI i TO.

![Kabel pomiarowy i jego kanały](../figures/probe-cable-channels.png)

1. Podłącz kabel pomiarowy do złącza wejściowego DSLogic Plus.
2. Wciśnij złącze do końca w urządzenie.

## Podłączenie kanałów do obwodu

> [!WARNING]
> Nie podłączaj przewodów pomiarowych do napięcia sieciowego. Nie podłączaj przewodów pomiarowych do obwodu, który ma połączenie elektryczne z napięciem sieciowym. Napięcie może spowodować obrażenia lub śmierć.

> [!CAUTION]
> Zanim podłączysz przewód masy, upewnij się, że masa obwodu i masa komputera mają to samo napięcie. Różnica napięć może spowodować duży prąd. Ten prąd może uszkodzić sprzęt.

1. Odłącz zasilanie od obwodu, który mierzysz.
2. Podłącz co najmniej jeden przewód masy do masy obwodu.
3. Podłącz każdy używany przewód kanału do sygnału w obwodzie.
4. Upewnij się, że żaden przewód pomiarowy nie dotyka innego styku.
5. Podłącz zasilanie do obwodu.

![Połączenia masy: jedna wspólna masa (z lewej) lub jedna masa dla każdego kanału (z prawej)](../figures/probe-grounding.png)

Dla sygnałów o częstotliwości poniżej 5 MHz wystarczy jeden przewód masy dla wszystkich kanałów. Dla sygnałów o wyższej częstotliwości podłącz koniec masy każdego przewodu kanału do masy blisko jego sygnału. Krótkie połączenia masy dają czyste zbocza sygnału.

## Odłączenie DSLogic Plus

> [!CAUTION]
> Nie odłączaj kabla USB podczas rejestracji. Jeśli go odłączysz, dane rejestracji mogą mieć błędy.

1. Zatrzymaj rejestrację. Kliknij **Stop**, jeśli ten przycisk jest na pasku narzędzi.
2. Odłącz zasilanie od obwodu.
3. Odłącz przewody pomiarowe od obwodu.
4. Odłącz kabel USB.
