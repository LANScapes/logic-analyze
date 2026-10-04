# Install and start the app

## Install the app

<!-- edition: download -->
1. Go to the releases page of the project: <https://github.com/LANScapes/logic-analyze/releases>.
2. Download the ZIP file of the newest release.
3. Open the ZIP file in Finder. Finder extracts **Logic Analyze.app**.
4. Drag **Logic Analyze.app** into the **Applications** folder.
<!-- end edition -->
<!-- edition: appstore -->
1. Open the App Store on the Mac.
2. Find Logic Analyze in the App Store.
3. Install the app. The App Store puts **Logic Analyze.app** in the **Applications** folder.
<!-- end edition -->

The app contains all the necessary libraries, firmware and protocol decoders. You do not install a driver on macOS.

## Start the app

1. Connect the logic analyzer to the computer. Refer to [Connect the DSLogic Plus](04-connect.md).
2. Open **Logic Analyze** from the **Applications** folder or from Launchpad.
3. If macOS shows a message about the app, click **Open**.

When the app starts for the first time, it can show the **Document** window. Click **Open** to read this manual. Click **Ignore** to close the window. Click **Not Show Again** if you do not want to see this window again.

## Update the app

<!-- edition: download -->
1. Click **Help** › **Update**. The app opens the releases page in your web browser.
2. If the releases page shows a newer version, download it.
3. Quit Logic Analyze.
4. Replace **Logic Analyze.app** in the **Applications** folder with the new version.

The app keeps your settings when you replace it.
<!-- end edition -->
<!-- edition: appstore -->
The App Store updates the app. To find an update immediately, do these steps:

1. Open the App Store on the Mac.
2. Open the list of updates.
3. If Logic Analyze is in the list, update it.

The app keeps your settings when it updates.
<!-- end edition -->

## Open this manual

Click **Help** › **Manual...**. The app opens this manual in the language of the user interface. If the manual is not available in that language, the app opens the English manual.

## Change the language

1. Click **Help** › **Language**.
2. Select a language from the list.

The user interface changes to the language that you select.

## Change the theme

1. Click **Options** › **Display** › **Themes**.
2. Select **Dark** or **Light**.

## Move the toolbar

You can put the toolbar at the top, at the bottom, on the left or on the right of the window. With the toolbar on the left or on the right, 16 channels can use the full height of the window.

1. Put the pointer on the grip at the start of the toolbar.
2. Hold the mouse button.
3. Drag the toolbar to an edge of the window.
4. Release the mouse button.

The app keeps the position of the toolbar when you start it again.

## Display options

To change the display options, click **Options** › **Display** › **Options**. The **Display options** window shows these settings:

| Setting | Function |
| --- | --- |
| **Waveform Scroll with Mouse Drag** | When you drag the waveform fast and release it, the waveform continues to move. Then it moves more slowly and stops. |
| **Update latest data for repeat mode stop** | When you stop a capture in the **Repetitive** mode, the app shows the data from the last capture, which is not complete. |
| **Auto scroll to latest data** | In stream mode, the waveform moves to show the newest data. |
| **Trig pos display at the middle** | In oscilloscope mode, the app shows the trigger position in the middle of the window. |
| **Display the profile in title bar** | The title bar shows the name of the session file. |
| **Font Size** | The dimension of the text in the waveform area. |

## Log options

The app can record a log file. The log file helps to find the cause of a problem.

1. Click **Help** › **Log Options**.
2. Select a value from 0 to 5 in **Log Level**. A higher value records more messages.
3. Select **Save To File**.
4. To add new messages to the end of the log file that you have, select **Append mode**.
5. Click **OK**.

To see the log file, click **Open** in the **Log Options** window. To erase the log file, click **Clear**.

## Report a problem

Click **Help** › **Bug Report**. The app opens the issue page of the project in your web browser. Give the app version, the macOS version and the device model in your report. If possible, attach the log file.
