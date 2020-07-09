# OPT3101 library for Arduino

Version: 1.0.0<br>
Release date: 2020-07-15<br>
[![Build Status](https://travis-ci.org/pololu/opt3101-arduino.svg?branch=master)](https://travis-ci.org/pololu/opt3101-arduino)<br>
[www.pololu.com](https://www.pololu.com/)

## Summary

This is a library that helps interface with the OPT3101 time-of-flight-based
distance sensor from Texas Instruments.

## Supported platforms

This library is designed to work with the Arduino IDE version 1.8.x or later;
we have not tested it with earlier versions.
This library should support any Arduino-compatible board, including the
[Pololu A-Star controllers](https://www.pololu.com/category/149/a-star-programmable-controllers).

## Getting started

### Hardware

An [OPT3101 carrier](TODO)
can be purchased from Pololu's website.
Before continuing, careful reading of the
[product page](TODO) is recommended.

Make the following connections between the Arduino and the OPT3101 board:

#### 5V Arduino boards

(including Arduino Uno, Leonardo, Mega; Pololu A-Star 32U4)

    Arduino   OPT3101 board
    -------   -------------
         5V - VIN
        GND - GND
        SDA - SDA
        SCL - SCL

#### 3.3V Arduino boards

(including Arduino Due)

    Arduino   OPT3101 board
    -------   -------------
        3V3 - VIN
        GND - GND
        SDA - SDA
        SCL - SCL


### Software

You can use the Library Manager in the [Arduino software (IDE)](http://www.arduino.cc/en/Main/Software) to install this library:

1. In the Arduino IDE, open the "Sketch" menu, select "Include Library", then
   "Manage Libraries...".
2. Search for "OPT3101".
3. Click the OPT3101 entry in the list.
4. Click "Install".

If this does not work, you can manually install the library:

1. Download the [latest release archive from GitHub](https://github.com/pololu/opt3101-arduino/releases) and decompress it.
2. Rename the folder "opt3101-arduino-master" to "OPT3101".
3. Move the "OPT3101" folder into the "libraries" directory inside your
   Arduino sketchbook directory.
   You can view your sketchbook location by opening the "File" menu and
   selecting "Preferences" in the Arduino IDE.
   If there is not already a "libraries" folder in that location, you should
   make the folder yourself.
4. After installing the library, restart the Arduino IDE.

## Examples

Several example sketches are available that show how to use the library
You can access them from the Arduino IDE by opening the "File" menu,
selecting "Examples", and then selecting "OPT3101". If you cannot find these
examples, the library was probably installed incorrectly and you should retry
the installation instructions above.

## Library reference

* `OPT3101()`<br>
  Constructor for the OPT3101 class.

* `void setAddress(uint8_t new_addr)`<br>
  Changes the I&sup2;C slave device address of the OPT3101 to the given value (7-bit).
  The library uses an address of 0x58 by default if you don't call this
  function.

* `uint8_t getAddress()`<br>
  Returns the current I&sup2;C address.

* `uint8_t getLastError()`<br>
  Returns a non-zero error code if there was an error communicating with the
  OPT3101.
  Every function in this library that communicates with the OPT3101 sets the
  error code returned by this function to zero on success or non-zero error
  code if it fails.

* `void resetAndWait()`<br>
  Resets the OPT3101 and then waits for the INIT_LOAD_DONE to be 1.

* `void writeReg(uint8_t reg, uint32_t value)`<br>
  Writes the given value to the an OPT3101 register.
  The OPT3101 registers are 24-bit, so the upper 8 bits of `value` are ignored.

* `uint32_t readReg(uint8_t reg);`<br>
  Reads an OPT3101 register and returns the value read.

* `void init();`<br>
  Initializes and configures the sensor.
  This includes calling `resetAndWait()` and `configureDefault()`.

* `void configureDefault();`<br>
  Configures the sensor with some default settings.
  This includes calling `setMonoshotMode()` and `setFrameTiming(512)`.

* `void setChannel(uint8_t channel);`<br>
  Configures the sensor to use the specified channel.
  This determines which set of infrared LEDs will be turned on while taking
  a reading.
  The valid values for `channel` are 0, 1, and 2, which correspond to channels
  TX0, TX1, and TX2 respectively.

* `void nextChannel();`<br>
  This convenience method configures the sensor to use the channel that comes
  numerically after the last channel that was sampled
  (according to the `channelUsed` member).

* `void setBrightness(OPT3101Brightness brightness)`<br>
  Sets the OPT3101 to use the specified LED brightness.
  Valid arguments are:
  - `OPT3101Brightness::Low` (0)
  - `OPT3101Brightness::High` (1)
  - `OPT3101Brightness::Adaptive` (255)
  Low brightness mode only works well for nearby objects (e.g. within about
  10 cm).
  High brightness mode works for a wide range of objects, but objects that
  are too reflective or too close can cause the sensor to saturate, meaning
  that it fails to measure a distance.
  Adaptive mode automatically uses low or high brightness depending on how
  much reflected light the sensor is seeing.

* `void setMonoshotMode();`<br>
  Configures the sensor to use monoshot mode, which means it only turns on the
  LEDs and takes measurements when we request it to.
  This function is called by `configureDefault()` which is called by `init()`
  so most applications will not need to use it directly.

* `void setFrameTiming(uint16_t subFrameCount);`<br>
  Configures the OPT3101 to average the specified number of sub-frames.
  Each frame of the OPT3101 (i.e. sample) consists of a certain number of
  sub-frames which take 0.25 ms each.
  The OPT3101 averages the results from these subframes together.
  Choosing a high number here makes the samples take longer, but could
  give increased accuracy.
  `subFrameCount` can be any power of 2 between 1 and 4096, but it should be
  at least 2 if you are using adaptive brightness.

* `void enableTimingGenerator();`<br>
  Enables the timing generator by setting the `TG_EN` bit to 1.
  This is automatically called by `startSample()` so most applications will not
  need to use it directly.

* `void disableTimingGenerator();`<br>
  Disables the timing generator by setting the `TG_EN` bit to 0.

* `void startSample();`<br>
  Tells the sensor to start taking a sample.
  The sensor will start emitting light from its LEDs according to the
  previosuly-specified settings.
  This calls `enableTimingGenerator()` if that has not been done already.

* `bool isSampleDone();`<br>
  Returns true if enough time has elapsed so that the sample started by
  `startSample()` is expected to be complete.
  This function only uses Arduino timing functions; it does not communicate
  with the OPT3101, and does not report any errors using `getLastError()`.

* `void readOutputRegs();`<br>
  Reads the sensor's output registers updates the data in the following
  member variables:
  `channelUsed`, `brightnessUsed`, `i`, `q`, `amplitude`, `phase`,
  `distanceMillimeters`, `ambient`, `temperature`.

* `void sample();`<br>
  This calls `startSample()`, then delays until the sample is complete, then
  called `readOutputRegs()` to read the results from the sample.

* Sample results:

  * `uint8_t channelUsed`<br>
    The channel used in the last sample.
    The library ensures that this number will be 0, 1, or 2,
    so that you can safely use it as an index to a 3-element array.
  * `OPT3101Brightness brightnessUsed;`<br>
    The brightness used in the last sample.  This will either be
    `OPT3101Brightness::Low` (0) or `OPT3101Brightness::High` (0)
  * `uint16_t ambient;`<br>
    The level of ambient light detected in the last sample.
    The OPT3101 uses this internally to adjust its data.
  * `uint16_t temperature;`<br>
    The temperature measured during the last sample.
    The OPT3101 uses this internally to adjsust its data.
  * `int32_t i, q;`<br>
    These two numbers come from reading the IPHASE_XTALK and QPHASE_XTALK
    registers from the OPT3101 while IQ_READ_DATA_SEL is 2.
    They represent a measurement of the reflected infrared light, and its
    decomposition into two signals: a signal that is in phase with the
    emitted wave (`i`) and a signal that is 90 degrees out of phase (`q`).
  * `uint16_t amplitude;`<br>
    The intensity of the reflected light measured in the last sample.
    The lower the amplitude, the less reliable the phase/distance readings will
    be.
    An amplitude of 65535 (0xFFFF) indicates that the sensor
    was saturated by receiving too much light.
    If this happens, `phase` and `distanceMillimeters` will both be 0.
  * `int16_t phase;`<br>
    This is the distance measured by the OPT3101 in the last sample, in
    units of 0.228723 mm.
    This is read from the PHASE_OUT register.
  * `int16_t distanceMillimeters;`<br>
    This is the distance measured by the OPT3101 in the last sample,
    converted to millimeters.
    Please note that it is normal for this reading&mdash;and all the other
    readings documented here&mdash;to have noise, even when your system is
    not moving.
    The noise is typically lower for objects that are closer to the sensor
    or more reflective.

## Version history

* 1.0.0 (2020-07-15): Original release.
