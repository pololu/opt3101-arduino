#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

const uint8_t eepromAddress = 0x50;

// irs15j_a: Generic calibration for irs15j boards.
const uint8_t image[] PROGMEM = "\x0b\x09\x00\x10\x0c\x00\x00\xbf\x29\xe4\x93\x2f\x2a\x20\x49\x7c\x2b\x78\x69\x00\x2c\xa7\x0b\x00\x2d\xbb\xe4\x5e\x2e\xa0\x01\x84\x2f\x67\xf5\x4c\x30\x7d\x0b\x20\x31\xc7\x0b\x5f\x32\x6e\x5d\xb0\x33\x49\xf0\x4b\x34\xe2\xf9\x60\x35\x6a\xcc\x00\x36\xc2\xc9\x00\x37\x1c\xe5\x00\x38\xeb\xf6\x29\x39\x3d\x99\xf1\x3a\x32\xa5\x4d\x3f\xc8\x08\x00\x41\x10\x80\x8c\x42\xe6\x3c\x00\x43\x83\x00\x00\x45\xdd\x85\x8c\x47\xc8\x08\x80\x48\xc8\x08\x00\x49\xc8\x08\x00\x51\x4c\x34\x00\x52\x6a\x3d\x00\x53\xaf\x34\x00\x54\x11\x3d\x00\x55\xd3\x34\x00\x5e\x00\x63\x13\x5f\xf8\x16\xbb\x60\x1d\xea\xd8\x61\xdf\x88\x00\x72\x50\x00\x00\x85\x7a\x26\x00\x86\x82\x26\x00\xb4\xd3\xe4\xe8\xb5\x01\x00\x00\xb8\x1a\x6c\x01\xb9\xa6\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";

// Calibration for David's irs15j_8 board.
// const uint8_t image[] PROGMEM = "\x0B\x09\x00\x10\x0C\x00\x00\xBF\x29\xE4\x93\x2F\x2A\x20\x49\x7C\x2B\x78\x69\x00\x2C\xA7\x0B\x00\x2D\xBB\xE4\x5E\x2E\xA0\x01\x84\x2F\x0B\xF6\x4C\x30\xF8\x0C\x20\x31\x1D\x17\x5F\x32\x47\x63\xB0\x33\x4C\xED\x4B\x34\x52\xF8\x60\x35\xC8\xBF\x00\x36\x51\xC1\x00\x37\x6D\xE6\x00\x38\x96\xF4\x29\x39\xC7\x99\xF1\x3A\x64\x98\x4D\x3F\xC8\x08\x00\x41\x10\x80\x8C\x42\x90\x3C\x00\x43\x83\x00\x00\x45\xDD\x85\x8C\x47\xC8\x08\x80\x48\xC8\x08\x00\x49\xC8\x08\x00\x51\x66\x34\x00\x52\x4B\x3C\x00\x53\x0B\x34\x00\x54\x71\x3D\x00\x55\x5B\x34\x00\x5E\x00\x63\x13\x5F\xF8\x16\xBB\x60\x1D\xEA\xD8\x61\xDF\x88\x00\x72\x50\x00\x00\x85\x7A\x26\x00\x86\x82\x26\x00\xB4\xD3\xE4\xE8\xB5\x01\x00\x00\xB8\x1A\x6C\x01\xB9\xA6\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

void dumpEEPROM()
{
  // Set the address to 0.
  Wire.beginTransmission(eepromAddress);
  Wire.write(0);
  uint8_t error = Wire.endTransmission();
  if (error)
  {
    Serial.println(F("Failed to set EEPROM address."));
    while(1) {}
  }

  Serial.print('"');
  uint16_t index = 0;
  while (index < 256)
  {
    uint8_t byteCount = Wire.requestFrom(eepromAddress, (uint8_t)16);
    if (byteCount != 16)
    {
      Serial.println(F("\nFailed to read EEPROM."));
      while(1) {}
    }
    for (uint8_t i = 0; i < 16; i++)
    {
      uint8_t value = Wire.read();
      Serial.print(F("\\x"));
      if (value < 0x10) { Serial.print('0'); }
      Serial.print(value, HEX);
      index++;
    }
  }
  Serial.println(F("\";"));
}

void verifyEEPROM()
{
  uint8_t * ptr = image;

  // Set the address to 0.
  Wire.beginTransmission(eepromAddress);
  Wire.write(0);
  uint8_t error = Wire.endTransmission();
  if (error)
  {
    Serial.println(F("Failed to set EEPROM address."));
    while(1) {}
  }

  uint16_t index = 0;
  while(index < 256)
  {
    uint8_t byteCount = Wire.requestFrom(eepromAddress, (uint8_t)16);
    if (byteCount != 16)
    {
      Serial.println(F("\nFailed to read EEPROM."));
      while(1) {}
    }
    for (uint8_t i = 0; i < 16; i++)
    {
      uint8_t expected = pgm_read_byte(&image[index]);
      uint8_t value = Wire.read();
      if (value != expected)
      {
        Serial.print(F("Verification failed at 0x"));
        Serial.print(index, HEX);
        Serial.print(F(": expected 0x"));
        Serial.print(expected, HEX);
        Serial.print(F(" got 0x"));
        Serial.print(value, HEX);
        Serial.println('.');
        while(1) {}
      }
      index++;
    }
  }
  Serial.println(F("Verified EEPROM."));
}

void writeEEPROM()
{
  uint16_t index = 0;
  while(index < 256)
  {
    Wire.beginTransmission(eepromAddress);
    Wire.write(index);
    for (uint8_t i = 0; i < 16; i++)
    {
      uint8_t value = pgm_read_byte(&image[index]);
      Wire.write(value);
      index++;
    }
    uint8_t lastError = Wire.endTransmission();
    if (lastError)
    {
      Serial.println(F("Failed to write EEPROM."));
      while (1) {}
    }
    delay(6);
  }
  Serial.println(F("Wrote EEPROM."));
}

void setup()
{
  Wire.begin();
  sensor.resetAndWait();

  while (!Serial) {}

  if (sensor.getLastError())
  {
    Serial.print(F("Failed to initialize OPT3101: error "));
    Serial.println(sensor.getLastError());
    while (1) {}
  }

  // FORCE_EN_BYPASS = 1: Lets us talk to the EEPROM.
  sensor.writeReg(0x00, 0x200000);

  //dumpEEPROM();
  writeEEPROM();
  verifyEEPROM();
}

void loop()
{
}