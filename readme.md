# Capture and Analyze Timing of WS2812 LED Data

WS2812Capture is a Teensy 4.x library which can capture and analyze WS2812 LED data.
Its primary purpose is to verify the correctness of timing used by other libraries
which transmit data to WS2812 addressable LEDs.

![Screenshot with timing numbers overlaid with photo of 8 LEDs and 2 Teensy boards](docs/eightleds.png)

Teensy 4.x FlexPWM timers with dual input capture are used to sample the incoming data
at 150 MHz, giving 6.7ns timing accuracy.  DMA is used to efficiently move measured timing
to RAM, giving a no-CPU solution to capturing WS2812 waveform timing.  The CPU is used
to detect the 50-300us reset time between LED data frames, and to decode raw timing data
into pixels.

## Hardware

A resistor divider should be used to reduce the 5 volt signal level used by WS2812
to the 3.3V level needed.  Teensy 4.0 pins are not 5V tolerant, so direct connection
of a 5V signal could risk hardware damage.

Pins 2, 4, 5, 6, 8, 22, 23, 29 on Teensy 4.0 or Teensy 4.1 may be used to capture WS2812 data.

Pins 36, 49, 53, 54 are also supported on Teensy 4.1.

## Basic Functions

* `WS2812Capture myleds(Pin, PixelFormat)`

    Create a WS2812Capture instance which captures using a specific pin.  PixelFormat may be WS2812_RGB, WS2812_GRB, WS2812_RGBW, WS2812_GRBW, etc.  In theory, multiple instances may be created to capture parallel WS2812 data, but in practice how many parallel streams Teensy 4.0 can handle remains an unanswered question...
  
* `myleds.begin(buffer, sizeof(buffer))`

    Begin capturing, using buffer memory your program supplies.  WS2812Capture needs 99 bytes for each regular RGB LED, or 132 bytes for each RGBW LED.  Each incoming byte of LED data is captured as 8 low time measurements, 8 high time measurements (16 bits each), and 1 actual decoded byte.  If cached memory is used, aligning to cache row size (32 bytes on Teensy 4.x) will allow better performance.

* `myleds.available()`

    Check for new LED data arrival.  When a complete LED data frame has arrived, the non-zero number of bits is returned.  Otherwise 0 is returned when no data has arrived, or while a partial LED data frame is still incoming.  This function should be called as rapidly as possible, because it implements the checking for the reset timing between LED data frames.

* `myleds.numPixels()`

    Return the number of pixels actually received.

* `myleds.getPixel(index)`

    Return the RGB or RGBW data for a single pixel.  Data is always given in RGB or WRGB (white in upper 24 bits) format.

## Detailed Timing Functions

* `myleds.numBits()`

    Return the number of pixels actually received.  This function is essentially the same as numPixels(), but can indicate whether extra unnecessary bits were transmitted after those needed by the last LED.

* `myleds.bitHighNanoseconds(index)`

    Return the logic high time for a single bit, in nanoseconds.  Bits are indexed in the order actually received, not rearranged to PixelFormat as with getPixel(index).

* `myleds.bitLowNanoseconds(index)`

    Return the logic low time for a single bit, in nanoseconds.  The last bit of a WS2812 frame has undefined logic low time, because the low time for that last bit blends together with the logic low reset time between frames.

* `myleds.getResetMicroseconds()`

    Return the logic low reset time, in microseconds, which preceded the current LED data.


