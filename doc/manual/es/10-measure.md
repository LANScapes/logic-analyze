# Mediciones

Puede medir la forma de onda con el ratón o con cursores. Un cursor es una línea vertical en un instante de la captura.

## Medir un pulso con el puntero

Coloque el puntero sobre un pulso de un canal. Un recuadro cerca del puntero muestra estos valores del pulso:

- **Ancho**: la duración del pulso.
- **Período**: el tiempo entre un flanco y el flanco siguiente de la misma dirección.
- **Frecuencia**: 1 dividido por el período.
- **Ciclo de trabajo**: el tiempo en nivel alto como porcentaje del período.

![La medición en el puntero](../figures/hover-measurement.png)

Para mostrar u ocultar este recuadro, abra el panel de medición y use **Activar medición flotante**.

## Contar los flancos en un área

1. Coloque el puntero sobre la forma de onda del canal, entre el nivel alto y el nivel bajo.
2. Mueva el puntero al principio del área.
3. Haga clic con el botón izquierdo del ratón.
4. Mueva el puntero al final del área. La aplicación muestra el número de flancos, de flancos ascendentes y de flancos descendentes.
5. Haga clic otra vez con el botón izquierdo del ratón para terminar la medición.

## Medir el tiempo entre dos flancos

1. Coloque el puntero sobre el primer flanco.
2. Haga clic con el botón izquierdo del ratón.
3. Mueva el puntero al segundo flanco. La aplicación muestra el tiempo y el número de muestras entre los dos flancos.
4. Haga clic otra vez con el botón izquierdo del ratón para terminar la medición.

![El tiempo entre dos flancos](../figures/edge-distance.png)

## Añadir un cursor

Use uno de estos métodos:

- En el área de formas de onda, haga doble clic con el botón izquierdo del ratón en el instante que quiere. Si el puntero está cerca de un flanco, el cursor se coloca en el flanco.
- En la regla de tiempo, haga clic con el botón izquierdo del ratón. Aparece una flecha en la regla. Haga clic en la flecha para añadir un cursor.

![Añadir un cursor desde la regla de tiempo](../figures/ruler-insert-cursor.png)

Cada cursor tiene un número. Los números empiezan en 1.

## Mover un cursor

Use uno de estos métodos:

- Coloque el puntero sobre el cursor. La línea del cursor se hace más gruesa. Haga clic en el cursor. Mueva el ratón. Haga clic otra vez para soltar el cursor. Cerca de un flanco, el cursor se coloca en el flanco.
- En la regla de tiempo, haga clic con el botón izquierdo del ratón en el instante nuevo. La regla muestra los números de todos los cursores. Haga clic en el número del cursor que quiere mover.

![Mover un cursor desde la regla de tiempo](../figures/ruler-move-cursor.png)

## Ir a un cursor

1. En la regla de tiempo, haga clic con el botón derecho del ratón. La regla muestra los números de todos los cursores.
2. Haga clic en el número de un cursor. La forma de onda se mueve a la posición de ese cursor.

![Ir al cursor 3](../figures/ruler-jump-cursor.png)

## Medir con cursores

Para abrir el panel de medición, haga clic en **Medir** en la barra de herramientas o pulse `M`. El panel tiene estos grupos:

- **Distancia entre cursores**: el tiempo y el número de muestras entre dos cursores.
- **Flancos**: el número de flancos de un canal entre dos cursores.
- **Cursores**: el tiempo y el número de muestra de cada cursor.

Para añadir una medición de tiempo, haga estos pasos:

1. En el grupo **Distancia entre cursores**, haga clic en el botón **+**.
2. Haga clic en el campo de inicio y seleccione el primer cursor.
3. Haga clic en el campo de fin y seleccione el segundo cursor.

El panel muestra el resultado en la columna **Tiempo/Muestras**.

Para añadir un recuento de flancos, haga estos pasos:

1. En el grupo **Flancos**, haga clic en el botón **+**.
2. Seleccione el cursor de inicio y el cursor de fin.
3. Seleccione el canal.

El panel muestra el número de flancos ascendentes, de flancos descendentes y de todos los flancos.

Para quitar una medición, haga clic en el botón **×** de su fila.

## Eliminar un cursor

Use uno de estos métodos:

- Haga clic en la **×** de la etiqueta del cursor en la regla de tiempo.
- Haga clic en el botón **×** del cursor en el grupo **Cursores** del panel de medición.

La aplicación da números nuevos a los cursores que quedan.
