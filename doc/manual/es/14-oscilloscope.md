# Modos osciloscopio y adquisición de datos

Logic Analyze también puede usar los osciloscopios DSCope de DreamSourceLab. Un DSCope tiene dos modos de dispositivo:

- **Osciloscopio**: para señales con un período constante y para una condición de señal.
- **Adquisición de datos**: para señales lentas durante mucho tiempo, por ejemplo una tensión de alimentación o la salida de un sensor.

Estos modos no están disponibles en los dispositivos DSLogic. Este capítulo da solo los procedimientos principales.

## Conectar el DSCope

> [!WARNING]
> No conecte las sondas a la tensión de red. No conecte las sondas a un circuito que tiene una conexión eléctrica con la tensión de red. La tensión puede causar lesiones o la muerte.

> [!CAUTION]
> La masa de las sondas, la masa del DSCope y la masa del ordenador están conectadas entre sí. Conecte la masa de la sonda solo a un punto que tiene la misma tensión que la masa del ordenador. Una diferencia de tensión puede dañar el equipo.

1. Conecte el DSCope al ordenador con el cable USB.
2. Inicie Logic Analyze. Asegúrese de que la lista de dispositivos muestra el DSCope.
3. Conecte las sondas a las entradas del DSCope.
4. Ajuste el conmutador de atenuación de cada sonda.
5. Conecte la pinza de masa de cada sonda a la masa del circuito.
6. Conecte la punta de la sonda a la señal.

## Opciones del dispositivo

Haga clic en **Opciones** › **Opciones del dispositivo...** o pulse `O`.

- **Modo de funcionamiento**: **Normal** para mediciones. **Prueba interna** es solo para pruebas del dispositivo.
- **Límite de ancho de banda**: **Ancho de banda completo** o **20MHz**. El límite de 20 MHz reduce el ruido de alta frecuencia.

## Calibrar el DSCope

La ganancia y el desplazamiento de las entradas cambian con la temperatura y la humedad. Calibre el DSCope para mantener mediciones exactas.

### Calibración automática

> [!CAUTION]
> Desconecte todas las sondas de las entradas antes de la calibración. Una señal en una entrada durante la calibración da valores de calibración incorrectos.

1. Abra la ventana **Opciones del dispositivo**.
2. Haga clic en **Calibración automática**.
3. Desconecte todas las sondas. Haga clic en **OK**. La calibración dura unos minutos.
4. Cuando la calibración está completa, haga clic en **Guardar** para conservar el resultado.

Para detener la calibración, haga clic en **Anular**. El dispositivo usa entonces los valores de calibración anteriores.

### Calibración manual

1. Abra la ventana **Opciones del dispositivo**.
2. Haga clic en **Calibración manual**.
3. Haga clic en **Iniciar** en la barra de herramientas.
4. Para ajustar el desplazamiento, conecte la sonda a masa. Para ajustar la ganancia, conecte la sonda a una señal de tensión conocida.
5. Ajuste la escala vertical que quiere calibrar.
6. Mueva el control deslizante **VOFF** o **VGAIN** del canal hasta que la forma de onda sea correcta.
7. Repita los pasos 5 y 6 para cada escala vertical.
8. Haga clic en **Guardar**.

Para descartar los cambios, haga clic en **Anular**. Para usar los cambios solo hasta desconectar el dispositivo, haga clic en **Salir**. Para volver a los valores iniciales, haga clic en **Restablecer**. Después de restablecer, haga otra vez la calibración automática.

## Ajustes de canal

Cada canal tiene estos controles a la izquierda del área de formas de onda:

- **Activar**: activa o desactiva el canal.
- **Escala vertical**: la tensión por división. La ventana tiene 10 divisiones. Para cambiar la escala, gire la rueda del ratón sobre el mando, o haga clic en la parte de arriba o de abajo del mando. También puede pulsar `0` o `1` para seleccionar el mando de un canal y después pulsar `↑` o `↓`.
- **Acoplamiento**: **CC** o **CA**.
- **Atenuación de la sonda**: ajuste **x1** o **x10** según el conmutador de la sonda.
- **AUTO**: ajusta la escala vertical, la escala horizontal y el nivel de disparo para la señal que está en la entrada.

Para mover la forma de onda de un canal hacia arriba o hacia abajo, arrastre la etiqueta del canal.

## Escala horizontal

Seleccione el tiempo por división en la lista de la barra de herramientas. También puede girar la rueda del ratón en el área de formas de onda.

## Iniciar y detener

- Haga clic en **Iniciar** o pulse `S` para iniciar una captura continua. Haga clic en **Detener** para detenerla.
- Haga clic en **Único** o pulse `I` para capturar una forma de onda y detener.

