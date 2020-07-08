#include "OPT3101.h"
#include <Wire.h>

static const uint32_t reg80Default = 0x4e1e;

void OPT3101::resetAndWait()
{
  // Set SOFTWARE_RESET to 1, but don't use writeReg, because the OPT3101 will
  // stop acknowledging after it receives the first register value byte.
  Wire.beginTransmission(address);
  Wire.write(0);
  Wire.write(1);
  lastError = Wire.endTransmission();
  if (lastError) { return; }

  // Give the sensor some time to get ready.
  delay(5);

  // Wait for INIT_LOAD_DONE to be set, indicating that the OPT3101 is done
  // loading settings from its EEPROM.
  while (!(readReg(3) & (1 << 8)))
  {
    if (getLastError()) { return; }
    delay(1);
  }
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

void OPT3101::init()
{
  resetAndWait();
  if (getLastError()) { return; }
  setStandardRuntimeSettings();
  if (getLastError()) { return; }
  setMonoshotMode();
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
  if (getLastError()) { return; }
  writeReg(0x6e, 0x0a0000);  // EN_TEMP_CONV = 1
  if (getLastError()) { return; }
  writeReg(0x50, 0x200101);  // CLIP_MODE_FC = 1 (see above)
                             // CLIP_MODE_TEMP = 0
                             // CLIP_MODE_OFFSET = 0
  if (getLastError()) { return; }

  // IQ_READ_DATA_SEL = 2: This lets us read IQ values later.
  uint32_t reg2e = readReg(0x2e);
  if (getLastError()) { return; }
  reg2e = (reg2e & ~(7 << 9)) | (2 << 9);
  writeReg(0x2e, reg2e);
  if (getLastError()) { return; }

  setFrameTiming(512);
}

void OPT3101::setChannel(OPT3101Channel ch)
{
  uint32_t reg2a = readReg(0x2a);
  if (getLastError()) { return; }

  reg2a &= ~((uint32_t)1 << 1);  // EN_TX_SWITCH = 0
  reg2a = reg2a & ~((uint32_t)3 << 1) | ((uint8_t)ch & 3) << 1;

  writeReg(0x2a, reg2a);
}

void OPT3101::nextChannel()
{
  if (channel == OPT3101Channel::TX0) { setChannel(OPT3101Channel::TX1); }
  else if (channel == OPT3101Channel::TX1) { setChannel(OPT3101Channel::TX2); }
  else { setChannel(OPT3101Channel::TX0); }
}

void OPT3101::setBrightness(OPT3101Brightness br)
{
  uint32_t reg2a = readReg(0x2a);
  if (getLastError()) { return; }

  if (br == OPT3101Brightness::Adaptive)
  {
    reg2a |= (1 << 15);  // EN_ADAPTIVE_HDR = 1
  }
  else
  {
    // EN_ADAPTIVE_HDR = 0
    // SEL_HDR_MODE = hdr
    reg2a = reg2a & ~0x18000 | ((uint32_t)br & 1) << 16;
  }

  writeReg(0x2a, reg2a);
}

void OPT3101::setMonoshotMode(uint8_t frameCount)
{
  // MONOSHOT_FZ_CLKCNT = default
  // MONOSHOT_NUMFRAME = frameCount
  // MONOSHOT_MODE = 3
  writeReg(0x27, 0x26ac03 | (frameCount & 0x3f) << 2);
  if (getLastError()) { return; }

  // DIS_GLB_PD_OSC = 1
  // DIS_GLB_PD_AMB_DAC = 1
  // DIS_GLB_PD_REFSYS = 1
  // (other fields default)
  writeReg(0x76, 0x000121);
  if (getLastError()) { return; }

  // POWERUP_DELAY = 95
  writeReg(0x26, (uint32_t)95 << 10 | 0xF);
  if (getLastError()) { return; }
}

void OPT3101::setFrameTiming(uint16_t subFrameCount)
{
  // Make sure subFrameCount is a power of 2 between 1 and 4096.
  if (subFrameCount < 1 || subFrameCount > 4096 ||
    subFrameCount & (subFrameCount - 1))
  {
    subFrameCount = 4096;
  }

  // Implement equation 6 from sbau310.pdf to calculate
  // XTALK_FILT_TIME CONST, but without floating-point operations.
  uint8_t timeConst = 0;
  while ((subFrameCount << timeConst) < 1024) { timeConst++; }

  uint32_t reg2e = readReg(0x2e);
  if (getLastError()) { return; }
  reg2e = reg2e & ~0xF00000 | (uint32_t)timeConst << 20;
  writeReg(0x2e, reg2e);

  // Set NUM_SUB_FRAMES and NUM_AVG_SUB_FRAMES.
  writeReg(0x9f, (subFrameCount - 1) | (uint32_t)(subFrameCount - 1) << 12);
  if (getLastError()) { return; }

  // Set TG_SEQ_INT_MASK_START and TG_SEQ_INT_MASK_END according to what
  // the OPT3101 datasheet says, but it's probably not needed.
  writeReg(0x97, (subFrameCount - 1) | (uint32_t)(subFrameCount - 1) << 12);

  // Assuming that SUB_VD_CLK_CNT has not been changed, each sub-frame is
  // 0.25 ms.  The +3 is to make sure we round up.
  uint16_t frameTimeMs = (subFrameCount + 3) / 4;

  // Add a ~6% margin in case the OPT3101 clock is running faster.
  frameDelayTimeMs = frameTimeMs + (frameTimeMs + 15) / 16;
}

void OPT3101::enableTimingGenerator()
{
  writeReg(0x80, reg80Default | 1);  // TG_EN = 1
  timingGeneratorEnabled = true;
}

void OPT3101::disableTimingGenerator()
{
  writeReg(0x80, reg80Default);  // TG_EN = 0
  timingGeneratorEnabled = false;
}

void OPT3101::startSample()
{
  if (!timingGeneratorEnabled) { enableTimingGenerator(); }
  // Set MONOSHOT_BIT to 0 before setting it to 1, as recommended here:
  // https://e2e.ti.com/support/sensors/f/1023/p/756598/2825649#2825649
  writeReg(0x00, 0x000000);
  writeReg(0x00, 0x800000);
  startSampleTimeMs = millis();
}

bool OPT3101::isSampleDone()
{
  return (uint16_t)(millis() - startSampleTimeMs) > frameDelayTimeMs;
}

void OPT3101::readOutputRegs()
{
  uint32_t reg08 = readReg(0x08);
  uint32_t reg09 = readReg(0x09);
  uint32_t reg0a = readReg(0x0a);

  channel = (OPT3101Channel)(reg08 >> 18 & 3);
  if (channel > OPT3101Channel::TX2) { channel = OPT3101Channel::TX2; }
  brightness = (OPT3101Brightness)(reg08 >> 17 & 1);

  i = readReg(0x3b);
  if (i > 0x7fffff) { i -= 0x1000000; }
  q = readReg(0x3c);
  if (q > 0x7fffff) { q -= 0x1000000; }

  amplitude = reg09 & 0xFFFF;  // AMP_OUT
  phase = reg08 & 0xFFFF;  // PHASE_OUT

  // c / (2 * 10 MHz * 0x10000) = 0.22872349395 mm ~= 14990/0x10000
  distanceMillimeters = (uint32_t)phase * 14990 >> 16;

  ambient = reg0a >> 2 & 0x3FF;  // AMB_DATA

  temperature = reg0a >> 12 & 0xFFF;  // TMAIN
}

void OPT3101::sample()
{
  startSample();
  delay(frameDelayTimeMs);
  readOutputRegs();
}
