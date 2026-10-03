# Examinar la forma de onda

Después de una captura, el área de formas de onda muestra los datos de cada canal como una forma de onda.

## Mover la forma de onda a la izquierda o a la derecha

Use uno de estos métodos:

- **Arrastrar.** Coloque el puntero sobre la forma de onda. Mantenga pulsado el botón izquierdo del ratón y mueva el ratón a la izquierda o a la derecha.
- **Arrastre rápido.** Arrastre la forma de onda rápidamente y suelte el botón del ratón. La forma de onda sigue moviéndose. Después, se mueve más despacio y se detiene. Para activar o desactivar esta función, use **Desplazar la forma de onda arrastrando con el ratón** en las opciones de visualización. Consulte [Instalar e iniciar la aplicación](02-install.md).
- **Barra de desplazamiento.** Arrastre la barra de desplazamiento de la parte de abajo de la ventana.
- **Teclado.** Pulse `Page Up` o `Page Down`. La forma de onda se mueve un ancho de ventana.

## Acercar y alejar

Use uno de estos métodos:

- **Rueda del ratón.** Coloque el puntero sobre la forma de onda y gire la rueda del ratón. El puntero queda en el centro del zoom.
- **Zoom de área.** Mantenga pulsado el botón derecho del ratón y arrastre sobre la forma de onda. Suelte el botón. La aplicación acerca el área que seleccionó.
- **Vista completa.** Haga doble clic en la forma de onda con el botón derecho del ratón. La aplicación muestra todos los datos. Haga doble clic otra vez con el botón derecho del ratón para volver al zoom anterior.
- **Teclado.** Pulse `←` para acercar. Pulse `→` para alejar.

## Buscar un patrón

La búsqueda encuentra un patrón de niveles y flancos en los canales.

1. Haga clic en **Buscar** en la barra de herramientas o pulse `R`. La barra de búsqueda se abre en la parte de abajo de la ventana.
2. Haga clic en el campo de búsqueda. Se abre la ventana **Opciones de búsqueda**.
3. Para cada canal, escriba uno de los caracteres `X`, `0`, `1`, `R`, `F` o `C`. [Disparos](07-trigger.md) da la función de cada carácter.
4. Haga clic en **OK**.
5. Haga clic en los botones de flecha de la barra de búsqueda para ir al resultado anterior o al resultado siguiente.

![La ventana Opciones de búsqueda](../figures/search-options.png)
<!-- TODO: new screenshot -->

Por ejemplo, escriba `C` para el canal 0 y `X` para todos los otros canales. La búsqueda encuentra entonces cada flanco del canal 0.

## Cambiar un canal

Cada canal tiene una etiqueta a la izquierda del área de formas de onda. La etiqueta muestra el color, el nombre y el número del canal.

![La etiqueta de canal: color, nombre y número](../figures/channel-label.png)

### Cambiar el color

1. Haga clic en la zona de color de la etiqueta de canal.
2. Seleccione un color.
3. Haga clic en **OK**.

### Cambiar el nombre

1. Haga clic en el nombre de la etiqueta de canal.
2. Escriba un nombre nuevo.
3. Pulse `Return`.

### Cambiar el orden de los canales

Cuando coloca el puntero sobre una etiqueta de canal, el puntero muestra una flecha. Use uno de estos métodos para mover el canal:

- Mantenga pulsado el botón izquierdo del ratón sobre la etiqueta y arrastre el canal hacia arriba o hacia abajo. Suelte el botón en la posición nueva.
- Haga clic en la etiqueta para seleccionar el canal. Mueva el ratón. El canal se mueve con el ratón. Haga clic otra vez para soltar el canal en la posición nueva.
- Mantenga pulsada la tecla `Command` y haga clic en varias etiquetas. Mueva el ratón. Los canales seleccionados se mueven con el ratón. Haga clic otra vez para soltarlos.
