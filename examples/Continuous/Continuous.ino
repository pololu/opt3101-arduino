// This example shows how to use non-blocking code to read
// from all three channels on the OPT3101 continuously
// and store the results in arrays.
//
// Each channel is sampled approximately 41 times per second.
//
// In addition to the usual power and I2C connections, you
// will need to connect the GPIO1 pin to an interrupt on your
// Arduino.  For ATmega32U4-based Arduinos, use pin 7.
// For other Arduinos, use pin 2.  To use a different pin,
// change the definition of dataReadyPin below, and refer to
// the documentation of attachInterrupt to make sure you
// pick a valid pin.

#include <OPT3101.h>
#include <Wire.h>

#ifdef __AVR_ATmega32U4__
const uint8_t dataReadyPin = 7;
#else
const uint8_t dataReadyPin = 2;
#endif

OPT3101 sensor;

uint16_t amplitudes[3];
int16_t distances[3];
volatile bool dataReady = false;

void setDataReadyFlag()
{
  dataReady = true;
}

void setup()
{
  Wire.begin();

  // Wait for the serial port to be opened before printing
  // messages (only applies to boards with native USB).
  while (!Serial) {}

  sensor.init();
  if (sensor.getLastError())
  {
    Serial.print(F("Failed to initialize OPT3101: error "));
    Serial.println(sensor.getLastError());
    while (1) {}
  }
  sensor.setContinuousMode();
  sensor.enableDataReadyOutput(1);
  sensor.setFrameTiming(32);
  sensor.setChannel(OPT3101ChannelAutoSwitch);
  sensor.setBrightness(OPT3101Brightness::Adaptive);

  attachInterrupt(digitalPinToInterrupt(dataReadyPin), setDataReadyFlag, RISING);
  sensor.enableTimingGenerator();
}

void loop()
{
  if (dataReady)
  {
    sensor.readOutputRegs();

    amplitudes[sensor.channelUsed] = sensor.amplitude;
    distances[sensor.channelUsed] = sensor.distanceMillimeters;

    if (sensor.channelUsed == 2)
    {
      for (uint8_t i = 0; i < 3; i++)
      {
        Serial.print(amplitudes[i]);
        Serial.print(',');
        Serial.print(distances[i]);
        Serial.print(", ");
      }
      Serial.println();
    }
    dataReady = false;
  }
}
