# Opciones del dispositivo

## Abrir las opciones del dispositivo

1. Haga clic en **Opciones** › **Opciones del dispositivo...** en la barra de herramientas. También puede pulsar `O`.
2. Cambie los ajustes en la ventana **Opciones del dispositivo**.
3. Haga clic en **OK**.

Los ajustes de la ventana son diferentes para cada modelo de dispositivo. Este capítulo da los ajustes del DSLogic Plus.

> [!NOTE]
> No puede cambiar las opciones del dispositivo durante una captura.

![La ventana Opciones del dispositivo](../figures/device-options.png)
<!-- TODO: new screenshot -->

## Modo de funcionamiento

El ajuste **Modo de funcionamiento** selecciona cómo el dispositivo envía los datos al ordenador.

**Modo Buffer.** El dispositivo guarda las muestras en su memoria interna durante la captura. Después de la captura, el dispositivo envía los datos al ordenador por USB. La memoria es más rápida que el USB. Por eso, el modo Buffer da las frecuencias de muestreo más altas. La capacidad de la memoria limita la longitud de la captura. Use el modo Buffer para señales rápidas y capturas cortas.

**Modo Stream.** El dispositivo envía las muestras al ordenador durante la captura. La memoria del ordenador limita la longitud de la captura. Puede ver los datos durante la captura. La velocidad de la conexión USB limita la frecuencia de muestreo. Use el modo Stream para señales lentas y capturas largas.

**Prueba interna.** Este modo es solo para pruebas del dispositivo. No lo use para mediciones.

## Opciones de parada

El ajuste **Opciones de parada** se aplica solo al modo Buffer. Define cómo funciona la aplicación cuando detiene una captura antes del final.

- **Detener inmediatamente**: La aplicación no recibe los datos del dispositivo. La aplicación no muestra datos.
- **Cargar datos capturados**: La aplicación recibe los datos que el dispositivo registró antes de la parada. La aplicación muestra estos datos.

## Nivel de umbral

El ajuste **Nivel de umbral** es la tensión que separa un nivel bajo de un nivel alto. Una señal por encima del umbral es un nivel alto. Una señal por debajo del umbral es un nivel bajo.

Puede ajustar un valor de 0,0 V a 5,0 V en pasos de 0,1 V. Ajuste el umbral a aproximadamente el 50 % de la tensión lógica del circuito. Para un circuito de 3,3 V, ajuste aproximadamente 1,6 V.

## Objetivos del filtro

El ajuste **Objetivos del filtro** elimina los pulsos cortos de los datos.

- **Ninguno**: La aplicación muestra todas las muestras.
- **1 ciclo de muestreo**: La aplicación elimina cada pulso más corto que un período de muestreo.

## Altura máxima

El ajuste **Altura máxima** define la altura máxima de cada fila de canal en el área de formas de onda. **1X** es una unidad de altura. Use un valor más grande cuando muestra solo un número pequeño de canales.

## Activar compresión RLE

Cuando selecciona **Activar compresión RLE**, el dispositivo comprime los datos en su memoria (codificación por longitud de series). Este ajuste se aplica solo al modo Buffer. Si las señales tienen pocos flancos, el dispositivo puede guardar una captura más larga en su memoria. Si las señales tienen muchos flancos, la compresión no aumenta la longitud.

## Usar reloj externo

Cuando selecciona **Usar reloj externo**, el dispositivo muestrea los canales en cada flanco de reloj del cable CK. El dispositivo no usa su reloj interno. Use este ajuste para registrar un bus que tiene una señal de reloj.

## Usar flanco descendente del reloj

Este ajuste se aplica solo con **Usar reloj externo**. Normalmente, el dispositivo muestrea los canales en el flanco ascendente del reloj. Cuando selecciona **Usar flanco descendente del reloj**, el dispositivo muestrea los canales en el flanco descendente del reloj.

## Modo de canales

El modo de canales define el número de canales que el dispositivo puede usar. También define la frecuencia de muestreo máxima. Un número menor de canales da una frecuencia de muestreo máxima más alta. Seleccione el modo de canales que corresponde al número y a la frecuencia de sus señales.

Para el DSLogic Plus, los modos de canales son:

| Modo de funcionamiento | Modo de canales | Frecuencia de muestreo máxima |
| --- | --- | --- |
| Modo Buffer | Canales 0 a 15 | 100 MHz |
| Modo Buffer | Canales 0 a 7 | 200 MHz |
| Modo Buffer | Canales 0 a 3 | 400 MHz |
| Modo Stream | 16 canales | 20 MHz |
| Modo Stream | 12 canales | 25 MHz |
| Modo Stream | 6 canales | 50 MHz |
| Modo Stream | 3 canales | 100 MHz |

## Activar y desactivar canales

Debajo de los modos de canales, la ventana muestra una casilla para cada canal.

1. Marque la casilla de cada canal que usa.
2. Desmarque la casilla de cada canal que no usa.
3. Para marcar todos los canales, haga clic en **Marcar todos**. Para desmarcar todos los canales, haga clic en **Desmarcar todos**.

En modo Stream, un número menor de canales activados puede permitirle usar una frecuencia de muestreo más alta.
