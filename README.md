# MidiMutt
<p align="center">
  <img width="128" height="128" src="icons/icon.png">
</p>

### *Transforming your spare gear into a macro suite since 20XX!*
MidiMutt is a Windows utility for converting MIDI input events into simulated keystrokes, mouse events, and scriptable macros with Lua integration. It is compatible with all devices that implement the MIDI standard.

# Features
- Simulate all forms of keyboard and mouse input - supports 3 different key simulation APIs
- Trigger actions based on velocity, including options for modulus (spacing) and sign
- Flexible repeat modes (*n* times, until Note_Off, until arbitrary MIDI signal)
- Write Lua scripts directly in the program or load from a file
- Restrict output unless a specific window is in focus
- Run shell commands

![Screenshot of the MidiMutt window](https://i.imgur.com/BqFLRVH.png)

# Installation
Downloaded the [latest release](https://www.github.com/Squawks/MidiMutt/releases), unzip it anywhere, and run MidiMutt.exe.

*(Note: some applications may require MidiMutt to run as administrator before they can receive simulated inputs.)*

# License
MidiMutt comes distributed under the MIT license. See [LICENSE](LICENSE) for the details.
