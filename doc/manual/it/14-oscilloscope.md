# Modalità oscilloscopio e acquisizione dati

Logic Analyze può usare anche gli oscilloscopi DSCope di DreamSourceLab. Un DSCope ha due modalità del dispositivo:

- **Oscilloscopio**: per segnali con un periodo costante e per una condizione di segnale.
- **Acquisizione dati**: per segnali lenti per un tempo lungo, per esempio una tensione di alimentazione o l'uscita di un sensore.

Queste modalità non sono disponibili sui dispositivi DSLogic. Questo capitolo dà solo le procedure principali.

## Collegare il DSCope

> [!WARNING]
> Non collegare le sonde alla tensione di rete. Non collegare le sonde a un circuito che ha un collegamento elettrico con la tensione di rete. La tensione può causare lesioni o la morte.

> [!CAUTION]
> La massa delle sonde, la massa del DSCope e la massa del computer sono collegate tra loro. Collegare la massa della sonda solo a un punto che ha la stessa tensione della massa del computer. Una differenza di tensione può danneggiare l'apparecchiatura.

1. Collegare il DSCope al computer con il cavo USB.
2. Avviare Logic Analyze. Verificare che l'elenco dei dispositivi mostri il DSCope.
3. Collegare le sonde agli ingressi del DSCope.
4. Impostare il selettore di attenuazione di ogni sonda.
5. Collegare il morsetto di massa di ogni sonda alla massa del circuito.
6. Collegare la punta della sonda al segnale.

## Opzioni del dispositivo

Fare clic su **Opzioni** › **Opzioni dispositivo...** o premere `O`.

- **Modalità operativa**: **Normale** per le misure. **Test interno** serve solo per i test del dispositivo.
- **Limite di banda**: **Banda completa** o **20MHz**. Il limite di 20 MHz riduce il rumore ad alta frequenza.

## Calibrare il DSCope

Il guadagno e l'offset degli ingressi cambiano con la temperatura e l'umidità. Calibrare il DSCope per mantenere le misure precise.

### Calibrazione automatica

> [!CAUTION]
> Scollegare tutte le sonde dagli ingressi prima della calibrazione. Un segnale su un ingresso durante la calibrazione dà valori di calibrazione non corretti.

1. Aprire la finestra **Opzioni dispositivo**.
2. Fare clic su **Calibrazione automatica**.
3. Scollegare tutte le sonde. Fare clic su **OK**. La calibrazione dura alcuni minuti.
4. Quando la calibrazione è completa, fare clic su **Salva** per conservare il risultato.

Per arrestare la calibrazione, fare clic su **Interrompi**. Il dispositivo usa allora i valori di calibrazione precedenti.

### Calibrazione manuale

1. Aprire la finestra **Opzioni dispositivo**.
2. Fare clic su **Calibrazione manuale**.
3. Fare clic su **Avvia** nella barra degli strumenti.
4. Per regolare l'offset, collegare la sonda a massa. Per regolare il guadagno, collegare la sonda a un segnale di tensione nota.
5. Impostare la scala verticale da calibrare.
6. Spostare il cursore **VOFF** o **VGAIN** del canale fino a quando la forma d'onda è corretta.
7. Ripetere i passi 5 e 6 per ogni scala verticale.
8. Fare clic su **Salva**.

Per annullare le modifiche, fare clic su **Interrompi**. Per usare le modifiche solo fino allo scollegamento del dispositivo, fare clic su **Esci**. Per tornare ai valori iniziali, fare clic su **Ripristina**. Dopo un ripristino, eseguire di nuovo la calibrazione automatica.

## Impostazioni dei canali

Ogni canale ha questi comandi a sinistra dell'area delle forme d'onda:

- **Attiva**: attiva o disattiva il canale.
- **Scala verticale**: la tensione per divisione. La finestra ha 10 divisioni. Per cambiare la scala, girare la rotellina del mouse sulla manopola, o fare clic sulla parte alta o bassa della manopola. È anche possibile premere `0` o `1` per selezionare la manopola di un canale e poi premere `↑` o `↓`.
- **Accoppiamento**: **DC** o **AC**.
- **Attenuazione della sonda**: impostare **x1** o **x10** in base al selettore della sonda.
- **AUTO**: imposta la scala verticale, la scala orizzontale e il livello di trigger per il segnale presente sull'ingresso.

Per spostare la forma d'onda di un canale verso l'alto o verso il basso, trascinare l'etichetta del canale.

## Scala orizzontale

Selezionare il tempo per divisione nell'elenco della barra degli strumenti. È anche possibile girare la rotellina del mouse nell'area delle forme d'onda.

## Avviare e arrestare

- Fare clic su **Avvia** o premere `S` per avviare un'acquisizione continua. Fare clic su **Arresta** per arrestarla.
- Fare clic su **Singola** o premere `I` per acquisire una forma d'onda e arrestare.

