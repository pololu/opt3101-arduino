#include "OPT3101.h"
#include <Wire.h>

static const uint32_t reg80Default = 0x4e1e;

void OPT3101::init()
{
  resetAndWait();
  if (getLastError()) { return; }
  setStandardRuntimeSettings();
  if (getLastError()) { return; }
  calibrateInternalCrosstalk();
  if (getLastError()) { return; }
}

void OPT3101::resetAndWait()
{
  digitalWrite(resetPin, 0);
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, 0);
  delayMicroseconds(50);
  pinMode(resetPin, INPUT);
  delay(5);

  // Wait for INIT_LOAD_DONE to be set, indicating that the OPT3101 is done
  // loading settings from its EEPROM.
  while (!(readReg(3) & (1 << 8)))
  {
    if (getLastError()) { return; }
    delay(1);
  }
}

// Sets these settings:
//  TG_OVL_WINDOW_START = 7000   (reg 0x89)
//  EN_TEMP_CONV = 1             (reg 0x6E)
//  CLIP_MODE_FC = 1             (reg 0x50)
//  CLIP_MODE_TEMP = 0           (reg 0x50)
//  CLIP_MODE_OFFSET = 0         (reg 0x50)
// To make things a little more reliable, we try to just write entire registers
// at a time, so we are making assumptions about what values we want in the
// other bits of those registers.
void OPT3101::setStandardRuntimeSettings()
{
  writeReg(0x89, 7000);      // TG_OVL_WINDOW_START = 7000
  writeReg(0x6e, 0x0a0000);  // EN_TEMP_CONV = 1
  writeReg(0x50, 0x200101);  // CLIP_MODE_* (see above)
}

void OPT3101::writeReg(uint8_t reg, uint32_t value)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write((uint8_t)(value >> 0));
  Wire.write((uint8_t)(value >> 8));
  Wire.write((uint8_t)(value >> 16));
  lastError = Wire.endTransmission();
}

uint32_t OPT3101::readReg(uint8_t reg)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  lastError = Wire.endTransmission(false); // no stop (repeated start)
  if (lastError)
  {
    return 0;
  }

  uint8_t byteCount = Wire.requestFrom(address, (uint8_t)3);
  if (byteCount != 3)
  {
    lastError = 50;
    return 0;
  }

  lastError = 0;
  uint32_t value = Wire.read();
  value |= (uint16_t)Wire.read() << 8;
  value |= (uint32_t)Wire.read() << 16;
  return value;
}

void OPT3101::setTxChannelAndHdr(uint8_t tx, uint8_t hdr)
{
  switch(tx)  // TODO: remove after I make sure it can compile
  {
    case OPT3101_TX0: break;
  }

  uint32_t reg2a = readReg(0x2a);

  if (tx == OPT3101_TX_SWITCH)
  {
    reg2a |= (1 << 1);  // EN_TX_SWITCH = 1
  }
  else
  {
    reg2a &= ~(1 << 1);  // EN_TX_SWITCH = 0
    reg2a = reg2a & ~(3 << 1) | (tx & 3) << 1;
  }

  if (hdr == OPT3101_ADAPTIVE_HDR)
  {
    reg2a |= (1 << 15);  // EN_ADAPTIVE_HDR = 1
  }
  else
  {
    reg2a &= ~(1 << 15);  // EN_ADAPTIVE_HDR = 0
    reg2a = reg2a & ~(1 << 16) | (hdr & 1) << 16;  // SEL_HDR_MODE = hdr
  }

  writeReg(0x2a, reg2a);
}

void OPT3101::setMonoshotMode(uint8_t frameCount)
{
  // MONOSHOT_FZ_CLKCNT = default
  // MONOSHOT_NUMFRAME = frameCount
  // MONOSHOT_MODE = 3
  writeReg(0x27, 0x26ac03 | (frameCount & 0x3f) << 2);

  // DIS_GLB_PD_OSC = 1
  // DIS_GLB_PD_AMB_DAC = 1
  // DIS_GLB_PD_REFSYS = 1
  // (other fields default)
  writeReg(0x76, 0x000121);

  // POWERUP_DELAY = 95
  writeReg(0x26, 95 << 10 | 0xF);
}

void setFrameTiming(uint16_t subFrameCount)
{

}


void OPT3101::calibrateInternalCrosstalk()
{
  // We call this just to disable adaptive HDR, which might cause issues.
  setTxChannelAndHdr(0, 0);

  setFrameTiming(4096);

  setMonoshotMode(5);

  uint32_t reg2e = readReg(0x2e);

  // USE_XTALK_FILT_INT = 1: Select the filtered IQ readings, not direct.
  reg2e |= (1 << 5)

  // USE_XTALK_REG_INT = 0: Use the internally calibrated value for
  // internal crosstalk.
  reg2e &= ~(1 << 6)

  // IQ_READ_DATA_SEL: Select internal crosstalk as the value we want to read
  // later from IPHASE_XTALK and QPHASE_XTALK.
  reg2e &= ~(7 << 9)

  writeReg(0x2e, reg2e);
  writeReg(0x80, reg80Default | 1);  // TG_EN = 1: Turn on timing generator.
  writeReg(0x2e, reg2e | (1 << 4));  // INT_XTALK_CALIB = 1.

  startMonoshotMeasurement();

  // Sleep for 5 frames. Each frame is 1024 ms.
  // Use a 5% margin in case the OPT3101 clock is running fast.
  delay(5376);  // 5 * 1024 * 1.05 = 5376

  writeReg(0x2e);
  writeReg(0x80, reg80Default);  // TG_EN = 0
}
