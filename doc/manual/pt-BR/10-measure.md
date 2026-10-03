# Medições

Você pode medir a forma de onda com o mouse ou com cursores. Um cursor é uma linha vertical em um tempo da captura.

## Medir um pulso com o ponteiro

Coloque o ponteiro em um pulso de um canal. Uma caixa perto do ponteiro mostra estes valores do pulso:

- **Largura**: o tempo do pulso.
- **Período**: o tempo de uma borda até a borda seguinte da mesma direção.
- **Frequência**: 1 dividido pelo período.
- **Ciclo de trabalho**: o tempo em nível alto como porcentagem do período.

![A medição no ponteiro](../figures/hover-measurement.png)

Para ligar ou desligar esta caixa, abra o painel de medição e use **Ativar medição flutuante**.

## Contar as bordas em uma área

1. Coloque o ponteiro na forma de onda do canal, entre o nível alto e o nível baixo.
2. Mova o ponteiro para o início da área.
3. Clique com o botão esquerdo do mouse.
4. Mova o ponteiro para o fim da área. O aplicativo mostra o número de bordas, de bordas de subida e de bordas de descida.
5. Clique com o botão esquerdo do mouse de novo para completar a medição.

## Medir o tempo entre duas bordas

1. Coloque o ponteiro na primeira borda.
2. Clique com o botão esquerdo do mouse.
3. Mova o ponteiro para a segunda borda. O aplicativo mostra o tempo e o número de amostras entre as duas bordas.
4. Clique com o botão esquerdo do mouse de novo para completar a medição.

![O tempo entre duas bordas](../figures/edge-distance.png)

## Adicionar um cursor

Use um destes métodos:

- Na área da forma de onda, dê um clique duplo com o botão esquerdo do mouse no tempo que você quer. Se o ponteiro estiver perto de uma borda, o cursor vai para a borda.
- Na régua de tempo, clique com o botão esquerdo do mouse. Uma seta aparece na régua. Clique na seta para adicionar um cursor.

![Adicionar um cursor pela régua de tempo](../figures/ruler-insert-cursor.png)

Cada cursor tem um número. Os números começam em 1.

## Mover um cursor

Use um destes métodos:

- Coloque o ponteiro no cursor. A linha do cursor fica mais grossa. Clique no cursor. Mova o mouse. Clique de novo para soltar o cursor. Perto de uma borda, o cursor vai para a borda.
- Na régua de tempo, clique com o botão esquerdo do mouse no tempo novo. A régua mostra os números de todos os cursores. Clique no número do cursor que você quer mover.

![Mover um cursor pela régua de tempo](../figures/ruler-move-cursor.png)

## Ir para um cursor

1. Na régua de tempo, clique com o botão direito do mouse. A régua mostra os números de todos os cursores.
2. Clique no número de um cursor. A forma de onda vai para a posição desse cursor.

![Ir para o cursor 3](../figures/ruler-jump-cursor.png)

## Medir com cursores

Para abrir o painel de medição, clique em **Medir** na barra de ferramentas ou pressione `M`. O painel tem estes grupos:

- **Distância entre cursores**: o tempo e o número de amostras entre dois cursores.
- **Bordas**: o número de bordas em um canal entre dois cursores.
- **Cursores**: o tempo e o número da amostra de cada cursor.

Para adicionar uma medição de tempo, faça estes passos:

1. No grupo **Distância entre cursores**, clique no botão **+**.
2. Clique no campo de início e selecione o primeiro cursor.
3. Clique no campo de fim e selecione o segundo cursor.

O painel mostra o resultado na coluna **Tempo/Amostras**.

Para adicionar uma contagem de bordas, faça estes passos:

1. No grupo **Bordas**, clique no botão **+**.
2. Selecione o cursor de início e o cursor de fim.
3. Selecione o canal.

O painel mostra o número de bordas de subida, de bordas de descida e de todas as bordas.

Para remover uma medição, clique no botão **×** da sua linha.

## Excluir um cursor

Use um destes métodos:

- Clique no **×** do rótulo do cursor na régua de tempo.
- Clique no botão **×** do cursor no grupo **Cursores** do painel de medição.

O aplicativo dá números novos aos cursores que ficam.
