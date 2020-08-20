// This example shows how to read the contents of the EEPROM
// connected to an OPT3101.

#include <OPT3101.h>
#include <Wire.h>

OPT3101 sensor;

const uint8_t eepromAddress = 0x50;

// Metadata found in the EEPROM (e.g. serial number).
uint8_t meta[30];
uint8_t metaSize = 0;

void dumpEEPROM()
{
  bool metadataDone = false;

  // Set the address to 0.
  Wire.beginTransmission(eepromAddress);
  Wire.write(0);
  uint8_t error = Wire.endTransmission();
  if (error)
  {
    Serial.println(F("Failed to set EEPROM address."));
    while(1) {}
  }

  Serial.println(F("EEPROM contents as a C string:"));
  Serial.print('"');
  uint16_t index = 0;
  while (index < 256)
  {
    uint8_t byteCount = Wire.requestFrom(eepromAddress, (uint8_t)4);
    if (byteCount != 4)
    {
      Serial.println(F("\nFailed to read EEPROM."));
      while(1) {}
    }

    // Read a chunk of 4 bytes and print those bytes to the serial terminal.
    uint8_t chunk[4];
    for (uint8_t i = 0; i < 4; i++)
    {
      uint8_t value = chunk[i] = Wire.read();
      Serial.print(F("\\x"));
      if (value < 0x10) { Serial.print('0'); }
      Serial.print(value, HEX);
      index++;
    }

    // See if the chunk contains metadata.
    if (!metadataDone)
    {
      if (chunk[0] == 0xB4)
      {
        if (metaSize + 3 <= sizeof(meta))
        {
          meta[metaSize++] = chunk[1];
          meta[metaSize++] = chunk[2];
          meta[metaSize++] = chunk[3];
        }
      }
      else
      {
        metadataDone = true;
      }
    }
  }
  Serial.println(F("\";"));
  Serial.println();
}

void printSerialNumber(uint8_t * p)
{
  char buffer[16];
  sprintf_P(buffer, PSTR("%02X-%02X-%02X-%02X"), p[0], p[1], p[2], p[3]);
  Serial.print("Serial number: ");
  Serial.println(buffer);
}

void interpretMetadata()
{
  if (metaSize == 0)
  {
    Serial.println(F("No metadata."));
    return;
  }

  if (meta[0] == 0xF0 && meta[1] == 0xB2 && meta[2] == 0xF1 && metaSize >= 9)
  {
    printSerialNumber(meta + 3);
  }
  else
  {
    Serial.println(F("Unrecognized metadata."));
  }
}

void setup()
{
  Wire.begin();

  while (!Serial) {}

  sensor.resetAndWait();

  // FORCE_EN_BYPASS = 1: Lets us talk to the EEPROM.
  sensor.writeReg(0x00, 0x200000);

  dumpEEPROM();
  interpretMetadata();
}

void loop()
{
}
