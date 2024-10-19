# Capture and Analyze Timing of WS2812 LED Data

WS2812Capture is a Teensy 4.x library which can capture and analyze WS2812 LED data.
It's primary purpose is to verify the correctness timing used by other libraries
which transmit data to WS2812 addressable LEDs.

![Screenshot with timing numbers overlaid with photo of 8 LEDs and 2 Teensy boards](docs/eightleds.png)

## Hardware

A resistor divider should be used to reduce the 5 volt signal level used by WS2812
to the 3.3V level needed.  Teensy 4.0 pins are not 5V tolerant, so direct connection
of a 5V signal could risk hardware damage.

Pins 2, 4, 5, 6, 8, 22, 23, 29 on Teensy 4.0 maybe used to capture WS2812 data.

## Basic Functions

`WS2812Capture myleds(Pin, PixelFormat)`

`myleds.begin(buffer, sizeof(buffer))`

`myleds.available()`

`myleds.numPixels()`

`myleds.getPixel(index)`

## Detailed Timing Functions

`myleds.numBits()`

`myleds.bitHighNanoseconds()`

`myleds.bitLowNanoseconds()`

`myleds.getResetMicroseconds()`


