# Conectar o DSLogic Plus

## Conectar o cabo USB

> [!NOTE]
> Use o cabo USB que veio com o dispositivo ou um cabo USB curto de boa qualidade. Conecte o cabo diretamente a uma porta do computador. Um hub USB ou um cabo longo pode causar erros em uma captura.

1. Conecte o cabo USB ao DSLogic Plus.
2. Conecte a outra ponta do cabo USB a uma porta USB do computador.
3. Verifique se o indicador do DSLogic Plus acende. Antes de o aplicativo iniciar, o indicador fica vermelho.
4. Inicie o Logic Analyze.
5. Verifique se o indicador muda para verde.
6. Verifique se a lista de dispositivos na barra de ferramentas mostra **DSLogic Plus**.

![A conexão USB](../figures/usb-connection.png)

Se a lista de dispositivos não mostrar o dispositivo, faça estes passos:

1. Desconecte o cabo USB do computador.
2. Espere 5 segundos.
3. Conecte o cabo USB a uma outra porta USB.
4. Se a lista de dispositivos não mostrar o dispositivo depois do passo 3, feche o aplicativo e inicie-o de novo.

> [!NOTE]
> Somente um programa pode usar o dispositivo de cada vez. Se o `dslcap` ou um outro programa usa o dispositivo, o aplicativo não o encontra.

## Conectar o cabo de pontas de prova

O cabo de pontas de prova tem 16 fios de canal. Cada fio de canal tem uma blindagem, uma ponta de sinal e uma ponta de terra. As cores dos fios identificam os canais 0 a 15. Um fio a mais tem estes sinais:

- **CK**: A entrada para um clock externo. Use-a somente com a configuração **Usar clock externo**.
- **TI**: A entrada para um sinal de gatilho externo.
- **TO**: A saída do sinal de gatilho. O dispositivo envia um pulso em TO quando o gatilho ocorre.

Normalmente, você não conecta os fios CK, TI e TO.

![O cabo de pontas de prova e os seus canais](../figures/probe-cable-channels.png)

1. Conecte o cabo de pontas de prova ao conector de entrada do DSLogic Plus.
2. Empurre o conector totalmente para dentro do dispositivo.

## Conectar os canais ao circuito

> [!WARNING]
> Não conecte as pontas de prova à tensão da rede elétrica. Não conecte as pontas de prova a um circuito que tem uma conexão elétrica com a tensão da rede elétrica. A tensão pode causar lesão ou morte.

> [!CAUTION]
> Antes de conectar um fio de terra, verifique se o terra do circuito e o terra do computador têm a mesma tensão. Uma diferença de tensão pode causar uma corrente alta que pode causar dano ao equipamento.

1. Desligue a alimentação do circuito que você mede.
2. Conecte pelo menos um fio de terra ao terra do circuito.
3. Conecte cada fio de canal que você usa a um sinal do circuito.
4. Verifique se nenhuma ponta de prova toca em um outro contato.
5. Ligue a alimentação do circuito.

![Conexões de terra: um terra comum (à esquerda) ou um terra para cada canal (à direita)](../figures/probe-grounding.png)

Para sinais com uma frequência menor que 5 MHz, um fio de terra para todos os canais é suficiente. Para sinais com uma frequência mais alta, conecte a ponta de terra de cada fio de canal ao terra perto do seu sinal. Conexões de terra curtas dão bordas de sinal limpas.

## Desconectar o DSLogic Plus

> [!CAUTION]
> Não desconecte o cabo USB durante uma captura. Se você o desconectar, os dados da captura podem ter erros.

1. Pare a captura. Clique em **Parar** se este botão aparecer na barra de ferramentas.
2. Desligue a alimentação do circuito.
3. Desconecte as pontas de prova do circuito.
4. Desconecte o cabo USB.
