# Capture data

Before you start a capture, set these items:

1. The device options. Refer to [Device options](05-device-options.md).
2. The sample rate and the sample duration. Refer to [Sample rate and sample duration](06-sample-rate.md).
3. The trigger, if necessary. Refer to [Triggers](07-trigger.md).
4. The capture mode. Refer to [Capture modes](#capture-modes).

## Start a capture

There are two types of capture:

- **Start** starts a standard capture. The device waits for the trigger if you set a trigger.
- **Instant** starts a capture immediately. The device does not use the trigger settings.

To start a standard capture, click **Start** or press `S`. To start an instant capture, click **Instant** or press `I`. During a capture, the button changes to **Stop**. Click **Stop** to stop the capture.

### Sequence of a standard capture in buffer mode

1. You click **Start**.
2. The app sends the settings to the device.
3. If there is no trigger, the device starts to record immediately. If there is a trigger, the device waits for the trigger.
4. The device records until the end of the sample duration or until its memory is full.
5. The device sends the data to the computer.
6. The app shows the waveform in the waveform area.

### Sequence of a standard capture in stream mode

1. You click **Start**.
2. The app sends the settings to the device.
3. If there is a trigger, the device waits for the trigger. In loop mode, the device does not use the trigger.
4. The device sends the data to the computer during the capture.
5. The app shows the waveform during the capture.
6. The capture stops at the end of the sample duration. In loop mode, the capture continues until you click **Stop**.

## Use the instant capture

The instant capture is the same as the standard capture, but it does not use the trigger settings. Use it in these conditions:

- The standard capture waits for a long time because the trigger condition does not occur.
- You want to see the signals at this time.
- You want to examine the signals before you change the trigger.

If there is no signal, a standard capture waits at the trigger position. The status shows **Waiting for Trigger!**. An instant capture records the signals immediately.

## Capture modes {#capture-modes}

To select the capture mode, click **Mode** on the toolbar. Then select one of these items:

| Capture mode | Buffer mode | Stream mode |
| --- | --- | --- |
| **Single** | Yes | Yes |
| **Repetitive** | Yes | Yes |
| **Loop** | No | Yes |

![The capture mode menu](../figures/capture-mode-menu.png)
<!-- TODO: new screenshot -->

### Single

The device does one capture. Then the capture stops.

In buffer mode, the app shows the waveform after the capture. In stream mode, the app shows the waveform during the capture.

Use this mode to capture one signal condition or the waveform at this time.

### Repetitive

The device does a capture. Then it starts the next capture automatically. This continues until you click **Stop**.

In buffer mode, the app shows a window for the interval between captures. You can set a value from 0.1 s to 10 s.

Use this mode to see a signal condition that occurs many times. For example, use it to see the signals after each reset of the circuit or after each push of a button. Use it together with a trigger.

### Loop

This mode is available only in stream mode. The capture continues until you click **Stop**. When the data is longer than the sample duration, the first data moves out of the window on the left. The newest data comes in on the right. The app discards the data that moves out.

Use this mode when you do not know the time of the signal condition. Look at the waveform during the capture. When you see the condition, click **Stop**.

> [!NOTE]
> In loop mode, the device does not use the trigger settings.

## Capture status

During a capture, the waveform area shows the status:

- **Waiting for Trigger!**: The device waits for the trigger condition.
- **Triggered!**: The trigger occurred.
- **% Captured**: The percentage of the capture that is complete.

After a capture, the bottom of the waveform area shows the **Trigger Time**.
