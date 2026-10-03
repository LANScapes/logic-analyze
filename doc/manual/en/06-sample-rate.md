# Sample rate and sample duration

The toolbar has two lists for the capture length. The top list is the sample duration. The bottom list is the sample rate.

- The **sample duration** is the time length of the capture.
- The **sample rate** is the number of samples in each second, for each channel.

The available values change with the device, the USB connection, the operation mode and the channel mode.

## Maximum sample duration

**Buffer mode.** The memory of the device limits the sample duration. Use this formula:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

The DSLogic Plus has 256 Mbit of memory. These are two examples:

- At 100 MHz with 16 channels, the maximum sample duration is about 167.77 ms.
- At 400 MHz with 1 channel, the maximum sample duration is about 671.09 ms.

**Stream mode.** The memory of the computer limits the sample duration. The app can keep 16 G samples for each channel. These are two examples:

- At 1 MHz, the maximum sample duration is about 4.77 hours.
- At 100 MHz, the maximum sample duration is about 2.86 minutes.

## Select the sample rate

Set the sample rate to 4 to 10 times the highest frequency in the signal.

At 4 times the signal frequency, the app records each edge. But the time of each edge has an error of up to 25% of the signal period. At 10 times the signal frequency, the error decreases to 10%.

The time error of an edge is equal to one sample period or less. For example, at 100 MHz the sample period is 10 ns. Thus, the error of each edge is ±10 ns or less.

![The effect of the sample rate on the recorded waveform](../figures/sample-rate-effect.png)

These are typical values:

| Signal | Typical sample rate |
| --- | --- |
| UART at 115200 baud | 2 MHz |
| I2C at 400 kHz | 4 MHz to 10 MHz |
| SPI at 40 MHz | 400 MHz |

## Do not use a sample rate that is too high

A higher sample rate gives a more accurate waveform. But a high sample rate also has these problems:

1. The app records more data in each second. Thus, the maximum sample duration decreases. The app also uses more time to show and decode the data.
2. A slow signal can have slow edges. At a high sample rate, the app can record small pulses at the threshold during each slow edge. These pulses can cause errors in the decoders.

If you see unwanted short pulses on slow signals, decrease the sample rate. You can also set **Filter Targets** to **1 Sample Clock**. Refer to [Device options](05-device-options.md).

## Set the sample rate and the duration

1. Set the operation mode and the channel mode. Refer to [Device options](05-device-options.md).
2. In the bottom list on the toolbar, select the sample rate.
3. In the top list on the toolbar, select the sample duration.

> [!NOTE]
> When you change the channel mode, the app can change the sample rate. Examine the sample rate again after each change to the device options.
