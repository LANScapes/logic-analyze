# Lo strumento dslcap

Lo strumento `dslcap` acquisisce dati da un dispositivo DSLogic senza la finestra principale. Usarlo in script e in test automatici. Lo strumento scrive i campioni in un file binario. Scrive un oggetto JSON con il risultato sull'uscita standard.

Lo strumento si trova nel pacchetto dell'applicazione:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Un solo programma alla volta può usare il dispositivo. Uscire da Logic Analyze prima di usare `dslcap`.

## Elencare i dispositivi

Per elencare i dispositivi che la libreria può trovare, digitare questo comando:

```sh
dslcap --list
```

Per elencare l'identificatore USB di ogni dispositivo DSLogic collegato, digitare questo comando:

```sh
dslcap --list-ids
```

Il comando `--list-ids` legge solo le informazioni che macOS conserva sui dispositivi USB. Non invia dati al dispositivo. L'uscita dà il modello, la posizione USB e un identificatore di registro per ogni dispositivo.

## Acquisire dati

Questo comando acquisisce 1000000 campioni sui canali 0 e 1 a 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

Lo strumento scrive i campioni in `/tmp/capture.bin`. Se esiste già un file con questo nome, lo strumento si arresta con un errore. Lo strumento non sostituisce un file.

Ecco le opzioni di acquisizione:

| Opzione | Funzione | Valore iniziale |
| --- | --- | --- |
| `--channels LIST` | I canali da registrare, per esempio `0,1,2`. | `0` |
| `--samplerate HZ` | La frequenza di campionamento in Hz. | `10000000` |
| `--samples N` | Il numero di campioni per ogni canale. | `1000000` |
| `--vth VOLTS` | La tensione di soglia. | `1.6` |
| `--mode MODE` | `buffer` o `stream`. | `buffer` |
| `--trigger CH[:T]` | Un trigger sul canale CH. Usare `R` (fronte di salita), `F` (fronte di discesa), `C` (fronte di salita o fronte di discesa), `1` (livello alto) o `0` (livello basso) per T. | Nessun trigger. `R` se si indica solo CH. |
| `--trigpos PERCENT` | La posizione del trigger come percentuale dei campioni. | `10` |
| `--timeout SEC` | Il tempo massimo dell'acquisizione in secondi. | `30` |
| `--out PATH` | Il percorso del file di uscita, senza l'estensione `.bin`. | Questa opzione è necessaria. |
| `--log-level N` | La quantità di messaggi della libreria sull'uscita di errore standard, da 0 (nessuno) a 5 (tutti). | `1` |

Lo strumento esamina tutte le opzioni prima di usare il dispositivo. Se un'opzione non è corretta, lo strumento si arresta e dà un errore.

## Il file di uscita

Il file `.bin` contiene i canali nell'ordine dei loro numeri, dal numero più basso. Per ogni canale, il file contiene tutti i campioni di quel canale. Ogni byte contiene 8 campioni. Il primo campione è il bit meno significativo. I dati di ogni canale occupano un numero intero di unità da 8 byte. Per questo ogni canale usa `ceil(samples / 64) × 8` byte.

## Il risultato JSON

Lo strumento scrive un oggetto JSON su una riga. Dopo un'acquisizione corretta, l'oggetto dà il nome del dispositivo, la frequenza di campionamento, il numero di campioni e i canali. Dà anche la tensione di soglia, la modalità, il trigger, il tempo dell'acquisizione e il percorso del file `.bin`. Se l'acquisizione non è corretta, l'oggetto contiene una chiave `error`. In questo caso lo strumento non crea un file `.bin`.

Usare il risultato solo quando lo stato di uscita è 0 e l'oggetto JSON è completo.

## Stato di uscita

| Stato | Significato |
| --- | --- |
| 0 | L'acquisizione è completa. |
| 1 | Si è verificato un errore durante l'operazione, per esempio un errore di I/O. |
| 2 | Un'opzione non è corretta, o un'impostazione non è disponibile sul dispositivo. |
| 3 | L'acquisizione non è stata completata. |

## Opzioni per i programmi che avviano dslcap

- `--parent-fd N`: Lo strumento si arresta quando il programma che lo ha avviato chiude la pipe con il descrittore N. Poi lo strumento rimuove il suo file di uscita se l'acquisizione non è completa.
- `--res DIR`: La cartella con i file del firmware. Di solito lo strumento trova questa cartella automaticamente. È anche possibile impostare la variabile d'ambiente `DSLCAP_RES`.
- `--res-manifest FD`: Lo strumento esamina il valore SHA-256 di ogni file del firmware prima di inviare il file al dispositivo.

Il file `tools/dslcap/README.md` nel codice sorgente dà tutte le informazioni su queste opzioni.
