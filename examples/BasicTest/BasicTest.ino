#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

void setup()
{
  Wire.begin();

  sensor.setResetPin(4);
  sensor.init();
  if (sensor.getLastError())
  {
    Serial.print("Failed to initialize sensor: code ");
    Serial.println(sensor.getLastError());
    while (1) {}
  }

  sensor.setFrameTiming(4096);
  sensor.setTxChannelAndHdr(OPT3101_TX1, OPT3101_HDR1);
  sensor.startTimingGenerator();
}

void loop()
{
  sensor.monoshotAndRead();
  Serial.print(sensor.i);
  Serial.print(',');
  Serial.print(sensor.q);
  Serial.print(',');
  Serial.print(sensor.amplitude);
  Serial.print(',');
  Serial.print(sensor.phase);
  Serial.println();
}
