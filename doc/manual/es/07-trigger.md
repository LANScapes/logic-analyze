# Disparos

Un disparo es una condición en las señales. Cuando ocurre la condición, el dispositivo marca ese instante como punto de disparo. El disparo le permite capturar la parte de la señal que quiere examinar.

La aplicación tiene dos tipos de disparo:

- **Disparo simple**: Un flanco o un nivel en uno o más canales.
- **Disparo avanzado**: Una secuencia de condiciones o un valor en un bus serie.

Para abrir el panel de disparo, haga clic en **Disparo** en la barra de herramientas o pulse `T`.

> [!NOTE]
> Si la señal no cumple la condición de disparo, la captura sigue esperando. Para ver la señal sin el disparo, haga clic en **Inmediato**. Para detener la espera, haga clic en **Detener**.

## Posición del disparo

El ajuste **Posición del disparo** define dónde está el punto de disparo en la captura. El valor es un porcentaje de la duración de muestreo.

- Un valor pequeño, por ejemplo el 10 %, muestra más parte de la señal después del disparo.
- Un valor grande, por ejemplo el 90 %, muestra más parte de la señal antes del disparo.

La posición del disparo usa la memoria del dispositivo. Por eso, solo puede ajustarla en modo Buffer. En modo Stream, la posición del disparo es siempre de aproximadamente el 1 %.

![Posición del disparo del 10 % (izquierda) y del 90 % (derecha)](../figures/trigger-position.png)
<!-- TODO: new screenshot -->

## Disparo simple

Cada etiqueta de canal del área de formas de onda tiene cinco botones de disparo. De izquierda a derecha, los botones son:

1. Flanco ascendente
2. Nivel alto
3. Flanco descendente
4. Nivel bajo
5. Flanco ascendente o flanco descendente

![Los botones de disparo de una etiqueta de canal](../figures/simple-trigger-buttons.png)

Para ajustar un disparo simple, haga estos pasos:

1. Abra el panel de disparo.
2. Seleccione **Disparo simple**.
3. En la etiqueta de un canal, haga clic en el botón de disparo que quiere. El botón cambia de color.
4. Para quitar el disparo de un canal, haga clic otra vez en el mismo botón.
5. Ajuste la **Posición del disparo**.

Si ajusta un disparo en más de un canal, todas las condiciones deben ocurrir en la misma muestra (Y lógico).

## Disparo avanzado

> [!NOTE]
> El disparo avanzado solo está disponible en modo Buffer. Para usarlo, ajuste **Modo de funcionamiento** a **Modo Buffer**. Consulte [Opciones del dispositivo](05-device-options.md).

Para usar el disparo avanzado, seleccione **Disparo avanzado** en el panel de disparo. Después, seleccione la pestaña **Disparo por etapas** o la pestaña **Disparo serie**.

### Valores para cada canal

El disparo por etapas y el disparo serie usan una fila de 16 caracteres. Cada carácter es la condición de un canal. El carácter de la derecha es el canal 0. El carácter de la izquierda es el canal 15.

| Carácter | Condición |
| --- | --- |
| `X` | Todos los valores (el canal no tiene efecto). |
| `0` | Nivel bajo. |
| `1` | Nivel alto. |
| `R` | Flanco ascendente. |
| `F` | Flanco descendente. |
| `C` | Flanco ascendente o flanco descendente. |

### Disparo por etapas

Un disparo por etapas es una secuencia de condiciones. Cada condición es una etapa. El dispositivo examina primero la etapa 0. Cuando ocurre la condición de una etapa, el dispositivo pasa a la etapa siguiente. El disparo ocurre cuando la última etapa está completa. Puede usar hasta 16 etapas.

Cada etapa tiene estos ajustes:

- Dos filas de condiciones de canal.
- Para cada fila, `==` o `!=`. Con `==`, la condición ocurre cuando los canales coinciden con la fila. Con `!=`, la condición ocurre cuando los canales no coinciden con la fila.
- **Y** u **O**. Este ajuste une las dos filas.
- **Contador**: El número de veces que la condición debe ocurrir antes de completar la etapa.
- **Contiguo**: Cuando marca esta casilla, la condición debe ocurrir en muestras seguidas, sin interrupción.

![Los ajustes del disparo por etapas](../figures/stage-trigger-panel.png)
<!-- TODO: new screenshot -->

Para ajustar un disparo por etapas, haga estos pasos:

