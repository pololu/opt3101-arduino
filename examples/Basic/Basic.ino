// This example shows basic usage of the OPT3101 library.

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

  // This tells the OPT3101 how many readings to average
  // together when it takes a sample.  Each reading takes
  // 0.25 ms, so getting 256 takes about 64 ms.
  // The library adds an extra 6% margin of error, making
  // it 68 ms.  You can specify any power of 2 between
  // 1 and 4096.
  sensor.setFrameTiming(256);

  // 1 means to use TX1, the middle channel.
  sensor.setChannel(1);

  // Adaptive means to automatically choose high or low brightness.
  // Other options you can use here are High and Low.
  sensor.setBrightness(OPT3101Brightness::Adaptive);
}

void loop()
{
  sensor.sample();

  Serial.print(sensor.channelUsed);
  Serial.print(',');
  Serial.print((uint8_t)sensor.brightnessUsed);
  Serial.print(',');
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