## Trigger

Fare clic su **Trigger** o premere `T` per aprire il pannello del trigger. Il pannello ha queste impostazioni:

- **Posizione trigger**: la posizione del punto di trigger nell'acquisizione, in percentuale.
- **Tempo di hold-off**: il tempo dopo un trigger durante il quale il dispositivo ignora i nuovi trigger. Usarlo per ottenere una forma d'onda stabile da gruppi di impulsi.
- **Sensibilità trigger**: la variazione di tensione necessaria per un trigger. Un valore più grande ignora più rumore.
- **Sorgenti trigger**: **Auto**, **Canale 0**, **Canale 1**, **Canale 0 && 1** o **Canale 0 | 1**.
- **Tipi di trigger**: **Fronte di salita** o **Fronte di discesa**.

Per impostare il livello di trigger, fare clic sull'etichetta del livello di trigger del canale. Spostare il mouse. Fare di nuovo clic per fissare il livello.

## Misure

### Misure automatiche

La parte in basso dell'area delle forme d'onda ha 10 riquadri per le misure automatiche.

1. Fare clic su un riquadro di misura.
2. Selezionare il canale.
3. Selezionare la misura. Per svuotare il riquadro, fare clic su **Ripristina**.

L'applicazione conserva queste impostazioni per il successivo avvio.

### Cursori

- Per aggiungere un cursore di tempo, fare clic sul righello del tempo. È anche possibile fare clic con il pulsante destro del mouse nell'area delle forme d'onda e selezionare **Aggiungi cursore Y**.
- Per aggiungere un cursore di tensione, fare clic con il pulsante destro del mouse nell'area delle forme d'onda e selezionare **Aggiungi cursore X**. Ogni cursore di tensione ha due linee orizzontali. L'etichetta tra le linee mostra la differenza di tensione.
- Per misurare il tempo tra due cursori, usare il gruppo **Distanza cursori** del pannello delle misure.

### Misurare con il puntatore

Dopo l'arresto dell'acquisizione, posizionare il puntatore sulla forma d'onda. L'applicazione mostra la tensione del campione sotto il puntatore.

Per misurare un tempo, fare doppio clic in una zona vuota della forma d'onda. Fare clic sul secondo punto. Fare clic sul terzo punto per vedere la frequenza, il periodo e il duty cycle. Fare clic con il pulsante destro del mouse per annullare.

## Spettro (FFT)

1. Fare clic su **Funzione** › **FFT**.
2. Selezionare **Attiva FFT**.
3. Impostare **Lunghezza FFT**, **Intervallo di campionamento**, **Sorgente FFT** e **Finestra FFT**.
4. Impostare **Modalità asse Y** e **Intervallo DBV**.
5. Fare clic su **OK**.

Lo spettro compare sotto la forma d'onda. Girare la rotellina del mouse nello spettro per fare lo zoom sulla scala delle frequenze. Trascinare lo spettro per spostarlo. Posizionare il puntatore sullo spettro per vedere la frequenza e l'ampiezza.

## Canale matematico

1. Fare clic su **Funzione** › **Math**.
2. Selezionare **Attiva**.
3. Selezionare il **Tipo di operazione**: **Somma**, **Sottrazione**, **Moltiplicazione** o **Divisione**.
4. Selezionare la **1ª sorgente** e la **2ª sorgente**.
5. Fare clic su **OK**.

## Figura di Lissajous

1. Fare clic su **Opzioni** › **Display** › **Lissajous**.
2. Selezionare **Attiva**.
3. Selezionare il canale per l'**Asse X** e l'**Asse Y**.
4. Fare clic su **OK**.

## Modalità acquisizione dati

1. Nell'elenco delle modalità del dispositivo della barra degli strumenti, selezionare **Acquisizione dati**.
2. Aprire la finestra **Opzioni dispositivo**.
3. Per ogni canale, impostare **Attiva**, **Accoppiamento** e **Volt/div**.
4. Per mostrare un'altra unità, impostare **Unità di mappatura**, **Mappatura min** e **Mappatura max**. Per esempio, mostrare l'uscita di un sensore di temperatura in °C.
5. Fare clic su **OK**.
6. Selezionare la frequenza e la durata di campionamento nella barra degli strumenti.
7. Fare clic su **Avvia** o premere `S`.

Non è possibile cambiare le impostazioni dei canali durante l'acquisizione. Alla frequenza di campionamento massima di 10 MHz, la durata di campionamento massima è di circa 10 secondi. A 1 kHz, l'acquisizione può continuare per un giorno.

La modalità acquisizione dati usa la calibrazione della modalità oscilloscopio. Se un canale mostra un offset, calibrare il dispositivo in modalità oscilloscopio.
