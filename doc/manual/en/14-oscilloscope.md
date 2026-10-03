# Oscilloscope and data acquisition modes

Logic Analyze can also operate the DSCope oscilloscopes from DreamSourceLab. A DSCope has two device modes:

- **Oscilloscope**: for signals with a constant period and for one signal condition.
- **Data Acquisition**: for slow signals over a long time, for example a supply voltage or a sensor output.

These modes are not available on the DSLogic devices. This chapter gives only the primary procedures.

## Connect the DSCope

> [!WARNING]
> Do not connect the probes to mains voltage. Do not connect the probes to a circuit that has an electrical connection to mains voltage. The voltage can cause injury or death.

> [!CAUTION]
> The ground of the probes, the ground of the DSCope and the ground of the computer connect together. Connect the probe ground only to a point that has the same voltage as the ground of the computer. A difference in voltage can cause damage to the equipment.

1. Connect the DSCope to the computer with the USB cable.
2. Start Logic Analyze. Make sure that the device list shows the DSCope.
3. Connect the probes to the inputs of the DSCope.
4. Set the attenuation switch on each probe.
5. Connect the ground clip of each probe to the ground of the circuit.
6. Connect the probe tip to the signal.

## Device options

Click **Options** › **Device Options...** or press `O`.

- **Operation Mode**: **Normal** for measurements. **Internal Test** is for tests of the device only.
- **Bandwidth Limit**: **Full Bandwidth** or **20MHz**. The 20 MHz limit decreases high-frequency noise.

## Calibrate the DSCope

The gain and the offset of the inputs change with temperature and humidity. Calibrate the DSCope to keep the measurements accurate.

### Automatic calibration

> [!CAUTION]
> Disconnect all the probes from the inputs before the calibration. A signal on an input during the calibration gives incorrect calibration values.

1. Open the **Device Options** window.
2. Click **Auto Calibration**.
3. Disconnect all the probes. Click **OK**. The calibration continues for some minutes.
4. When the calibration is complete, click **Save** to keep the result.

To stop the calibration, click **Abort**. The device then uses the previous calibration values.

### Manual calibration

1. Open the **Device Options** window.
2. Click **Manual Calibration**.
3. Click **Start** on the toolbar.
4. To adjust the offset, connect the probe to ground. To adjust the gain, connect the probe to a signal with a known voltage.
5. Set the vertical scale that you want to calibrate.
6. Move the **VOFF** or **VGAIN** slider of the channel until the waveform is correct.
7. Do steps 5 and 6 again for each vertical scale.
8. Click **Save**.

To discard the changes, click **Abort**. To use the changes only until you disconnect the device, click **Exit**. To go back to the initial values, click **Reset**. After a reset, do the automatic calibration again.

## Channel settings

Each channel has these controls on the left of the waveform area:

- **Enable**: turns the channel on or off.
- **Vertical scale**: the voltage for each division. The window has 10 divisions. To change the scale, turn the mouse wheel on the knob, or click the top or the bottom part of the knob. You can also press `0` or `1` to select the knob of a channel and then press `↑` or `↓`.
- **Coupling**: **DC** or **AC**.
- **Probe attenuation**: set **x1** or **x10** to agree with the switch on the probe.
- **AUTO**: sets the vertical scale, the horizontal scale and the trigger level for the signal that is on the input.

To move the waveform of a channel up or down, drag the channel label.

## Horizontal scale

Select the time for each division in the list on the toolbar. You can also turn the mouse wheel in the waveform area.

## Start and stop

- Click **Start** or press `S` to start a continuous capture. Click **Stop** to stop it.
- Click **Single** or press `I` to capture one waveform and stop.

## Trigger

Click **Trigger** or press `T` to open the trigger dock. The dock has these settings:

- **Trigger Position**: the position of the trigger point in the capture, as a percentage.
- **Hold Off Time**: the time after a trigger when the device ignores new triggers. Use it to get a stable waveform from groups of pulses.
- **Trigger Sensitivity**: the voltage change that is necessary for a trigger. A larger value ignores more noise.
- **Trigger Sources**: **Auto**, **Channel 0**, **Channel 1**, **Channel 0 && 1** or **Channel 0 | 1**.
- **Trigger Types**: **Rising Edge** or **Falling Edge**.

To set the trigger level, click the trigger level label of the channel. Move the mouse. Click again to set the level.

## Measurements

### Automatic measurements

The bottom of the waveform area has 10 boxes for automatic measurements.

1. Click a measurement box.
2. Select the channel.
3. Select the measurement. To clear the box, click **Reset**.

The app keeps these settings for the next start.

### Cursors

- To add a time cursor, click the time ruler. You can also click the right mouse button in the waveform area and select **Add Y-cursor**.
- To add a voltage cursor, click the right mouse button in the waveform area and select **Add X-cursor**. Each voltage cursor has two horizontal lines. The label between the lines shows the voltage difference.
- To measure the time between two cursors, use the **Cursor Distance** group in the measurement dock.

### Measure with the pointer

After you stop the capture, put the pointer on the waveform. The app shows the voltage of the sample at the pointer.

To measure a time, do a double-click in an empty area of the waveform. Click at the second point. Click at the third point to see the frequency, the period and the duty cycle. Click the right mouse button to cancel.

## Spectrum (FFT)

1. Click **Function** › **FFT**.
2. Select **FFT Enable**.
3. Set **FFT Length**, **Sample Interval**, **FFT Source** and **FFT Window**.
4. Set **Y-axis Mode** and **DBV Range**.
5. Click **OK**.

The spectrum shows below the waveform. Turn the mouse wheel in the spectrum to zoom the frequency scale. Drag the spectrum to move it. Put the pointer on the spectrum to see the frequency and the amplitude.

## Math channel

1. Click **Function** › **Math**.
2. Select **Enable**.
3. Select the **Math Type**: **Add**, **Subtract**, **Multiply** or **Divide**.
4. Select the **1st Source** and the **2nd Source**.
5. Click **OK**.

## Lissajous figure

1. Click **Options** › **Display** › **Lissajous**.
2. Select **Enable**.
3. Select the channel for the **X-axis** and the **Y-axis**.
4. Click **OK**.

## Data acquisition mode

1. In the device mode list on the toolbar, select **Data Acquisition**.
2. Open the **Device Options** window.
3. For each channel, set **Enable**, **Coupling** and **Volts/div**.
4. To show a different unit, set **Map Unit**, **Map Min** and **Map Max**. For example, show the output of a temperature sensor in °C.
5. Click **OK**.
6. Select the sample rate and the sample duration on the toolbar.
7. Click **Start** or press `S`.

You cannot change the channel settings during the capture. At the highest sample rate of 10 MHz, the maximum sample duration is about 10 seconds. At 1 kHz, the capture can continue for one day.

The data acquisition mode uses the calibration of the oscilloscope mode. If a channel shows an offset, calibrate the device in oscilloscope mode.
