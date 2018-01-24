FeynmanBootloader is a SAM-BA compatible bootloader for the SAMG55 microcontroler for use over a USB Virtual COM Port. The SAMG55 has a ROM bootloader but it does not support USB, instead it works over I2C/SPI.

It it is based on generic sam-ba bootloader example code and the bootloader code from the Arduino Zero.

It resides in the first 16KB section of the flash on the chip. 

To go into bootloader state you can double tap the reset button. 

It requires a custom version of the bossac command line firmware upload tool. A custom version of this tool is also made abvailable to support the SAMG55 microcontroller.