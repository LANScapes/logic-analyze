# Collegare il DSLogic Plus

## Collegare il cavo USB

> [!NOTE]
> Usare il cavo USB fornito con il dispositivo o un cavo USB corto di buona qualità. Collegare il cavo direttamente a una porta del computer. Un hub USB o un cavo lungo può causare errori in un'acquisizione.

1. Collegare il cavo USB al DSLogic Plus.
2. Collegare l'altra estremità del cavo USB a una porta USB del computer.
3. Verificare che l'indicatore del DSLogic Plus si accenda. Prima dell'avvio dell'applicazione, l'indicatore è rosso.
4. Avviare Logic Analyze.
5. Verificare che l'indicatore diventi verde.
6. Verificare che l'elenco dei dispositivi nella barra degli strumenti mostri **DSLogic Plus**.

![Il collegamento USB](../figures/usb-connection.png)

Se l'elenco dei dispositivi non mostra il dispositivo, eseguire questi passi:

1. Scollegare il cavo USB dal computer.
2. Attendere 5 secondi.
3. Collegare il cavo USB a un'altra porta USB.
4. Se l'elenco dei dispositivi non mostra il dispositivo dopo il passo 3, uscire dall'applicazione e avviarla di nuovo.

> [!NOTE]
> Un solo programma alla volta può usare il dispositivo. Se `dslcap` o un altro programma usa il dispositivo, l'applicazione non lo trova.

## Collegare il cavo delle sonde

Il cavo delle sonde ha 16 fili di canale. Ogni filo di canale ha una schermatura, un'estremità di segnale e un'estremità di massa. I colori dei fili identificano i canali da 0 a 15. Un filo in più porta questi segnali:

- **CK**: L'ingresso per un clock esterno. Usarlo solo con l'impostazione **Usa clock esterno**.
- **TI**: L'ingresso per un segnale di trigger esterno.
- **TO**: L'uscita del segnale di trigger. Il dispositivo invia un impulso su TO quando il trigger si verifica.

Di solito non è necessario collegare i fili CK, TI e TO.

![Il cavo delle sonde e i suoi canali](../figures/probe-cable-channels.png)

1. Collegare il cavo delle sonde al connettore di ingresso del DSLogic Plus.
2. Spingere il connettore completamente nel dispositivo.

## Collegare i canali al circuito

> [!WARNING]
> Non collegare le sonde alla tensione di rete. Non collegare le sonde a un circuito che ha un collegamento elettrico con la tensione di rete. La tensione può causare lesioni o la morte.

> [!CAUTION]
> Prima di collegare un filo di massa, verificare che la massa del circuito e la massa del computer abbiano la stessa tensione. Una differenza di tensione può causare una corrente elevata che può danneggiare l'apparecchiatura.

1. Scollegare l'alimentazione dal circuito da misurare.
2. Collegare almeno un filo di massa alla massa del circuito.
3. Collegare ogni filo di canale usato a un segnale del circuito.
4. Verificare che nessuna sonda tocchi un altro contatto.
5. Collegare l'alimentazione al circuito.

![Collegamenti di massa: una massa comune (a sinistra) o una massa per ogni canale (a destra)](../figures/probe-grounding.png)

Per segnali con una frequenza inferiore a 5 MHz, un solo filo di massa per tutti i canali è sufficiente. Per segnali con una frequenza più alta, collegare l'estremità di massa di ogni filo di canale alla massa vicino al suo segnale. Collegamenti di massa corti danno fronti di segnale puliti.

## Scollegare il DSLogic Plus

> [!CAUTION]
> Non scollegare il cavo USB durante un'acquisizione. Se lo si scollega, i dati dell'acquisizione possono contenere errori.

1. Arrestare l'acquisizione. Fare clic su **Arresta** se è visibile nella barra degli strumenti.
2. Scollegare l'alimentazione dal circuito.
3. Scollegare le sonde dal circuito.
4. Scollegare il cavo USB.
