# Connect the DSLogic Plus

## Connect the USB cable

> [!NOTE]
> Use the USB cable that came with the device, or a short USB cable of good quality. Connect the cable directly to a port on the computer. A USB hub or a long cable can cause errors in a capture.

1. Connect the USB cable to the DSLogic Plus.
2. Connect the other end of the USB cable to a USB port on the computer.
3. Make sure that the indicator on the DSLogic Plus comes on. Before the app starts, the indicator shows red.
4. Start Logic Analyze.
5. Make sure that the indicator changes to green.
6. Make sure that the device list on the toolbar shows **DSLogic Plus**.

![The USB connection](../figures/usb-connection.png)

If the device list does not show the device, do these steps:

1. Disconnect the USB cable from the computer.
2. Wait 5 seconds.
3. Connect the USB cable to a different USB port.
4. If the device list does not show the device after step 3, quit the app and start it again.

> [!NOTE]
> Only one program can use the device at a time. If `dslcap` or a different program uses the device, the app cannot find it.

## Connect the probe cable

The probe cable has 16 channel wires. Each channel wire has a shield, a signal end and a ground end. The colors of the wires identify channels 0 to 15. One more wire has these signals:

- **CK**: The input for an external clock. Use it only with the **Using External Clock** setting.
- **TI**: The input for an external trigger signal.
- **TO**: The output for the trigger signal. The device sends a pulse on TO when the trigger occurs.

Usually, you do not connect the CK, TI and TO wires.

![The probe cable and its channels](../figures/probe-cable-channels.png)

1. Connect the probe cable to the input connector of the DSLogic Plus.
2. Push the connector fully into the device.

## Connect the channels to the circuit

> [!WARNING]
> Do not connect the probes to mains voltage. Do not connect the probes to a circuit that has an electrical connection to mains voltage. The voltage can cause injury or death.

> [!CAUTION]
> Before you connect a ground wire, make sure that the ground of the circuit and the ground of the computer have the same voltage. A difference in voltage can cause a large current that can cause damage to the equipment.

1. Disconnect the power from the circuit that you measure.
2. Connect at least one ground wire to the ground of the circuit.
3. Connect each channel wire that you use to a signal in the circuit.
4. Make sure that no probe touches a different contact.
5. Connect the power to the circuit.

![Ground connections: one common ground (left) or one ground for each channel (right)](../figures/probe-grounding.png)

For signals with a frequency of less than 5 MHz, one ground wire for all channels is sufficient. For signals with a higher frequency, connect the ground end of each channel wire to the ground near its signal. Short ground connections give clean signal edges.

## Disconnect the DSLogic Plus

> [!CAUTION]
> Do not disconnect the USB cable during a capture. If you disconnect it, the data of the capture can have errors.

1. Stop the capture. Click **Stop** if it shows on the toolbar.
2. Disconnect the power from the circuit.
3. Disconnect the probes from the circuit.
4. Disconnect the USB cable.
