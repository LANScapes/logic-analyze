# Introduction

## About Logic Analyze

Logic Analyze is a macOS app for logic analyzers from DreamSourceLab. LANScapes supplies the app. The app comes from DSView, which is a program from DreamSourceLab. DSView uses software from the sigrok project.

The app records digital signals from a DSLogic logic analyzer. Then it shows the signals as waveforms. You can measure the waveforms and decode serial protocols. You can also save the data and export it to other formats.

This manual uses the DSLogic Plus for the examples. Other DSLogic models operate with the same procedures. Their limits for channels, memory and sample rate are different.

The app also includes the `dslcap` tool. This tool captures data without the main window. Refer to [The dslcap tool](13-dslcap.md).

## About this manual

This manual uses ASD-STE100 Simplified Technical English. Each sentence is short. Each step in a procedure gives one instruction. Each technical name in this manual has only one sense. [Technical names and verbs](15-terms.md) gives the list of the technical names.

This manual uses these text formats:

- **Bold text** shows a label in the app, for example a button, a menu item or a field.
- `Code text` shows a key on the keyboard, a command, a file name or a value that you type.
- A path through menus uses the sign ›, for example **File** › **Save...**.
- A list of steps with numbers is a procedure. Do the steps in the given sequence.

## Safety instructions

This manual uses these labels for safety instructions:

- **WARNING** identifies a risk of injury or death.
- **CAUTION** identifies a risk of damage to equipment or a risk to your data.
- **NOTE** gives information that helps you. The information after **NOTE** does not give an instruction.

A safety instruction comes before the step that it applies to. Read all the safety instructions before you start a procedure.

> [!WARNING]
> Do not connect the probes to mains voltage. Do not connect the probes to a circuit that has an electrical connection to mains voltage. The probes have an electrical connection to the computer. The voltage can cause injury or death.

> [!CAUTION]
> Do not apply a voltage to a channel input that is more than the limit in the device specification. A voltage that is too high can cause damage to the logic analyzer.

> [!CAUTION]
> The ground wires of the logic analyzer connect to the ground of your computer through the USB cable. Connect the ground wires only to the ground of the circuit that you measure. If the two grounds have different voltages, a large current can flow and cause damage to the circuit, the logic analyzer and the computer.

## System requirements

This equipment is necessary:

- A Mac with macOS. The release notes give the minimum macOS version.
- A USB port. A USB 3.0 port gives the highest speed. A USB 2.0 port also operates.
- A DSLogic logic analyzer, its USB cable and its probe cable.

You can use the app without a logic analyzer. The **Demo** device makes test signals. You can also open a data file from a previous capture.
