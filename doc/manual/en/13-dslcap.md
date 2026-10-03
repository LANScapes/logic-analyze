# The dslcap tool

The `dslcap` tool captures data from a DSLogic device without the main window. Use it in scripts and in automatic tests. The tool writes the samples to a binary file. It writes one JSON object with the result to the standard output.

The tool is in the app bundle:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Only one program can use the device at a time. Quit Logic Analyze before you use `dslcap`.

## List the devices

To list the devices that the library can find, type this command:

```sh
dslcap --list
```

To list the USB identifier of each connected DSLogic device, type this command:

```sh
dslcap --list-ids
```

The `--list-ids` command reads only the information that macOS keeps about USB devices. It does not send data to the device. The output gives the model, the USB location and a registry identifier for each device.

## Capture data

This command captures 1000000 samples on channels 0 and 1 at 10 MHz:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

The tool writes the samples to `/tmp/capture.bin`. If there is a file with this name, the tool stops with an error. The tool does not replace a file.

These are the capture options:

| Option | Function | Value at start |
| --- | --- | --- |
| `--channels LIST` | The channels to record, for example `0,1,2`. | `0` |
| `--samplerate HZ` | The sample rate in Hz. | `10000000` |
| `--samples N` | The number of samples for each channel. | `1000000` |
| `--vth VOLTS` | The threshold voltage. | `1.6` |
| `--mode MODE` | `buffer` or `stream`. | `buffer` |
| `--trigger CH[:T]` | A trigger on channel CH. Use `R` (rising edge), `F` (falling edge), `C` (rising edge or falling edge), `1` (high level) or `0` (low level) for T. | No trigger. `R` if you give only CH. |
| `--trigpos PERCENT` | The trigger position as a percentage of the samples. | `10` |
| `--timeout SEC` | The maximum time of the capture in seconds. | `30` |
| `--out PATH` | The path of the output file, without the `.bin` extension. | This option is necessary. |
| `--log-level N` | The quantity of library messages on the standard error output, from 0 (none) to 5 (all). | `1` |

The tool examines all the options before it uses the device. If an option is not correct, the tool stops and gives an error.

## The output file

The `.bin` file contains the channels in the sequence of their numbers, from the lowest number. For each channel, the file contains all the samples of that channel. Each byte holds 8 samples. The first sample is the least significant bit. The data for each channel fills a full number of 8-byte units. Thus, each channel uses `ceil(samples / 64) × 8` bytes.

## The JSON result

The tool writes one JSON object on one line. After a correct capture, the object gives the device name, the sample rate, the number of samples and the channels. It also gives the threshold voltage, the mode, the trigger, the time of the capture and the path of the `.bin` file. If the capture is not correct, the object contains an `error` key. Then the tool does not make a `.bin` file.

Use the result only when the exit status is 0 and the JSON object is complete.

## Exit status

| Status | Meaning |
| --- | --- |
| 0 | The capture is complete. |
| 1 | An error occurred during the operation, for example an I/O error. |
| 2 | An option is not correct, or a setting is not available on the device. |
| 3 | The capture did not complete. |

## Options for programs that start dslcap

- `--parent-fd N`: The tool stops when the program that started it closes the pipe with descriptor N. Then the tool removes its output file if the capture is not complete.
- `--res DIR`: The folder with the firmware files. Usually, the tool finds this folder automatically. You can also set the `DSLCAP_RES` environment variable.
- `--res-manifest FD`: The tool examines the SHA-256 value of each firmware file before it sends the file to the device.

The file `tools/dslcap/README.md` in the source code gives all the data about these options.
