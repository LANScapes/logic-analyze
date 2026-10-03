# Taxa de amostragem e duração da amostragem

A barra de ferramentas tem duas listas para o tamanho da captura. A lista de cima é a duração da amostragem. A lista de baixo é a taxa de amostragem.

- A **duração da amostragem** é o tempo da captura.
- A **taxa de amostragem** é o número de amostras em cada segundo, para cada canal.

Os valores disponíveis mudam com o dispositivo, a conexão USB, o modo de operação e o modo de canais.

## Duração máxima da amostragem

**Modo buffer.** A memória do dispositivo limita a duração da amostragem. Use esta fórmula:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

O DSLogic Plus tem 256 Mbit de memória. Estes são dois exemplos:

- A 100 MHz com 16 canais, a duração máxima da amostragem é de cerca de 167,77 ms.
- A 400 MHz com 1 canal, a duração máxima da amostragem é de cerca de 671,09 ms.

**Modo stream.** A memória do computador limita a duração da amostragem. O aplicativo pode guardar 16 G amostras para cada canal. Estes são dois exemplos:

- A 1 MHz, a duração máxima da amostragem é de cerca de 4,77 horas.
- A 100 MHz, a duração máxima da amostragem é de cerca de 2,86 minutos.

## Selecionar a taxa de amostragem

Defina a taxa de amostragem em 4 a 10 vezes a frequência mais alta do sinal.

A 4 vezes a frequência do sinal, o aplicativo registra cada borda. Mas o tempo de cada borda tem um erro de até 25% do período do sinal. A 10 vezes a frequência do sinal, o erro diminui para 10%.

O erro de tempo de uma borda é igual a um período de amostragem ou menor. Por exemplo, a 100 MHz o período de amostragem é de 10 ns. Por isso, o erro de cada borda é de ±10 ns ou menor.

![O efeito da taxa de amostragem na forma de onda registrada](../figures/sample-rate-effect.png)

Estes são valores típicos:

| Sinal | Taxa de amostragem típica |
| --- | --- |
| UART a 115200 baud | 2 MHz |
| I2C a 400 kHz | 4 MHz a 10 MHz |
| SPI a 40 MHz | 400 MHz |

## Não use uma taxa de amostragem alta demais

Uma taxa de amostragem mais alta dá uma forma de onda mais precisa. Mas uma taxa de amostragem alta também tem estes problemas:

1. O aplicativo registra mais dados em cada segundo. Por isso, a duração máxima da amostragem diminui. O aplicativo também usa mais tempo para mostrar e decodificar os dados.
2. Um sinal lento pode ter bordas lentas. A uma taxa de amostragem alta, o aplicativo pode registrar pulsos pequenos no limiar durante cada borda lenta. Esses pulsos podem causar erros nos decodificadores.

Se você vir pulsos curtos indesejados em sinais lentos, diminua a taxa de amostragem. Você também pode definir **Alvos do filtro** como **1 ciclo de amostragem**. Consulte [Opções do dispositivo](05-device-options.md).

## Definir a taxa de amostragem e a duração

1. Defina o modo de operação e o modo de canais. Consulte [Opções do dispositivo](05-device-options.md).
2. Na lista de baixo da barra de ferramentas, selecione a taxa de amostragem.
3. Na lista de cima da barra de ferramentas, selecione a duração da amostragem.

> [!NOTE]
> Quando você muda o modo de canais, o aplicativo pode mudar a taxa de amostragem. Verifique a taxa de amostragem de novo depois de cada mudança nas opções do dispositivo.
