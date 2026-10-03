# Decodificatori di protocollo

Un decodificatore di protocollo legge i dati di un'acquisizione e trova i frame di un protocollo, per esempio UART, I2C o SPI. L'applicazione mostra il risultato in una nuova riga sopra i canali. L'applicazione ha più di 100 decodificatori.

Per aprire il pannello dei decodificatori, fare clic su **Decodifica** nella barra degli strumenti o premere `D`. Il pannello ha due parti:

- L'elenco dei decodificatori, con il campo **Cerca decodificatore...** in alto.
- L'elenco **Risultati di decodifica**. Questo elenco mostra ogni elemento del decodificatore come una riga di testo.

![Il pannello dei decodificatori](../figures/decoder-dock.png)
<!-- TODO: new screenshot -->

## Aggiungere un decodificatore

> [!NOTE]
> Un decodificatore con il prefisso `0:` è una versione ridotta. Non mostra i bit. Non è possibile aggiungere un protocollo di livello superiore su di esso. Decodifica più velocemente e usa meno memoria.

1. Fare clic sul campo **Cerca decodificatore...**. Si apre l'elenco dei decodificatori.
2. Digitare una parte del nome del protocollo, per esempio `I2C`. L'elenco mostra solo i decodificatori che corrispondono al testo.
3. Fare clic sul decodificatore. Si apre la finestra **Opzioni decodificatore**.
4. Impostare i canali del protocollo. Per esempio, impostare **SCL** e **SDA** per I2C.
5. Impostare le opzioni del protocollo, per esempio il baud rate di una UART.
6. Selezionare le righe di risultati che l'applicazione mostra.
7. Se necessario, impostare la regione di decodifica. Vedere [Decodificare una parte dell'acquisizione](#decode-region).
8. Fare clic su **OK**.

L'applicazione decodifica i dati e mostra i risultati in una nuova riga dell'area delle forme d'onda.

Per aggiungere altri decodificatori, ripetere la procedura per ogni decodificatore.

![I pulsanti del decodificatore: il pulsante delle impostazioni apre le opzioni del decodificatore](../figures/decoder-buttons.png)

Per cambiare le impostazioni di un decodificatore, fare clic sul pulsante delle impostazioni di quel decodificatore nel pannello.

## Aggiungere un decodificatore impilato

Alcuni protocolli usano un protocollo di livello inferiore. Per esempio, il protocollo 24xx EEPROM usa I2C. Quando si aggiunge il protocollo di livello superiore, l'applicazione aggiunge anche i protocolli di livello inferiore.

1. Nel campo **Cerca decodificatore...**, digitare il nome del protocollo di livello superiore, per esempio `24xx`.
2. Fare clic sul decodificatore.
3. Nella finestra **Opzioni decodificatore**, impostare le opzioni di ogni livello di protocollo.
4. Fare clic su **OK**.

I risultati mostrano i frame del protocollo di livello inferiore e i comandi e i dati del protocollo di livello superiore.

## Decodificare una parte dell'acquisizione {#decode-region}

Di solito l'applicazione decodifica tutti i dati. Per decodificare solo una parte, impostare un cursore di inizio e un cursore di fine. Per esempio, è possibile ignorare il rumore durante un reset del circuito. Un'area più breve riduce anche il tempo di decodifica.

1. Aggiungere due cursori all'inizio e alla fine dell'area. Vedere [Misure](10-measure.md).
2. Aprire la finestra **Opzioni decodificatore** del decodificatore.
3. Nell'elenco **Inizio**, selezionare il cursore di inizio.
4. Nell'elenco **Fine**, selezionare il cursore di fine.
5. Fare clic su **OK**.

## Leggere l'elenco dei risultati

L'elenco **Risultati di decodifica** mostra gli elementi del decodificatore in ordine di tempo. Fare clic su una riga per spostare la forma d'onda su quell'elemento.

Per cambiare le colonne dell'elenco, fare clic sul pulsante delle impostazioni in alto nell'elenco.

## Trovare un testo nei risultati

1. Digitare un testo nel campo di ricerca dell'elenco **Risultati di decodifica**.
2. Fare clic sulla freccia destra per andare alla riga successiva che contiene il testo. Fare clic sulla freccia sinistra per andare alla riga precedente.

La forma d'onda si sposta sull'elemento di ogni riga trovata dalla ricerca. Se prima si fa clic su una riga, la ricerca inizia da quella riga.

![Ricerca nei risultati di decodifica](../figures/decoder-list-search.png)

Per trovare una sequenza di byte, inserire il segno `-` tra i byte. Per esempio, `70-70-70` trova tre byte consecutivi con il valore 70.

![Ricerca di una sequenza di byte](../figures/decoder-multibyte-search.png)

> [!NOTE]
> La ricerca di una sequenza di byte funziona solo con i decodificatori UART, I2C e SPI.

## Esportare i risultati

1. Fare clic sul pulsante di salvataggio in alto nell'elenco **Risultati di decodifica**. Si apre la finestra **Esportazione protocollo**.
2. In **Formato di esportazione**, selezionare CSV o TXT.
3. Selezionare ogni colonna da esportare. L'applicazione mette tutte le colonne in un file, in ordine di tempo.
4. Fare clic su **OK**.
5. Selezionare la cartella e digitare il nome del file.
6. Fare clic su **Salva**.

## Eliminare un decodificatore

![Eliminare un decodificatore o tutti i decodificatori](../figures/decoder-delete.png)

- Per eliminare un decodificatore, fare clic sul pulsante **×** nella riga di quel decodificatore.
- Per eliminare tutti i decodificatori, fare clic sul pulsante **×** in alto nel pannello, accanto al pulsante **+**.
