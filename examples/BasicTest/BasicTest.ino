#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

void setup()
{
  Wire.begin();

  sensor.setResetPin(4);
  sensor.init();
}

void loop()
{
  sensor.writeReg(0xa3, 0x7a8923);
  Serial.println(sensor.readReg(0xa3), HEX);  // expect 8601a0
  delay(1000);
}
