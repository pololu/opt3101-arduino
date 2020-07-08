# OPT3101 library for Arduino

Version: 1.0.0<br>
Release date: 2020-07-15<br>
[![Build Status](https://travis-ci.org/pololu/opt3101-arduino.svg?branch=master)](https://travis-ci.org/pololu/opt3101-arduino)<br>
[www.pololu.com](https://www.pololu.com/)

## Summary

This is a library that helps interface with the OPT3101 time-of-flight-based
distance sensor.

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

* `uint8_t getAddress()`<br>
  Returns the current I&sup2;C address.

* `uint8_t getLastError()`<br>
  Returns a non-zero error code if there was an error communicating with the
  OPT3101.
  Every function in this library that communicates sets the error code returned
  by this function to zero on success or non-zero error code if it fails.

*
