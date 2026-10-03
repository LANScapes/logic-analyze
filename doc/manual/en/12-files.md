# Files and sessions

Click **File** on the toolbar to open the file menu. The menu has these items:

- **Config**: a menu to load and save sessions.
- **Open...**: open a data file.
- **Save...**: save the data of the capture.
- **Export...**: export the data to a different format.
- **Capture...**: save an image of the window.

## Sessions

A session file contains the settings, but not the data of the capture. A session includes the device options, the enabled channels, the channel names and colors, and the trigger settings. A session file has the extension `.dsc`.

### Save a session

1. Click **File** › **Config** › **Save Session**.
2. Select the folder and type the file name.
3. Click **Save**.

### Load a session

1. Click **File** › **Config** › **Load Session**.
2. Select the session file.
3. Click **Open**.

### Go back to the initial settings

Click **File** › **Config** › **Load Default Session**. The app sets all the settings of the device to their initial values.

The app saves the settings automatically when you quit. When you start the app again, it loads the settings from the last session.

## Save the data

1. Click **File** › **Save...**.
2. Select the folder and type the file name.
3. Click **Save**.

The app saves the data and the settings in a file with the extension `.dsl`. You can open this file again in Logic Analyze.

> [!CAUTION]
> The app does not save the data automatically. Save the data before you start a new capture or quit the app. A new capture replaces the data of the previous capture.

## Open a data file

1. Click **File** › **Open...**.
2. Select a file with the extension `.dsl`.
3. Click **Open**.

The app shows the data in the waveform area. The device type label shows **File**.

## Export the data

The export makes a file that other programs can read.

1. Click **File** › **Export...**. The **Export** window opens.
2. Click **path**.
3. Select the folder, type the file name and select the format.
4. Click **Save**.
5. If the format is CSV, select **Original data** or **Compressed data**. Compressed data contains a row only for each change of value.
6. Click **OK**.

In logic analyzer mode, these formats are available:

| Format | Extension | Use |
| --- | --- | --- |
| CSV | `.csv` | Spreadsheet programs and scripts. |
| VCD | `.vcd` | Waveform programs, for example GTKWave. |
| Gnuplot | `.gnuplot` | The Gnuplot program. |
| srzip | `.srzip` | sigrok programs, for example PulseView. |

In oscilloscope mode and in data acquisition mode, only CSV is available.

![The export window for CSV](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Save an image of the window

1. Click **File** › **Capture...**.
2. Select the folder and type the file name.
3. Select PNG or JPEG.
4. Click **Save**.
