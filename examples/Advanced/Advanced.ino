#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

uint16_t amplitudes[3];
int16_t distances[3];

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

  sensor.setFrameTiming(512);
  sensor.setChannel(OPT3101Channel::TX0);
  sensor.setBrightness(OPT3101Brightness::Adaptive);

  sensor.startSample();
}

void loop()
{
  if (sensor.isSampleDone())
  {
    sensor.readOutputRegs();

    amplitudes[(uint8_t)sensor.channel] = sensor.amplitude;
    distances[(uint8_t)sensor.channel] = sensor.distanceMillimeters;

    if (sensor.channel == OPT3101Channel::TX2)
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
    sensor.nextChannel();
    sensor.startSample();
  }
}
