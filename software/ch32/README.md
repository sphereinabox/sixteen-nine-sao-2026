## Ch32V003 software for sixteen-nine-SAO

Uses ch32fun: https://github.com/cnlohr/ch32fun

To get started compiling and uploading this software:
 1. Install Visual Studio Code
 2. Install PlatformIO extension https://platformio.org/install/ide?install=vscode
 3. Open folder `software/ch32` in Visual Studio Code
 4. Select project for example `example_blink` from bottom toolbar
 5. Build using checkmark in bottom toolbar.

### Connecting Programmer
To connect to ch32 programmer the pins on J3 header, connect the following:
 1. GND
 2. VCC to programmer power output, or power the SAO from a badge
 3. SWIO

Programmer options I've tried:
 - Official WCH-LinkE
 - ESP32-S2, running https://github.com/cnlohr/esp32s2-cookbook/tree/master/ch32v003programmer

You might try:
 - RP2040 with micropython: https://github.com/psychogenic/pico_ch32v003_prog
 - arduino uno atmega328p or similar with https://gitlab.com/BlueSyncLine/arduino-ch32v003-swio
 - others listed on readme for ch32fun https://github.com/cnlohr/ch32fun

#### ESP32-S2 programmer notes
https://github.com/cnlohr/esp32s2-cookbook/tree/master/ch32v003programmer

I've used the following boards:
 - flipper zero wifi devboard. I swapped SWIO pin from 6 to 18 to use already-soldered headers
 - cheap purple aliexpress/amazon ESP32-S2FN4R2 boards 

The latest ESP-IDF 6.1 didn't work, but 5.3 did

the SWIO_PIN pin on esp32-s2 is 6, specified in main/pindefs.h

Activate ESP-IDF environment and build with:
```
source ~/.espressif/tools/activate_idf_v5.3.sh
idf.py build
```
upload to esp32-s2 with
`esptool.py -p /dev/ttyACM0 -b 460800 --before=no_reset --after=no_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 build/bootloader/bootloader.bin 0x10000 build/usb_sandbox.bin 0x8000 build/partition_table/partition-table.bin`

lsusb showed it as
`Bus 003 Device 104: ID 303a:4004 CNLohr ESP32-S2 CH32V003Programmer`

see `udev` folder for `/etc/udev/rules.d/` file examples
