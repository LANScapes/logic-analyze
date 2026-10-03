# Trigger

Un trigger è una condizione nei segnali. Quando la condizione si verifica, il dispositivo segna quell'istante come punto di trigger. Il trigger permette di acquisire la parte del segnale da esaminare.

L'applicazione ha due tipi di trigger:

- **Trigger semplice**: Un fronte o un livello su uno o più canali.
- **Trigger avanzato**: Una sequenza di condizioni o un valore su un bus seriale.

Per aprire il pannello del trigger, fare clic su **Trigger** nella barra degli strumenti o premere `T`.

> [!NOTE]
> Se il segnale non corrisponde alla condizione di trigger, l'acquisizione continua ad attendere. Per vedere il segnale senza il trigger, fare clic su **Immediata**. Per arrestare l'attesa, fare clic su **Arresta**.

## Posizione del trigger

L'impostazione **Posizione trigger** definisce dove si trova il punto di trigger nell'acquisizione. Il valore è una percentuale della durata di campionamento.

- Un valore piccolo, per esempio 10%, mostra una parte maggiore del segnale dopo il trigger.
- Un valore grande, per esempio 90%, mostra una parte maggiore del segnale prima del trigger.

La posizione del trigger usa la memoria del dispositivo. Per questo è possibile impostarla solo in modalità buffer. In modalità streaming, la posizione del trigger è sempre di circa 1%.

![Posizione del trigger al 10% (a sinistra) e al 90% (a destra)](../figures/trigger-position.png)
<!-- TODO: new screenshot -->

## Trigger semplice

Ogni etichetta di canale nell'area delle forme d'onda ha cinque pulsanti di trigger. Da sinistra a destra, i pulsanti sono:

1. Fronte di salita
2. Livello alto
3. Fronte di discesa
4. Livello basso
5. Fronte di salita o fronte di discesa

