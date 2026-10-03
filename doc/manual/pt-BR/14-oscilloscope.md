# Modos osciloscópio e aquisição de dados

O Logic Analyze também pode operar os osciloscópios DSCope da DreamSourceLab. Um DSCope tem dois modos de dispositivo:

- **Osciloscópio**: para sinais com um período constante e para uma condição de sinal.
- **Aquisição de Dados**: para sinais lentos durante um tempo longo, por exemplo uma tensão de alimentação ou a saída de um sensor.

Estes modos não estão disponíveis nos dispositivos DSLogic. Este capítulo dá somente os procedimentos principais.

## Conectar o DSCope

> [!WARNING]
> Não conecte as pontas de prova à tensão da rede elétrica. Não conecte as pontas de prova a um circuito que tem uma conexão elétrica com a tensão da rede elétrica. A tensão pode causar lesão ou morte.

> [!CAUTION]
> O terra das pontas de prova, o terra do DSCope e o terra do computador estão conectados entre si. Conecte o terra da ponta de prova somente a um ponto que tem a mesma tensão que o terra do computador. Uma diferença de tensão pode causar dano ao equipamento.

1. Conecte o DSCope ao computador com o cabo USB.
2. Inicie o Logic Analyze. Verifique se a lista de dispositivos mostra o DSCope.
3. Conecte as pontas de prova às entradas do DSCope.
4. Ajuste a chave de atenuação em cada ponta de prova.
5. Conecte a garra de terra de cada ponta de prova ao terra do circuito.
6. Conecte a ponta da ponta de prova ao sinal.

## Opções do dispositivo

Clique em **Opções** › **Opções do dispositivo...** ou pressione `O`.

- **Modo de operação**: **Normal** para medições. **Teste interno** serve somente para testes do dispositivo.
- **Limite de largura de banda**: **Largura de banda total** ou **20MHz**. O limite de 20 MHz diminui o ruído de alta frequência.

## Calibrar o DSCope

O ganho e o offset das entradas mudam com a temperatura e a umidade. Calibre o DSCope para manter as medições precisas.

### Calibração automática

> [!CAUTION]
> Desconecte todas as pontas de prova das entradas antes da calibração. Um sinal em uma entrada durante a calibração dá valores de calibração incorretos.

1. Abra a janela **Opções do dispositivo**.
2. Clique em **Calibração automática**.
3. Desconecte todas as pontas de prova. Clique em **OK**. A calibração continua por alguns minutos.
4. Quando a calibração estiver completa, clique em **Salvar** para manter o resultado.

Para parar a calibração, clique em **Abortar**. O dispositivo então usa os valores de calibração anteriores.

### Calibração manual

1. Abra a janela **Opções do dispositivo**.
2. Clique em **Calibração manual**.
3. Clique em **Iniciar** na barra de ferramentas.
4. Para ajustar o offset, conecte a ponta de prova ao terra. Para ajustar o ganho, conecte a ponta de prova a um sinal com uma tensão conhecida.
5. Defina a escala vertical que você quer calibrar.
6. Mova o controle deslizante **VOFF** ou **VGAIN** do canal até a forma de onda ficar correta.
7. Faça os passos 5 e 6 de novo para cada escala vertical.
8. Clique em **Salvar**.

Para descartar as mudanças, clique em **Abortar**. Para usar as mudanças somente até você desconectar o dispositivo, clique em **Sair**. Para voltar aos valores iniciais, clique em **Redefinir**. Depois de redefinir, faça a calibração automática de novo.

## Configurações do canal

Cada canal tem estes controles à esquerda da área da forma de onda:

- **Ativar**: liga ou desliga o canal.
- **Escala vertical**: a tensão para cada divisão. A janela tem 10 divisões. Para mudar a escala, gire a roda do mouse sobre o botão giratório ou clique na parte de cima ou na parte de baixo do botão giratório. Você também pode pressionar `0` ou `1` para selecionar o botão giratório de um canal e depois pressionar `↑` ou `↓`.
- **Acoplamento**: **CC** ou **CA**.
- **Atenuação da ponta de prova**: defina **x1** ou **x10** de acordo com a chave da ponta de prova.
- **AUTO**: define a escala vertical, a escala horizontal e o nível do gatilho para o sinal que está na entrada.

Para mover a forma de onda de um canal para cima ou para baixo, arraste o rótulo do canal.

## Escala horizontal

Selecione o tempo para cada divisão na lista da barra de ferramentas. Você também pode girar a roda do mouse na área da forma de onda.

## Iniciar e parar

