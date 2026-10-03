# Introducción

## Acerca de Logic Analyze

Logic Analyze es una aplicación de macOS para los analizadores lógicos de DreamSourceLab. LANScapes suministra la aplicación. La aplicación viene de DSView, un programa de DreamSourceLab. DSView usa software del proyecto sigrok.

La aplicación registra señales digitales con un analizador lógico DSLogic. Después, muestra las señales como formas de onda. Puede medir las formas de onda y decodificar protocolos serie. También puede guardar los datos y exportarlos a otros formatos.

Este manual usa el DSLogic Plus en los ejemplos. Los otros modelos DSLogic usan los mismos procedimientos. Sus límites de canales, memoria y frecuencia de muestreo son diferentes.

La aplicación también incluye la herramienta `dslcap`. Esta herramienta captura datos sin la ventana principal. Consulte [La herramienta dslcap](13-dslcap.md).

## Acerca de este manual

Este manual aplica las reglas de ASD-STE100 Simplified Technical English. Cada frase es corta. Cada paso de un procedimiento da una sola instrucción. Cada término técnico de este manual tiene un solo significado. [Términos técnicos y verbos](15-terms.md) da la lista de los términos técnicos.

Este manual usa estos formatos de texto:

- El **texto en negrita** indica una etiqueta de la aplicación, por ejemplo un botón, un elemento de menú o un campo.
- El `texto de código` indica una tecla del teclado, un comando, un nombre de archivo o un valor que usted escribe.
- Una ruta por los menús usa el signo ›, por ejemplo **Archivo** › **Guardar...**.
- Una lista de pasos numerados es un procedimiento. Haga los pasos en el orden indicado.

## Instrucciones de seguridad

Este manual usa estas etiquetas para las instrucciones de seguridad:

- **ADVERTENCIA** indica un riesgo de lesiones o de muerte.
- **PRECAUCIÓN** indica un riesgo de daños al equipo o un riesgo para sus datos.
- **NOTA** da información útil. La información después de **NOTA** no da una instrucción.

Una instrucción de seguridad está antes del paso al que se aplica. Lea todas las instrucciones de seguridad antes de empezar un procedimiento.

> [!WARNING]
> No conecte las sondas a la tensión de red. No conecte las sondas a un circuito que tiene una conexión eléctrica con la tensión de red. Las sondas tienen una conexión eléctrica con el ordenador. La tensión puede causar lesiones o la muerte.

> [!CAUTION]
> No aplique a una entrada de canal una tensión mayor que el límite de la especificación del dispositivo. Una tensión demasiado alta puede dañar el analizador lógico.

> [!CAUTION]
> Los cables de masa del analizador lógico se conectan a la masa del ordenador a través del cable USB. Conecte los cables de masa solo a la masa del circuito que mide. Si las dos masas tienen tensiones diferentes, puede circular una corriente alta y dañar el circuito, el analizador lógico y el ordenador.

## Requisitos del sistema

Este equipo es necesario:

- Un Mac con macOS. Las notas de la versión dan la versión mínima de macOS.
- Un puerto USB. Un puerto USB 3.0 da la velocidad más alta. Un puerto USB 2.0 también funciona.
- Un analizador lógico DSLogic, su cable USB y su cable de sondas.

Puede usar la aplicación sin un analizador lógico. El dispositivo **Demo** produce señales de prueba. También puede abrir un archivo de datos de una captura anterior.
