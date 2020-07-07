#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

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
  sensor.setChannel(OPT3101Channel::TX1);
  sensor.setBrightness(OPT3101Brightness::Adaptive);
}

void loop()
{
  sensor.sample();

  Serial.print(sensor.temperature);
  Serial.print(',');
  Serial.print(sensor.ambient);
  Serial.print(',');
  Serial.print(sensor.i);
  Serial.print(',');
  Serial.print(sensor.q);
  Serial.print(',');
  Serial.print(sensor.amplitude);
  Serial.print(',');
  Serial.print(sensor.distanceMillimeters);
  Serial.println();
}