- Clique em **Iniciar** ou pressione `S` para iniciar uma captura contínua. Clique em **Parar** para pará-la.
- Clique em **Única** ou pressione `I` para capturar uma forma de onda e parar.

## Gatilho

Clique em **Gatilho** ou pressione `T` para abrir o painel do gatilho. O painel tem estas configurações:

- **Posição do gatilho**: a posição do ponto do gatilho na captura, como porcentagem.
- **Tempo de holdoff**: o tempo depois de um gatilho em que o dispositivo ignora gatilhos novos. Use-o para obter uma forma de onda estável de grupos de pulsos.
- **Sensibilidade do gatilho**: a mudança de tensão necessária para um gatilho. Um valor maior ignora mais ruído.
- **Fontes de gatilho**: **Auto**, **Canal 0**, **Canal 1**, **Canal 0 && 1** ou **Canal 0 | 1**.
- **Tipos de gatilho**: **Borda de subida** ou **Borda de descida**.

Para definir o nível do gatilho, clique no rótulo do nível do gatilho do canal. Mova o mouse. Clique de novo para definir o nível.

## Medições

### Medições automáticas

A parte de baixo da área da forma de onda tem 10 caixas para medições automáticas.

1. Clique em uma caixa de medição.
2. Selecione o canal.
3. Selecione a medição. Para limpar a caixa, clique em **Redefinir**.

O aplicativo mantém estas configurações para o próximo início.

### Cursores

- Para adicionar um cursor de tempo, clique na régua de tempo. Você também pode clicar com o botão direito do mouse na área da forma de onda e selecionar **Adicionar cursor Y**.
- Para adicionar um cursor de tensão, clique com o botão direito do mouse na área da forma de onda e selecione **Adicionar cursor X**. Cada cursor de tensão tem duas linhas horizontais. O rótulo entre as linhas mostra a diferença de tensão.
- Para medir o tempo entre dois cursores, use o grupo **Distância entre cursores** no painel de medição.

### Medir com o ponteiro

Depois de parar a captura, coloque o ponteiro na forma de onda. O aplicativo mostra a tensão da amostra no ponteiro.

Para medir um tempo, dê um clique duplo em uma área vazia da forma de onda. Clique no segundo ponto. Clique no terceiro ponto para ver a frequência, o período e o ciclo de trabalho. Clique com o botão direito do mouse para cancelar.

## Espectro (FFT)

1. Clique em **Função** › **FFT**.
2. Selecione **Ativar FFT**.
3. Defina **Comprimento da FFT**, **Intervalo de amostragem**, **Fonte da FFT** e **Janela da FFT**.
4. Defina **Modo do eixo Y** e **Faixa DBV**.
5. Clique em **OK**.

O espectro aparece abaixo da forma de onda. Gire a roda do mouse no espectro para mudar o zoom da escala de frequência. Arraste o espectro para movê-lo. Coloque o ponteiro no espectro para ver a frequência e a amplitude.

## Canal matemático

1. Clique em **Função** › **Cálculo**.
2. Selecione **Ativar**.
3. Selecione o **Tipo de operação**: **Somar**, **Subtrair**, **Multiplicar** ou **Dividir**.
4. Selecione a **1ª fonte** e a **2ª fonte**.
5. Clique em **OK**.

## Figura de Lissajous

1. Clique em **Opções** › **Exibir** › **Lissajous**.
2. Selecione **Ativar**.
3. Selecione o canal para o **Eixo X** e para o **Eixo Y**.
4. Clique em **OK**.

## Modo aquisição de dados

1. Na lista de modos de dispositivo da barra de ferramentas, selecione **Aquisição de Dados**.
2. Abra a janela **Opções do dispositivo**.
3. Para cada canal, defina **Ativar**, **Acoplamento** e **Volts/div**.
4. Para mostrar uma outra unidade, defina **Unidade mapeada**, **Mín. mapeado** e **Máx. mapeado**. Por exemplo, mostre a saída de um sensor de temperatura em °C.
5. Clique em **OK**.
6. Selecione a taxa de amostragem e a duração da amostragem na barra de ferramentas.
7. Clique em **Iniciar** ou pressione `S`.

Você não pode mudar as configurações dos canais durante a captura. Na taxa de amostragem mais alta de 10 MHz, a duração máxima da amostragem é de cerca de 10 segundos. A 1 kHz, a captura pode continuar por um dia.

O modo aquisição de dados usa a calibração do modo osciloscópio. Se um canal mostrar um offset, calibre o dispositivo no modo osciloscópio.
