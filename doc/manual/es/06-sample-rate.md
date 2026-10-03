# Frecuencia y duración de muestreo

La barra de herramientas tiene dos listas para la longitud de la captura. La lista de arriba es la duración de muestreo. La lista de abajo es la frecuencia de muestreo.

- La **duración de muestreo** es la duración de la captura.
- La **frecuencia de muestreo** es el número de muestras por segundo, para cada canal.

Los valores disponibles cambian según el dispositivo, la conexión USB, el modo de funcionamiento y el modo de canales.

## Duración de muestreo máxima

**Modo Buffer.** La memoria del dispositivo limita la duración de muestreo. Use esta fórmula:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

El DSLogic Plus tiene 256 Mbit de memoria. Estos son dos ejemplos:

- A 100 MHz con 16 canales, la duración de muestreo máxima es de unos 167,77 ms.
- A 400 MHz con 1 canal, la duración de muestreo máxima es de unos 671,09 ms.

**Modo Stream.** La memoria del ordenador limita la duración de muestreo. La aplicación puede guardar 16 G muestras para cada canal. Estos son dos ejemplos:

- A 1 MHz, la duración de muestreo máxima es de unas 4,77 horas.
- A 100 MHz, la duración de muestreo máxima es de unos 2,86 minutos.

## Seleccionar la frecuencia de muestreo

Ajuste la frecuencia de muestreo a un valor de 4 a 10 veces la frecuencia más alta de la señal.

A 4 veces la frecuencia de la señal, la aplicación registra cada flanco. Pero el tiempo de cada flanco tiene un error de hasta el 25 % del período de la señal. A 10 veces la frecuencia de la señal, el error baja al 10 %.

El error de tiempo de un flanco es igual a un período de muestreo o menos. Por ejemplo, a 100 MHz el período de muestreo es de 10 ns. Por eso, el error de cada flanco es de ±10 ns o menos.

![El efecto de la frecuencia de muestreo en la forma de onda registrada](../figures/sample-rate-effect.png)

Estos son valores típicos:

| Señal | Frecuencia de muestreo típica |
| --- | --- |
| UART a 115200 baudios | 2 MHz |
| I2C a 400 kHz | 4 MHz a 10 MHz |
| SPI a 40 MHz | 400 MHz |

## No use una frecuencia de muestreo demasiado alta

Una frecuencia de muestreo más alta da una forma de onda más exacta. Pero una frecuencia de muestreo alta también tiene estos problemas:

1. La aplicación registra más datos por segundo. Por eso, la duración de muestreo máxima disminuye. La aplicación también usa más tiempo para mostrar y decodificar los datos.
2. Una señal lenta puede tener flancos lentos. Con una frecuencia de muestreo alta, la aplicación puede registrar pulsos pequeños en el umbral durante cada flanco lento. Estos pulsos pueden causar errores en los decodificadores.

Si ve pulsos cortos no deseados en señales lentas, reduzca la frecuencia de muestreo. También puede ajustar **Objetivos del filtro** a **1 ciclo de muestreo**. Consulte [Opciones del dispositivo](05-device-options.md).

## Ajustar la frecuencia y la duración de muestreo

1. Ajuste el modo de funcionamiento y el modo de canales. Consulte [Opciones del dispositivo](05-device-options.md).
2. En la lista de abajo de la barra de herramientas, seleccione la frecuencia de muestreo.
3. En la lista de arriba de la barra de herramientas, seleccione la duración de muestreo.

> [!NOTE]
> Cuando cambia el modo de canales, la aplicación puede cambiar la frecuencia de muestreo. Compruebe otra vez la frecuencia de muestreo después de cada cambio de las opciones del dispositivo.
