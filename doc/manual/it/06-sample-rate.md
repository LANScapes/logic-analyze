# Frequenza e durata di campionamento

La barra degli strumenti ha due elenchi per la lunghezza dell'acquisizione. L'elenco in alto è la durata di campionamento. L'elenco in basso è la frequenza di campionamento.

- La **durata di campionamento** è la durata dell'acquisizione.
- La **frequenza di campionamento** è il numero di campioni al secondo, per ogni canale.

I valori disponibili cambiano con il dispositivo, il collegamento USB, la modalità operativa e la modalità canali.

## Durata di campionamento massima

**Modalità buffer.** La memoria del dispositivo limita la durata di campionamento. Usare questa formula:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

Il DSLogic Plus ha 256 Mbit di memoria. Ecco due esempi:

- A 100 MHz con 16 canali, la durata di campionamento massima è di circa 167,77 ms.
- A 400 MHz con 1 canale, la durata di campionamento massima è di circa 671,09 ms.

**Modalità streaming.** La memoria del computer limita la durata di campionamento. L'applicazione può conservare 16 G campioni per ogni canale. Ecco due esempi:

- A 1 MHz, la durata di campionamento massima è di circa 4,77 ore.
- A 100 MHz, la durata di campionamento massima è di circa 2,86 minuti.

## Scegliere la frequenza di campionamento

Impostare la frequenza di campionamento a un valore da 4 a 10 volte la frequenza più alta del segnale.

A 4 volte la frequenza del segnale, l'applicazione registra ogni fronte. Ma il tempo di ogni fronte ha un errore fino al 25% del periodo del segnale. A 10 volte la frequenza del segnale, l'errore scende al 10%.

L'errore di tempo di un fronte è uguale a un periodo di campionamento o meno. Per esempio, a 100 MHz il periodo di campionamento è di 10 ns. Per questo l'errore di ogni fronte è di ±10 ns o meno.

![L'effetto della frequenza di campionamento sulla forma d'onda registrata](../figures/sample-rate-effect.png)

Ecco alcuni valori tipici:

| Segnale | Frequenza di campionamento tipica |
| --- | --- |
| UART a 115200 baud | 2 MHz |
| I2C a 400 kHz | Da 4 MHz a 10 MHz |
| SPI a 40 MHz | 400 MHz |

## Non usare una frequenza di campionamento troppo alta

Una frequenza di campionamento più alta dà una forma d'onda più precisa. Ma una frequenza di campionamento alta ha anche questi problemi:

1. L'applicazione registra più dati ogni secondo. Per questo la durata di campionamento massima diminuisce. L'applicazione usa anche più tempo per mostrare e decodificare i dati.
2. Un segnale lento può avere fronti lenti. Con una frequenza di campionamento alta, l'applicazione può registrare piccoli impulsi alla soglia durante ogni fronte lento. Questi impulsi possono causare errori nei decodificatori.

Se si vedono impulsi brevi indesiderati su segnali lenti, ridurre la frequenza di campionamento. È anche possibile impostare **Impostazioni filtro** su **1 ciclo di campionamento**. Vedere [Opzioni del dispositivo](05-device-options.md).

## Impostare la frequenza e la durata di campionamento

1. Impostare la modalità operativa e la modalità canali. Vedere [Opzioni del dispositivo](05-device-options.md).
2. Nell'elenco in basso della barra degli strumenti, selezionare la frequenza di campionamento.
3. Nell'elenco in alto della barra degli strumenti, selezionare la durata di campionamento.

> [!NOTE]
> Quando si cambia la modalità canali, l'applicazione può cambiare la frequenza di campionamento. Controllare di nuovo la frequenza di campionamento dopo ogni modifica delle opzioni del dispositivo.
