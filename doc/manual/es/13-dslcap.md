# La herramienta dslcap

La herramienta `dslcap` captura datos de un dispositivo DSLogic sin la ventana principal. Úsela en scripts y en pruebas automáticas. La herramienta escribe las muestras en un archivo binario. Escribe un objeto JSON con el resultado en la salida estándar.

La herramienta está en el paquete de la aplicación:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Solo un programa a la vez puede usar el dispositivo. Salga de Logic Analyze antes de usar `dslcap`.

## Listar los dispositivos

Para listar los dispositivos que la biblioteca puede encontrar, escriba este comando:

```sh
dslcap --list
```

Para listar el identificador USB de cada dispositivo DSLogic conectado, escriba este comando:

```sh
dslcap --list-ids
```

El comando `--list-ids` lee solo la información que macOS guarda sobre los dispositivos USB. No envía datos al dispositivo. La salida da el modelo, la ubicación USB y un identificador de registro para cada dispositivo.

## Capturar datos

Este comando captura 1000000 muestras en los canales 0 y 1 a 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

La herramienta escribe las muestras en `/tmp/capture.bin`. Si ya hay un archivo con este nombre, la herramienta se detiene con un error. La herramienta no sustituye un archivo.

Estas son las opciones de captura:

| Opción | Función | Valor inicial |
| --- | --- | --- |
| `--channels LIST` | Los canales que se registran, por ejemplo `0,1,2`. | `0` |
| `--samplerate HZ` | La frecuencia de muestreo en Hz. | `10000000` |
| `--samples N` | El número de muestras para cada canal. | `1000000` |
| `--vth VOLTS` | La tensión de umbral. | `1.6` |
| `--mode MODE` | `buffer` o `stream`. | `buffer` |
| `--trigger CH[:T]` | Un disparo en el canal CH. Use `R` (flanco ascendente), `F` (flanco descendente), `C` (flanco ascendente o flanco descendente), `1` (nivel alto) o `0` (nivel bajo) para T. | Sin disparo. `R` si da solo CH. |
| `--trigpos PERCENT` | La posición del disparo como porcentaje de las muestras. | `10` |
| `--timeout SEC` | El tiempo máximo de la captura en segundos. | `30` |
| `--out PATH` | La ruta del archivo de salida, sin la extensión `.bin`. | Esta opción es necesaria. |
| `--log-level N` | La cantidad de mensajes de la biblioteca en la salida de error estándar, de 0 (ninguno) a 5 (todos). | `1` |

La herramienta examina todas las opciones antes de usar el dispositivo. Si una opción no es correcta, la herramienta se detiene y da un error.

## El archivo de salida

El archivo `.bin` contiene los canales en el orden de sus números, empezando por el número más bajo. Para cada canal, el archivo contiene todas las muestras de ese canal. Cada byte contiene 8 muestras. La primera muestra es el bit menos significativo. Los datos de cada canal ocupan un número entero de unidades de 8 bytes. Por eso, cada canal usa `ceil(samples / 64) × 8` bytes.

## El resultado JSON

La herramienta escribe un objeto JSON en una línea. Después de una captura correcta, el objeto da el nombre del dispositivo, la frecuencia de muestreo, el número de muestras y los canales. También da la tensión de umbral, el modo, el disparo, el tiempo de la captura y la ruta del archivo `.bin`. Si la captura no es correcta, el objeto contiene una clave `error`. En ese caso, la herramienta no crea un archivo `.bin`.

Use el resultado solo cuando el estado de salida es 0 y el objeto JSON está completo.

## Estado de salida

| Estado | Significado |
| --- | --- |
| 0 | La captura está completa. |
| 1 | Ocurrió un error durante la operación, por ejemplo un error de E/S. |
| 2 | Una opción no es correcta, o un ajuste no está disponible en el dispositivo. |
| 3 | La captura no se completó. |

## Opciones para programas que inician dslcap

- `--parent-fd N`: La herramienta se detiene cuando el programa que la inició cierra la tubería con el descriptor N. Después, la herramienta elimina su archivo de salida si la captura no está completa.
- `--res DIR`: La carpeta con los archivos de firmware. Normalmente, la herramienta encuentra esta carpeta automáticamente. También puede ajustar la variable de entorno `DSLCAP_RES`.
- `--res-manifest FD`: La herramienta examina el valor SHA-256 de cada archivo de firmware antes de enviar el archivo al dispositivo.

El archivo `tools/dslcap/README.md` del código fuente da toda la información sobre estas opciones.
