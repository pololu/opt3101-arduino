#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

void setup()
{
  Wire.begin();
  sensor.init();

  while (!Serial) {}

  if (sensor.getLastError())
  {
    Serial.print(F("Failed to initialize sensor: error "));
    Serial.println(sensor.getLastError());
    while (1) {}
  }

  sensor.setFrameTiming(512);
  sensor.setChannelAndBrightness(OPT3101Channel::TX1);
  sensor.startTimingGenerator();
}

void loop()
{
  sensor.monoshotAndRead();

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