## Disparo

Haga clic en **Disparo** o pulse `T` para abrir el panel de disparo. El panel tiene estos ajustes:

- **Posición del disparo**: la posición del punto de disparo en la captura, como porcentaje.
- **Tiempo de retención**: el tiempo después de un disparo durante el cual el dispositivo ignora los disparos nuevos. Úselo para obtener una forma de onda estable de grupos de pulsos.
- **Sensibilidad del disparo**: el cambio de tensión necesario para un disparo. Un valor mayor ignora más ruido.
- **Fuentes de disparo**: **Auto**, **Canal 0**, **Canal 1**, **Canal 0 && 1** o **Canal 0 | 1**.
- **Tipos de disparo**: **Flanco ascendente** o **Flanco descendente**.

Para ajustar el nivel de disparo, haga clic en la etiqueta de nivel de disparo del canal. Mueva el ratón. Haga clic otra vez para fijar el nivel.

## Mediciones

### Mediciones automáticas

La parte de abajo del área de formas de onda tiene 10 recuadros para mediciones automáticas.

1. Haga clic en un recuadro de medición.
2. Seleccione el canal.
3. Seleccione la medición. Para vaciar el recuadro, haga clic en **Restablecer**.

La aplicación conserva estos ajustes para el siguiente inicio.

### Cursores

- Para añadir un cursor de tiempo, haga clic en la regla de tiempo. También puede hacer clic con el botón derecho del ratón en el área de formas de onda y seleccionar **Añadir cursor Y**.
- Para añadir un cursor de tensión, haga clic con el botón derecho del ratón en el área de formas de onda y seleccione **Añadir cursor X**. Cada cursor de tensión tiene dos líneas horizontales. La etiqueta entre las líneas muestra la diferencia de tensión.
- Para medir el tiempo entre dos cursores, use el grupo **Distancia entre cursores** del panel de medición.

### Medir con el puntero

Después de detener la captura, coloque el puntero sobre la forma de onda. La aplicación muestra la tensión de la muestra en el puntero.

Para medir un tiempo, haga doble clic en una zona vacía de la forma de onda. Haga clic en el segundo punto. Haga clic en el tercer punto para ver la frecuencia, el período y el ciclo de trabajo. Haga clic con el botón derecho del ratón para cancelar.

## Espectro (FFT)

1. Haga clic en **Función** › **FFT**.
2. Marque **Activar FFT**.
3. Ajuste **Longitud de FFT**, **Intervalo de muestreo**, **Fuente de FFT** y **Ventana de FFT**.
4. Ajuste **Modo del eje Y** y **Rango DBV**.
5. Haga clic en **OK**.

El espectro aparece debajo de la forma de onda. Gire la rueda del ratón en el espectro para hacer zoom en la escala de frecuencias. Arrastre el espectro para moverlo. Coloque el puntero sobre el espectro para ver la frecuencia y la amplitud.

## Canal matemático

1. Haga clic en **Función** › **Cálculo**.
2. Marque **Activar**.
3. Seleccione el **Tipo de operación**: **Sumar**, **Restar**, **Multiplicar** o **Dividir**.
4. Seleccione la **1.ª fuente** y la **2.ª fuente**.
5. Haga clic en **OK**.

## Figura de Lissajous

1. Haga clic en **Opciones** › **Pantalla** › **Lissajous**.
2. Marque **Activar**.
3. Seleccione el canal para el **Eje X** y el **Eje Y**.
4. Haga clic en **OK**.

## Modo adquisición de datos

1. En la lista de modos de dispositivo de la barra de herramientas, seleccione **Adquisición de datos**.
2. Abra la ventana **Opciones del dispositivo**.
3. Para cada canal, ajuste **Activar**, **Acoplamiento** y **Voltios/div**.
4. Para mostrar otra unidad, ajuste **Unidad de escala**, **Mínimo de escala** y **Máximo de escala**. Por ejemplo, muestre la salida de un sensor de temperatura en °C.
5. Haga clic en **OK**.
6. Seleccione la frecuencia y la duración de muestreo en la barra de herramientas.
7. Haga clic en **Iniciar** o pulse `S`.

No puede cambiar los ajustes de canal durante la captura. Con la frecuencia de muestreo más alta de 10 MHz, la duración de muestreo máxima es de unos 10 segundos. A 1 kHz, la captura puede continuar un día.

El modo adquisición de datos usa la calibración del modo osciloscopio. Si un canal muestra un desplazamiento, calibre el dispositivo en modo osciloscopio.
