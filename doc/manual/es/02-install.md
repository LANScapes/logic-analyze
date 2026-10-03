# Instalar e iniciar la aplicación

## Instalar la aplicación

1. Vaya a la página de versiones del proyecto: <https://github.com/LANScapes/logic-analyze/releases>.
2. Descargue el archivo ZIP de la versión más reciente.
3. Abra el archivo ZIP en el Finder. El Finder extrae **Logic Analyze.app**.
4. Arrastre **Logic Analyze.app** a la carpeta **Aplicaciones**.

La aplicación contiene todas las bibliotecas, el firmware y los decodificadores de protocolo necesarios. En macOS no es necesario instalar un controlador.

## Iniciar la aplicación

1. Conecte el analizador lógico al ordenador. Consulte [Conectar el DSLogic Plus](04-connect.md).
2. Abra **Logic Analyze** desde la carpeta **Aplicaciones** o desde el Launchpad.
3. Si macOS muestra un mensaje sobre la aplicación, haga clic en **Abrir**.

La primera vez que se inicia, la aplicación puede mostrar la ventana **Documento**. Haga clic en **Abrir** para leer este manual. Haga clic en **Ignorar** para cerrar la ventana. Haga clic en **No volver a mostrar** si no quiere ver esta ventana otra vez.

## Actualizar la aplicación

1. Haga clic en **Ayuda** › **Actualizar**. La aplicación abre la página de versiones en su navegador web.
2. Si la página de versiones muestra una versión más reciente, descárguela.
3. Salga de Logic Analyze.
4. Sustituya **Logic Analyze.app** en la carpeta **Aplicaciones** por la versión nueva.

La aplicación conserva sus ajustes cuando la sustituye.

## Abrir este manual

Haga clic en **Ayuda** › **Manual...**. La aplicación abre este manual en el idioma de la interfaz de usuario. Si el manual no está disponible en ese idioma, la aplicación abre el manual en inglés.

## Cambiar el idioma

1. Haga clic en **Ayuda** › **Idioma**.
2. Seleccione un idioma de la lista.

La interfaz de usuario cambia al idioma que selecciona.

## Cambiar el tema

1. Haga clic en **Opciones** › **Pantalla** › **Temas**.
2. Seleccione **Oscuro** o **Claro**.

## Mover la barra de herramientas

Puede colocar la barra de herramientas arriba, abajo, a la izquierda o a la derecha de la ventana. Con la barra de herramientas a la izquierda o a la derecha, 16 canales pueden usar toda la altura de la ventana.

1. Coloque el puntero sobre el asa al principio de la barra de herramientas.
2. Mantenga pulsado el botón del ratón.
3. Arrastre la barra de herramientas hasta un borde de la ventana.
4. Suelte el botón del ratón.

La aplicación conserva la posición de la barra de herramientas en el siguiente inicio.

## Opciones de visualización

Para cambiar las opciones de visualización, haga clic en **Opciones** › **Pantalla** › **Opciones**. La ventana **Opciones de visualización** muestra estos ajustes:

| Ajuste | Función |
| --- | --- |
| **Desplazar la forma de onda arrastrando con el ratón** | Cuando arrastra la forma de onda rápidamente y la suelta, la forma de onda sigue moviéndose. Después, se mueve más despacio y se detiene. |
| **Actualizar los últimos datos al detener el modo repetitivo** | Cuando detiene una captura en el modo **Repetitivo**, la aplicación muestra los datos de la última captura, que no está completa. |
| **Desplazar automáticamente a los últimos datos** | En modo Stream, la forma de onda se mueve para mostrar los datos más recientes. |
| **Mostrar la posición del disparo en el centro** | En modo osciloscopio, la aplicación muestra la posición del disparo en el centro de la ventana. |
| **Mostrar la sesión en la barra de título** | La barra de título muestra el nombre del archivo de sesión. |
| **Tamaño de fuente** | El tamaño del texto en el área de formas de onda. |

## Opciones de registro

La aplicación puede escribir un archivo de registro. El archivo de registro ayuda a encontrar la causa de un problema.

1. Haga clic en **Ayuda** › **Opciones de registro**.
2. Seleccione un valor de 0 a 5 en **Nivel de registro**. Un valor más alto registra más mensajes.
3. Seleccione **Guardar en archivo**.
4. Para añadir los mensajes nuevos al final del archivo de registro existente, seleccione **Modo de anexado**.
5. Haga clic en **OK**.

Para ver el archivo de registro, haga clic en **Abrir** en la ventana **Opciones de registro**. Para borrar el archivo de registro, haga clic en **Borrar**.

## Informar de un problema

Haga clic en **Ayuda** › **Informar error**. La aplicación abre la página de incidencias del proyecto en su navegador web. Indique en su informe la versión de la aplicación, la versión de macOS y el modelo del dispositivo. Si es posible, adjunte el archivo de registro.
