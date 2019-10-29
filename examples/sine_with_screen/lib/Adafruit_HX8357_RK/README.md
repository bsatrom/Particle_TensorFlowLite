# Adafruit_HX8357_RK

*Port of Adafruit TFT FeatherWing - 3.5" Touchscreen for Particle Argon/Boron/Xenon*

The [Adafruit 3.5" TFT FeatherWing](https://www.adafruit.com/product/3651) has a touch screen display on the front and a socket for any Feather on the back, including the Particle Argon, Boron, and Xenon. You can find [technical information](https://learn.adafruit.com/adafruit-3-5-tft-featherwing) at Adafruit.

There aren't any wires necessary, but it does use the following pins:

- SPI (SCK, MISO, MOSI)
- SD (SD card CS) = D2
- RT (Touch screen CS) = D3
- TFT_CS (display) = D4
- D/C (display) = D5

This library is just a wrapper around two other libraries:

- [Adafruit\_STMPE610\_RK](https://github.com/rickkas7/Adafruit_STMPE610_RK)
- [Adafruit\_GFX\_RK](https://github.com/rickkas7/Adafruit_GFX_RK)

Once the SdFat library is modified to support the mesh devices it will be added as well. 

You don't need to use this library - if you only need the display and not the touchscreen, for example, you can use the Adafruit\_ILI9341\_RK directly directly.

There are two Adafruit examples:

- examples/graphicstest\_featherwing
- examples/touchpaint\_featherwing

If you are looking for the 2.4" TFT Featherwing, check out [Adafruit\_ILI9341\_RK](https://github.com/rickkas7/Adafruit_ILI9341_RK) instead.



# Adafruit HX8357 Arduino Library [![Build Status](https://travis-ci.org/adafruit/Adafruit_HX8357_Library.svg?branch=master)](https://travis-ci.org/adafruit/Adafruit_HX8357_Library)



This is a library for the Adafruit HX8357 display products

This library works with the Adafruit 3.5" Breakout
   * http://www.adafruit.com/products/2050
And the 3.5" TFT FeatherWing
   * https://www.adafruit.com/product/3651

Check out the links above for our tutorials and wiring diagrams.
These displays use SPI to communicate, 4 or 5 pins are required
to interface (RST is optional).

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution

Recent Arduino IDE releases include the Library Manager for easy installation. Otherwise, to download, click the DOWNLOAD ZIP button, uncompress and rename the uncompressed folder Adafruit_HX8357. Confirm that the Adafruit_HX8357 folder contains Adafruit_HX8357.cpp and Adafruit_HX8357.h. Place the Adafruit_HX8357 library folder your ArduinoSketchFolder/Libraries/ folder. You may need to create the Libraries subfolder if its your first library. Restart the IDE.

Also requires the Adafruit_GFX library for Arduino.
