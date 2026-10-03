# A ferramenta dslcap

A ferramenta `dslcap` captura dados de um dispositivo DSLogic sem a janela principal. Use-a em scripts e em testes automáticos. A ferramenta grava as amostras em um arquivo binário. Ela grava um objeto JSON com o resultado na saída padrão.

A ferramenta fica no pacote do aplicativo:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Somente um programa pode usar o dispositivo de cada vez. Feche o Logic Analyze antes de usar o `dslcap`.

## Listar os dispositivos

Para listar os dispositivos que a biblioteca pode encontrar, digite este comando:

```sh
dslcap --list
```

Para listar o identificador USB de cada dispositivo DSLogic conectado, digite este comando:

```sh
dslcap --list-ids
```

O comando `--list-ids` lê somente as informações que o macOS guarda sobre os dispositivos USB. Ele não envia dados ao dispositivo. A saída dá o modelo, a localização USB e um identificador de registro para cada dispositivo.

## Capturar dados

Este comando captura 1000000 amostras nos canais 0 e 1 a 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

A ferramenta grava as amostras em `/tmp/capture.bin`. Se já existe um arquivo com este nome, a ferramenta para com um erro. A ferramenta não substitui um arquivo.

Estas são as opções de captura:

| Opção | Função | Valor inicial |
| --- | --- | --- |
| `--channels LIST` | Os canais a registrar, por exemplo `0,1,2`. | `0` |
| `--samplerate HZ` | A taxa de amostragem em Hz. | `10000000` |
| `--samples N` | O número de amostras para cada canal. | `1000000` |
| `--vth VOLTS` | A tensão de limiar. | `1.6` |
| `--mode MODE` | `buffer` ou `stream`. | `buffer` |
| `--trigger CH[:T]` | Um gatilho no canal CH. Use `R` (borda de subida), `F` (borda de descida), `C` (borda de subida ou borda de descida), `1` (nível alto) ou `0` (nível baixo) para T. | Sem gatilho. `R` se você der somente CH. |
| `--trigpos PERCENT` | A posição do gatilho como porcentagem das amostras. | `10` |
| `--timeout SEC` | O tempo máximo da captura em segundos. | `30` |
| `--out PATH` | O caminho do arquivo de saída, sem a extensão `.bin`. | Esta opção é necessária. |
| `--log-level N` | A quantidade de mensagens da biblioteca na saída de erro padrão, de 0 (nenhuma) a 5 (todas). | `1` |

A ferramenta examina todas as opções antes de usar o dispositivo. Se uma opção não estiver correta, a ferramenta para e dá um erro.

## O arquivo de saída

O arquivo `.bin` contém os canais na sequência dos seus números, a partir do menor número. Para cada canal, o arquivo contém todas as amostras desse canal. Cada byte guarda 8 amostras. A primeira amostra é o bit menos significativo. Os dados de cada canal ocupam um número inteiro de unidades de 8 bytes. Por isso, cada canal usa `ceil(samples / 64) × 8` bytes.

## O resultado JSON

A ferramenta grava um objeto JSON em uma linha. Depois de uma captura correta, o objeto dá o nome do dispositivo, a taxa de amostragem, o número de amostras e os canais. Ele também dá a tensão de limiar, o modo, o gatilho, o tempo da captura e o caminho do arquivo `.bin`. Se a captura não estiver correta, o objeto contém uma chave `error`. Neste caso, a ferramenta não gera um arquivo `.bin`.

Use o resultado somente quando o status de saída for 0 e o objeto JSON estiver completo.

## Status de saída

| Status | Significado |
| --- | --- |
| 0 | A captura está completa. |
| 1 | Ocorreu um erro durante a operação, por exemplo um erro de E/S. |
| 2 | Uma opção não está correta ou uma configuração não está disponível no dispositivo. |
| 3 | A captura não foi completada. |

## Opções para programas que iniciam o dslcap

- `--parent-fd N`: A ferramenta para quando o programa que a iniciou fecha o pipe com o descritor N. Depois, a ferramenta remove o seu arquivo de saída se a captura não estiver completa.
- `--res DIR`: A pasta com os arquivos de firmware. Normalmente, a ferramenta encontra esta pasta automaticamente. Você também pode definir a variável de ambiente `DSLCAP_RES`.
- `--res-manifest FD`: A ferramenta examina o valor SHA-256 de cada arquivo de firmware antes de enviar o arquivo ao dispositivo.

O arquivo `tools/dslcap/README.md` no código-fonte dá todas as informações sobre estas opções.
