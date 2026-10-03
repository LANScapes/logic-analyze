# Decodificadores de protocolo

Un decodificador de protocolo lee los datos de una captura y encuentra las tramas de un protocolo, por ejemplo UART, I2C o SPI. La aplicación muestra el resultado en una fila nueva encima de los canales. La aplicación tiene más de 100 decodificadores.

Para abrir el panel de decodificadores, haga clic en **Decodificar** en la barra de herramientas o pulse `D`. El panel tiene dos partes:

- La lista de decodificadores, con el campo **Buscar decodificador...** arriba.
- La lista **Resultados de decodificación**. Esta lista muestra cada elemento del decodificador como una fila de texto.

![El panel de decodificadores](../figures/decoder-dock.png)
<!-- TODO: new screenshot -->

## Añadir un decodificador

> [!NOTE]
> Un decodificador con el prefijo `0:` es una versión reducida. No muestra los bits. No puede añadir un protocolo superior sobre él. Decodifica más rápido y usa menos memoria.

1. Haga clic en el campo **Buscar decodificador...**. Se abre la lista de decodificadores.
2. Escriba una parte del nombre del protocolo, por ejemplo `I2C`. La lista muestra solo los decodificadores que coinciden con el texto.
3. Haga clic en el decodificador. Se abre la ventana **Opciones del decodificador**.
4. Ajuste los canales del protocolo. Por ejemplo, ajuste **SCL** y **SDA** para I2C.
5. Ajuste las opciones del protocolo, por ejemplo la velocidad en baudios de un UART.
6. Seleccione las filas de resultados que muestra la aplicación.
7. Si es necesario, ajuste la región de decodificación. Consulte [Decodificar una parte de la captura](#decode-region).
8. Haga clic en **OK**.

La aplicación decodifica los datos y muestra los resultados en una fila nueva del área de formas de onda.

Para añadir más decodificadores, repita el procedimiento para cada decodificador.

![Los botones del decodificador: el botón de ajustes abre las opciones del decodificador](../figures/decoder-buttons.png)

Para cambiar los ajustes de un decodificador, haga clic en el botón de ajustes de ese decodificador en el panel.

## Añadir un decodificador apilado

Algunos protocolos usan un protocolo inferior. Por ejemplo, el protocolo 24xx EEPROM usa I2C. Cuando añade el protocolo superior, la aplicación también añade los protocolos inferiores.

1. En el campo **Buscar decodificador...**, escriba el nombre del protocolo superior, por ejemplo `24xx`.
2. Haga clic en el decodificador.
3. En la ventana **Opciones del decodificador**, ajuste las opciones de cada capa de protocolo.
4. Haga clic en **OK**.

Los resultados muestran las tramas del protocolo inferior y los comandos y datos del protocolo superior.

## Decodificar una parte de la captura {#decode-region}

Normalmente, la aplicación decodifica todos los datos. Para decodificar solo una parte, ajuste un cursor de inicio y un cursor de fin. Por ejemplo, puede ignorar el ruido de un reinicio del circuito. Un área más corta también reduce el tiempo de decodificación.

1. Añada dos cursores al principio y al final del área. Consulte [Mediciones](10-measure.md).
2. Abra la ventana **Opciones del decodificador** del decodificador.
3. En la lista **Inicio**, seleccione el cursor de inicio.
4. En la lista **Fin**, seleccione el cursor de fin.
5. Haga clic en **OK**.

## Leer la lista de resultados

La lista **Resultados de decodificación** muestra los elementos del decodificador en orden de tiempo. Haga clic en una fila para mover la forma de onda a ese elemento.

Para cambiar las columnas de la lista, haga clic en el botón de ajustes en la parte de arriba de la lista.

## Buscar un texto en los resultados

1. Escriba un texto en el campo de búsqueda de la lista **Resultados de decodificación**.
2. Haga clic en la flecha derecha para ir a la fila siguiente que contiene el texto. Haga clic en la flecha izquierda para ir a la fila anterior.

La forma de onda se mueve al elemento de cada fila que encuentra la búsqueda. Si primero hace clic en una fila, la búsqueda empieza en esa fila.

![Búsqueda en los resultados de decodificación](../figures/decoder-list-search.png)

Para buscar una secuencia de bytes, ponga el signo `-` entre los bytes. Por ejemplo, `70-70-70` encuentra tres bytes seguidos con el valor 70.

![Búsqueda de una secuencia de bytes](../figures/decoder-multibyte-search.png)

> [!NOTE]
> La búsqueda de una secuencia de bytes funciona solo con los decodificadores UART, I2C y SPI.

## Exportar los resultados

1. Haga clic en el botón de guardar en la parte de arriba de la lista **Resultados de decodificación**. Se abre la ventana **Exportar protocolo**.
2. En **Formato de exportación**, seleccione CSV o TXT.
3. Seleccione cada columna que quiere exportar. La aplicación pone todas las columnas en un archivo, en orden de tiempo.
4. Haga clic en **OK**.
5. Seleccione la carpeta y escriba el nombre del archivo.
6. Haga clic en **Guardar**.

## Eliminar un decodificador

![Eliminar un decodificador o todos los decodificadores](../figures/decoder-delete.png)

- Para eliminar un decodificador, haga clic en el botón **×** de la fila de ese decodificador.
- Para eliminar todos los decodificadores, haga clic en el botón **×** de la parte de arriba del panel, al lado del botón **+**.