1. En **Etapas de disparo totales**, seleccione el número de etapas.
2. En la lista de etapas de la derecha, haga clic en la etapa 0.
3. Escriba las condiciones de canal en la primera fila.
4. Si es necesario, escriba las condiciones de canal en la segunda fila y seleccione **Y** u **O**.
5. Escriba un valor en **Contador**.
6. Repita los pasos 2 a 5 para cada una de las otras etapas.

Estos son tres ejemplos.

**Ejemplo 1.** Disparar cuando el canal 0 está en nivel alto durante más de 1000 muestras:

1. Ajuste **Etapas de disparo totales** a 1.
2. En la etapa 0, escriba `1` para el canal 0 en la primera fila.
3. Marque **Contiguo**.
4. Ajuste **Contador** a 1000.

![Ejemplo 1](../figures/stage-example-level-count.png)

**Ejemplo 2.** Disparar con un flanco ascendente en el canal 0 o un flanco descendente en el canal 1:

1. Ajuste **Etapas de disparo totales** a 1.
2. En la etapa 0, escriba `R` para el canal 0 en la primera fila.
3. Escriba `F` para el canal 1 en la segunda fila.
4. Seleccione **O**.

![Ejemplo 2](../figures/stage-example-or.png)

**Ejemplo 3.** Disparar con un flanco ascendente en el canal 0, después 100 flancos descendentes en el canal 1 y después un nivel alto en el canal 2:

1. Ajuste **Etapas de disparo totales** a 3.
2. En la etapa 0, escriba `R` para el canal 0.
3. En la etapa 1, escriba `F` para el canal 1. Ajuste **Contador** a 100.
4. En la etapa 2, escriba `1` para el canal 2.

![Ejemplo 3](../figures/stage-example-sequence.png)

### Disparo serie

Un disparo serie encuentra un valor de datos en un bus serie. Funciona como un registro de desplazamiento. Estos son los ajustes:

- **Indicador de inicio**: La condición que inicia el disparo serie.
- **Indicador de parada**: La condición que borra el registro de desplazamiento.
- **Indicador de reloj**: La condición que añade un bit al registro de desplazamiento.
- **Canal de datos**: El canal que transmite los datos.
- **Bits de datos**: El número de bits del valor.
- **Valor de datos**: El valor que causa el disparo.

Después del indicador de inicio, el dispositivo lee el canal de datos en cada indicador de reloj. El dispositivo pasa este bit al registro de desplazamiento. Cuando los últimos bits del registro de desplazamiento son iguales a **Valor de datos**, ocurre el disparo. Cuando ocurre el indicador de parada, el dispositivo borra el registro de desplazamiento.

![Los ajustes del disparo serie](../figures/serial-trigger-panel.png)
<!-- TODO: new screenshot -->

**Ejemplo 4.** Disparar cuando el valor `010000100` aparece en un bus I2C. El canal 0 es SCL y el canal 1 es SDA.

1. Ajuste **Indicador de inicio** a un flanco descendente en SDA mientras SCL está en nivel alto: `F1` en los dos caracteres de la derecha.
2. Ajuste **Indicador de parada** a un flanco ascendente en SDA mientras SCL está en nivel alto: `R1`.
3. Ajuste **Indicador de reloj** a un flanco ascendente en SCL: `R` para el canal 0.
4. Ajuste **Canal de datos** a 1.
5. Ajuste **Bits de datos** a 9.
6. Escriba `010000100` en **Valor de datos**.

![Ejemplo 4](../figures/serial-example-i2c.png)

**Ejemplo 5.** Disparar cuando el valor `0x1234` aparece en MOSI de un bus SPI. El canal 0 es CS#, el canal 1 es CLK, el canal 2 es MISO y el canal 3 es MOSI.

1. Ajuste **Indicador de inicio** a un flanco descendente en CS#: `F` para el canal 0.
2. Ajuste **Indicador de parada** a un flanco ascendente en CS#: `R` para el canal 0.
3. Ajuste **Indicador de reloj** a un flanco ascendente en CLK: `R` para el canal 1.
4. Ajuste **Canal de datos** a 3.
5. Ajuste **Bits de datos** a 16.
6. Escriba `0001001000110100` en **Valor de datos**.

![Ejemplo 5](../figures/serial-example-spi.png)

Para escribir el valor en hexadecimal, marque **Introducir en formato hexadecimal**. Después, escriba el valor en el campo **Hex**.
