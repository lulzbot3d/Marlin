# LulzBot Marlin firmware

This is the development branch of Marlin for LulzBot printers.

The source on this branch can compile firmware for the TAZ and Mini series of printers and all the current toolheads. This firmware also supports some internal R&D prototypes and toolheads.

# Safety and warnings:

**This repository may contain untested software.** It has not been extensively tested and may damage your printer and present other hazards. Use at your own risk. Do not operate your printer while unattended and be sure to power it off when leaving the room. Please consult the documentation that came with your printer for additional safety and warning information.

# Compilation from the command line on Linux using "avr-gcc"

Run the "build-lulzbot-firmware.sh" from the top level directory. This will build the ".hex" files for every printer and toolhead combination. The ".hex" files will be saved in the "build" subdirectory.

# Compilation using Arduino IDE

Replace the "Configuration.h" and "Configuration_adv.h" files in the "Marlin" subdirectory with one of the example configuration files from "config/examples/AlephObjects"

Open the "Marlin.ino" file from the "Marlin" subdirectory in the Arduino IDE.

Choose "Preferences" from the "File" menu and add "https://raw.githubusercontent.com/ultimachine/ArduinoAddons/master/package_ultimachine_index.json" to the "Additional Boards Manager URLs".

In the "Boards Manager":
    - For TAZ Pro: Search for "Archim" and install "Archim by UltiMachine"
    - For all others: Search for "RAMBo" and install "RepRap Arduino-compatible Mother Board (RAMBo) by UltiMachine"

Choose the board corresponding to your printer from the "Board" submenu menu of the "Tools" menu. For a TAZ Pro, select "Archim"; for all others, select "RAMBo"

To compile and upload the firmware to your printer, select "Upload" from the "Sketch" menu.

# Modifying the Default Configuration

If you need to modify Marlin's configuration, change the "Configuration.h" and "Configuration_adv.h" as desired following the directions from "marlinfw.org". Any LulzBot specific changes to the default values are highlighted by "<-- changed"