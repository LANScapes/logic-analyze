# Introdução

## Sobre o Logic Analyze

O Logic Analyze é um aplicativo para macOS para os analisadores lógicos da DreamSourceLab. A LANScapes fornece o aplicativo. O aplicativo vem do DSView, que é um programa da DreamSourceLab. O DSView usa software do projeto sigrok.

O aplicativo registra sinais digitais de um analisador lógico DSLogic. Depois, ele mostra os sinais como formas de onda. Você pode medir as formas de onda e decodificar protocolos seriais. Você também pode salvar os dados e exportá-los para outros formatos.

Este manual usa o DSLogic Plus nos exemplos. Os outros modelos DSLogic operam com os mesmos procedimentos. Os limites de canais, de memória e de taxa de amostragem são diferentes.

O aplicativo também contém a ferramenta `dslcap`. Esta ferramenta captura dados sem a janela principal. Consulte [A ferramenta dslcap](13-dslcap.md).

## Sobre este manual

Este manual usa o ASD-STE100 Simplified Technical English. Cada frase é curta. Cada passo de um procedimento dá uma instrução. Cada nome técnico deste manual tem um só sentido. [Nomes e verbos técnicos](15-terms.md) dá a lista dos nomes técnicos.

Este manual usa estes formatos de texto:

- **Texto em negrito** mostra um rótulo do aplicativo, por exemplo um botão, um item de menu ou um campo.
- `Texto de código` mostra uma tecla do teclado, um comando, um nome de arquivo ou um valor que você digita.
- Um caminho pelos menus usa o sinal ›, por exemplo **Arquivo** › **Salvar...**.
- Uma lista de passos com números é um procedimento. Faça os passos na sequência dada.

## Instruções de segurança

Este manual usa estes rótulos nas instruções de segurança:

- **ADVERTÊNCIA** identifica um risco de lesão ou de morte.
- **CUIDADO** identifica um risco de dano ao equipamento ou um risco aos seus dados.
- **NOTA** dá informações que ajudam você. A informação depois de **NOTA** não dá uma instrução.

Uma instrução de segurança vem antes do passo a que ela se aplica. Leia todas as instruções de segurança antes de começar um procedimento.

> [!WARNING]
> Não conecte as pontas de prova à tensão da rede elétrica. Não conecte as pontas de prova a um circuito que tem uma conexão elétrica com a tensão da rede elétrica. As pontas de prova têm uma conexão elétrica com o computador. A tensão pode causar lesão ou morte.

> [!CAUTION]
> Não aplique a uma entrada de canal uma tensão maior que o limite da especificação do dispositivo. Uma tensão muito alta pode causar dano ao analisador lógico.

> [!CAUTION]
> Os fios de terra do analisador lógico estão conectados ao terra do computador pelo cabo USB. Conecte os fios de terra somente ao terra do circuito que você mede. Se os dois terras têm tensões diferentes, uma corrente alta pode passar e causar dano ao circuito, ao analisador lógico e ao computador.

## Requisitos do sistema

Este equipamento é necessário:

- Um Mac com macOS. As notas da versão dão a versão mínima do macOS.
- Uma porta USB. Uma porta USB 3.0 dá a velocidade mais alta. Uma porta USB 2.0 também funciona.
- Um analisador lógico DSLogic, o cabo USB dele e o cabo de pontas de prova dele.

Você pode usar o aplicativo sem um analisador lógico. O dispositivo **Demo** gera sinais de teste. Você também pode abrir um arquivo de dados de uma captura anterior.
