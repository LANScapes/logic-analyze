# Conectar el DSLogic Plus

## Conectar el cable USB

> [!NOTE]
> Use el cable USB que viene con el dispositivo o un cable USB corto de buena calidad. Conecte el cable directamente a un puerto del ordenador. Un concentrador USB o un cable largo puede causar errores en una captura.

1. Conecte el cable USB al DSLogic Plus.
2. Conecte el otro extremo del cable USB a un puerto USB del ordenador.
3. Asegúrese de que el indicador del DSLogic Plus se enciende. Antes del inicio de la aplicación, el indicador está rojo.
4. Inicie Logic Analyze.
5. Asegúrese de que el indicador cambia a verde.
6. Asegúrese de que la lista de dispositivos de la barra de herramientas muestra **DSLogic Plus**.

![La conexión USB](../figures/usb-connection.png)

Si la lista de dispositivos no muestra el dispositivo, haga estos pasos:

1. Desconecte el cable USB del ordenador.
2. Espere 5 segundos.
3. Conecte el cable USB a otro puerto USB.
4. Si la lista de dispositivos no muestra el dispositivo después del paso 3, salga de la aplicación e iníciela otra vez.

> [!NOTE]
> Solo un programa a la vez puede usar el dispositivo. Si `dslcap` u otro programa usa el dispositivo, la aplicación no lo encuentra.

## Conectar el cable de sondas

El cable de sondas tiene 16 cables de canal. Cada cable de canal tiene un blindaje, un extremo de señal y un extremo de masa. Los colores de los cables identifican los canales 0 a 15. Un cable más tiene estas señales:

- **CK**: La entrada de un reloj externo. Úsela solo con el ajuste **Usar reloj externo**.
- **TI**: La entrada de una señal de disparo externa.
- **TO**: La salida de la señal de disparo. El dispositivo envía un pulso por TO cuando ocurre el disparo.

Normalmente, no es necesario conectar los cables CK, TI y TO.

![El cable de sondas y sus canales](../figures/probe-cable-channels.png)

1. Conecte el cable de sondas al conector de entrada del DSLogic Plus.
2. Empuje el conector completamente dentro del dispositivo.

## Conectar los canales al circuito

> [!WARNING]
> No conecte las sondas a la tensión de red. No conecte las sondas a un circuito que tiene una conexión eléctrica con la tensión de red. La tensión puede causar lesiones o la muerte.

> [!CAUTION]
> Antes de conectar un cable de masa, asegúrese de que la masa del circuito y la masa del ordenador tienen la misma tensión. Una diferencia de tensión puede causar una corriente alta que puede dañar el equipo.

1. Desconecte la alimentación del circuito que mide.
2. Conecte al menos un cable de masa a la masa del circuito.
3. Conecte cada cable de canal que usa a una señal del circuito.
4. Asegúrese de que ninguna sonda toca otro contacto.
5. Conecte la alimentación del circuito.

![Conexiones de masa: una masa común (izquierda) o una masa para cada canal (derecha)](../figures/probe-grounding.png)

Para señales con una frecuencia menor de 5 MHz, un solo cable de masa para todos los canales es suficiente. Para señales con una frecuencia más alta, conecte el extremo de masa de cada cable de canal a la masa cerca de su señal. Las conexiones de masa cortas dan flancos de señal limpios.

## Desconectar el DSLogic Plus

> [!CAUTION]
> No desconecte el cable USB durante una captura. Si lo desconecta, los datos de la captura pueden tener errores.

1. Detenga la captura. Haga clic en **Detener** si aparece en la barra de herramientas.
2. Desconecte la alimentación del circuito.
3. Desconecte las sondas del circuito.
4. Desconecte el cable USB.
