# Misure

È possibile misurare la forma d'onda con il mouse o con i cursori. Un cursore è una linea verticale in un istante dell'acquisizione.

## Misurare un impulso con il puntatore

Posizionare il puntatore su un impulso di un canale. Un riquadro vicino al puntatore mostra questi valori dell'impulso:

- **Larghezza**: la durata dell'impulso.
- **Periodo**: il tempo da un fronte al fronte successivo della stessa direzione.
- **Frequenza**: 1 diviso il periodo.
- **Duty cycle**: il tempo a livello alto come percentuale del periodo.

![La misura al puntatore](../figures/hover-measurement.png)

Per mostrare o nascondere questo riquadro, aprire il pannello delle misure e usare **Attiva misura flottante**.

## Contare i fronti in un'area

1. Posizionare il puntatore sulla forma d'onda del canale, tra il livello alto e il livello basso.
2. Spostare il puntatore all'inizio dell'area.
3. Fare clic con il pulsante sinistro del mouse.
4. Spostare il puntatore alla fine dell'area. L'applicazione mostra il numero di fronti, di fronti di salita e di fronti di discesa.
5. Fare di nuovo clic con il pulsante sinistro del mouse per completare la misura.

## Misurare il tempo tra due fronti

1. Posizionare il puntatore sul primo fronte.
2. Fare clic con il pulsante sinistro del mouse.
3. Spostare il puntatore sul secondo fronte. L'applicazione mostra il tempo e il numero di campioni tra i due fronti.
4. Fare di nuovo clic con il pulsante sinistro del mouse per completare la misura.

![Il tempo tra due fronti](../figures/edge-distance.png)

## Aggiungere un cursore

Usare uno di questi metodi:

- Nell'area delle forme d'onda, fare doppio clic con il pulsante sinistro del mouse nell'istante desiderato. Se il puntatore è vicino a un fronte, il cursore si posiziona sul fronte.
- Nel righello del tempo, fare clic con il pulsante sinistro del mouse. Sul righello compare una freccia. Fare clic sulla freccia per aggiungere un cursore.

![Aggiungere un cursore dal righello del tempo](../figures/ruler-insert-cursor.png)

Ogni cursore ha un numero. I numeri iniziano da 1.

## Spostare un cursore

Usare uno di questi metodi:

- Posizionare il puntatore sul cursore. La linea del cursore diventa più spessa. Fare clic sul cursore. Spostare il mouse. Fare di nuovo clic per rilasciare il cursore. Vicino a un fronte, il cursore si posiziona sul fronte.
- Nel righello del tempo, fare clic con il pulsante sinistro del mouse nel nuovo istante. Il righello mostra i numeri di tutti i cursori. Fare clic sul numero del cursore da spostare.

![Spostare un cursore dal righello del tempo](../figures/ruler-move-cursor.png)

## Andare a un cursore

1. Nel righello del tempo, fare clic con il pulsante destro del mouse. Il righello mostra i numeri di tutti i cursori.
2. Fare clic sul numero di un cursore. La forma d'onda si sposta alla posizione di quel cursore.

![Andare al cursore 3](../figures/ruler-jump-cursor.png)

## Misurare con i cursori

Per aprire il pannello delle misure, fare clic su **Misura** nella barra degli strumenti o premere `M`. Il pannello ha questi gruppi:

- **Distanza cursori**: il tempo e il numero di campioni tra due cursori.
- **Fronti**: il numero di fronti su un canale tra due cursori.
- **Cursori**: il tempo e il numero di campione di ogni cursore.

Per aggiungere una misura di tempo, eseguire questi passi:

1. Nel gruppo **Distanza cursori**, fare clic sul pulsante **+**.
2. Fare clic sul campo di inizio e selezionare il primo cursore.
3. Fare clic sul campo di fine e selezionare il secondo cursore.

Il pannello mostra il risultato nella colonna **Tempo/Campioni**.

Per aggiungere un conteggio di fronti, eseguire questi passi:

1. Nel gruppo **Fronti**, fare clic sul pulsante **+**.
2. Selezionare il cursore di inizio e il cursore di fine.
3. Selezionare il canale.

Il pannello mostra il numero di fronti di salita, di fronti di discesa e di tutti i fronti.

Per rimuovere una misura, fare clic sul pulsante **×** della sua riga.

## Eliminare un cursore

Usare uno di questi metodi:

- Fare clic sulla **×** dell'etichetta del cursore nel righello del tempo.
- Fare clic sul pulsante **×** del cursore nel gruppo **Cursori** del pannello delle misure.

L'applicazione dà nuovi numeri ai cursori che restano.
