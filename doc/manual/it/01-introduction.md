# Introduzione

## Informazioni su Logic Analyze

Logic Analyze è un'applicazione per macOS per gli analizzatori logici di DreamSourceLab. LANScapes fornisce l'applicazione. L'applicazione deriva da DSView, un programma di DreamSourceLab. DSView usa software del progetto sigrok.

L'applicazione registra segnali digitali con un analizzatore logico DSLogic. Poi mostra i segnali come forme d'onda. È possibile misurare le forme d'onda e decodificare protocolli seriali. È anche possibile salvare i dati ed esportarli in altri formati.

Questo manuale usa il DSLogic Plus negli esempi. Gli altri modelli DSLogic usano le stesse procedure. I loro limiti di canali, memoria e frequenza di campionamento sono diversi.

L'applicazione contiene anche lo strumento `dslcap`. Questo strumento acquisisce dati senza la finestra principale. Vedere [Lo strumento dslcap](13-dslcap.md).

## Informazioni su questo manuale

Questo manuale applica le regole di ASD-STE100 Simplified Technical English. Ogni frase è breve. Ogni passo di una procedura dà una sola istruzione. Ogni termine tecnico di questo manuale ha un solo significato. [Termini tecnici e verbi](15-terms.md) dà l'elenco dei termini tecnici.

Questo manuale usa questi formati di testo:

- Il **testo in grassetto** indica un'etichetta dell'applicazione, per esempio un pulsante, una voce di menu o un campo.
- Il `testo in codice` indica un tasto della tastiera, un comando, un nome di file o un valore da digitare.
- Un percorso nei menu usa il segno ›, per esempio **File** › **Salva...**.
- Un elenco di passi numerati è una procedura. Eseguire i passi nell'ordine indicato.

## Istruzioni di sicurezza

Questo manuale usa queste etichette per le istruzioni di sicurezza:

- **AVVERTENZA** indica un rischio di lesioni o di morte.
- **ATTENZIONE** indica un rischio di danni all'apparecchiatura o un rischio per i dati.
- **NOTA** dà un'informazione utile. L'informazione dopo **NOTA** non dà istruzioni.

Un'istruzione di sicurezza si trova prima del passo a cui si applica. Leggere tutte le istruzioni di sicurezza prima di iniziare una procedura.

> [!WARNING]
> Non collegare le sonde alla tensione di rete. Non collegare le sonde a un circuito che ha un collegamento elettrico con la tensione di rete. Le sonde hanno un collegamento elettrico con il computer. La tensione può causare lesioni o la morte.

> [!CAUTION]
> Non applicare a un ingresso di canale una tensione superiore al limite della specifica del dispositivo. Una tensione troppo alta può danneggiare l'analizzatore logico.

> [!CAUTION]
> I fili di massa dell'analizzatore logico sono collegati alla massa del computer tramite il cavo USB. Collegare i fili di massa solo alla massa del circuito da misurare. Se le due masse hanno tensioni diverse, può circolare una corrente elevata e danneggiare il circuito, l'analizzatore logico e il computer.

## Requisiti di sistema

È necessaria questa apparecchiatura:

- Un Mac con macOS. Le note di rilascio danno la versione minima di macOS.
- Una porta USB. Una porta USB 3.0 dà la velocità più alta. Anche una porta USB 2.0 funziona.
- Un analizzatore logico DSLogic, il suo cavo USB e il suo cavo delle sonde.

È possibile usare l'applicazione senza un analizzatore logico. Il dispositivo **Demo** genera segnali di prova. È anche possibile aprire un file di dati di un'acquisizione precedente.
