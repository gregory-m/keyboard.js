# keyboard.js v2

## Documentation
* [Introduction to USB and AVR USB stack](https://kampi.gitbook.io/avr/lets-use-usb)

## How to flash keyboarder with new firmware.
1. Install `avr-gcc` (version 9.4.0 used during development) and `avrdude`.
2. Compile code:
```
$ make 
```
3. Put keyboarder into DFU mode:
    1. Disconnect keyboarder from usb.
    2. Press and hold shrug key.
    3. Connect keyboarder to usb while holding shrug key.

    New USB device named `ATm32U4DFU` should appear.
4. Flash code to keyboarder:
```
$ make flash
```

## DFU Bootloader
[Official AVR USB DFU bootloader](https://www.microchip.com/en-us/product/atmega32u4) 
in "Embedded Software" section.