![I pulsanti di trigger su un'etichetta di canale](../figures/simple-trigger-buttons.png)

Per impostare un trigger semplice, eseguire questi passi:

1. Aprire il pannello del trigger.
2. Selezionare **Trigger semplice**.
3. Sull'etichetta di un canale, fare clic sul pulsante di trigger desiderato. Il pulsante cambia colore.
4. Per rimuovere il trigger da un canale, fare di nuovo clic sullo stesso pulsante.
5. Impostare la **Posizione trigger**.

Se si imposta un trigger su più di un canale, tutte le condizioni devono verificarsi sullo stesso campione (AND logico).

## Trigger avanzato

> [!NOTE]
> Il trigger avanzato è disponibile solo in modalità buffer. Per usarlo, impostare **Modalità operativa** su **Modalità buffer**. Vedere [Opzioni del dispositivo](05-device-options.md).

Per usare il trigger avanzato, selezionare **Trigger avanzato** nel pannello del trigger. Poi selezionare la scheda **Trigger a stadi** o la scheda **Trigger seriale**.

### Valori per ogni canale

Il trigger a stadi e il trigger seriale usano una riga di 16 caratteri. Ogni carattere è la condizione per un canale. Il carattere a destra è il canale 0. Il carattere a sinistra è il canale 15.

| Carattere | Condizione |
| --- | --- |
| `X` | Tutti i valori (il canale non ha effetto). |
| `0` | Livello basso. |
| `1` | Livello alto. |
| `R` | Fronte di salita. |
| `F` | Fronte di discesa. |
| `C` | Fronte di salita o fronte di discesa. |

### Trigger a stadi

Un trigger a stadi è una sequenza di condizioni. Ogni condizione è uno stadio. Il dispositivo esamina per primo lo stadio 0. Quando la condizione di uno stadio si verifica, il dispositivo passa allo stadio successivo. Il trigger si verifica quando l'ultimo stadio è completo. È possibile usare fino a 16 stadi.

Ogni stadio ha queste impostazioni:

- Due righe di condizioni di canale.
- Per ogni riga, `==` o `!=`. Con `==`, la condizione si verifica quando i canali corrispondono alla riga. Con `!=`, la condizione si verifica quando i canali non corrispondono alla riga.
- **And** o **Or**. Questa impostazione collega le due righe.
- **Contatore**: Il numero di volte che la condizione deve verificarsi prima che lo stadio sia completo.
- **Contigui**: Quando si seleziona questa casella, la condizione deve verificarsi su campioni consecutivi senza interruzioni.

![Le impostazioni del trigger a stadi](../figures/stage-trigger-panel.png)
<!-- TODO: new screenshot -->

Per impostare un trigger a stadi, eseguire questi passi:

1. In **Stadi di trigger totali**, selezionare il numero di stadi.
2. Nell'elenco degli stadi a destra, fare clic sullo stadio 0.
3. Digitare le condizioni di canale nella prima riga.
4. Se necessario, digitare le condizioni di canale nella seconda riga e selezionare **And** o **Or**.
5. Digitare un valore in **Contatore**.
6. Ripetere i passi da 2 a 5 per ogni altro stadio.

Ecco tre esempi.

**Esempio 1.** Trigger quando il canale 0 resta a livello alto per più di 1000 campioni:

1. Impostare **Stadi di trigger totali** su 1.
2. Nello stadio 0, digitare `1` per il canale 0 nella prima riga.
3. Selezionare **Contigui**.
4. Impostare **Contatore** su 1000.

![Esempio 1](../figures/stage-example-level-count.png)

**Esempio 2.** Trigger su un fronte di salita del canale 0 o un fronte di discesa del canale 1:

1. Impostare **Stadi di trigger totali** su 1.
2. Nello stadio 0, digitare `R` per il canale 0 nella prima riga.
3. Digitare `F` per il canale 1 nella seconda riga.
4. Selezionare **Or**.

![Esempio 2](../figures/stage-example-or.png)

**Esempio 3.** Trigger su un fronte di salita del canale 0, poi 100 fronti di discesa del canale 1, poi un livello alto del canale 2:

1. Impostare **Stadi di trigger totali** su 3.
2. Nello stadio 0, digitare `R` per il canale 0.
3. Nello stadio 1, digitare `F` per il canale 1. Impostare **Contatore** su 100.
4. Nello stadio 2, digitare `1` per il canale 2.

![Esempio 3](../figures/stage-example-sequence.png)

### Trigger seriale

Un trigger seriale trova un valore di dati su un bus seriale. Funziona come un registro a scorrimento. Ecco le impostazioni:

- **Flag di inizio**: La condizione che avvia il trigger seriale.
- **Flag di fine**: La condizione che azzera il registro a scorrimento.
- **Flag di clock**: La condizione che aggiunge un bit al registro a scorrimento.
- **Canale dati**: Il canale che trasmette i dati.
- **Bit di dati**: Il numero di bit del valore.
- **Valore dati**: Il valore che causa il trigger.

Dopo il flag di inizio, il dispositivo legge il canale dati a ogni flag di clock. Il dispositivo inserisce questo bit nel registro a scorrimento. Quando gli ultimi bit del registro a scorrimento sono uguali a **Valore dati**, il trigger si verifica. Quando il flag di fine si verifica, il dispositivo azzera il registro a scorrimento.

![Le impostazioni del trigger seriale](../figures/serial-trigger-panel.png)
<!-- TODO: new screenshot -->

**Esempio 4.** Trigger quando il valore `010000100` compare su un bus I2C. Il canale 0 è SCL e il canale 1 è SDA.

1. Impostare **Flag di inizio** su un fronte di discesa di SDA mentre SCL è a livello alto: `F1` nei due caratteri a destra.
2. Impostare **Flag di fine** su un fronte di salita di SDA mentre SCL è a livello alto: `R1`.
3. Impostare **Flag di clock** su un fronte di salita di SCL: `R` per il canale 0.
4. Impostare **Canale dati** su 1.
5. Impostare **Bit di dati** su 9.
6. Digitare `010000100` in **Valore dati**.

![Esempio 4](../figures/serial-example-i2c.png)

**Esempio 5.** Trigger quando il valore `0x1234` compare su MOSI di un bus SPI. Il canale 0 è CS#, il canale 1 è CLK, il canale 2 è MISO e il canale 3 è MOSI.

1. Impostare **Flag di inizio** su un fronte di discesa di CS#: `F` per il canale 0.
2. Impostare **Flag di fine** su un fronte di salita di CS#: `R` per il canale 0.
3. Impostare **Flag di clock** su un fronte di salita di CLK: `R` per il canale 1.
4. Impostare **Canale dati** su 3.
5. Impostare **Bit di dati** su 16.
6. Digitare `0001001000110100` in **Valore dati**.

![Esempio 5](../figures/serial-example-spi.png)

Per digitare il valore in esadecimale, selezionare **Inserimento in formato esadecimale**. Poi digitare il valore nel campo **Hex**.
