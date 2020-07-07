#pragma once

#include <Arduino.h>

enum class OPT3101Channel {
  TX0 = 0,
  TX1 = 1,
  TX2 = 2,
  Switch = 255
};

enum class OPT3101Brightness {
  Low = 0,
  High = 1,
  Adaptive = 255
};

class OPT3101
{
public:
  void setAddress(uint8_t address) { this->address = address; }
  uint8_t getAddress() { return address; }

  uint8_t getLastError() { return lastError; };

  void resetAndWait();
  void writeReg(uint8_t reg, uint32_t value);
  uint32_t readReg(uint8_t reg);

  void init();
  void setStandardRuntimeSettings();
  void setChannel(OPT3101Channel);
  void setBrightness(OPT3101Brightness);
  void setMonoshotMode(uint8_t frameCount = 1);
  void setFrameTiming(uint16_t subFrameCount);
  void enableTimingGenerator();
  void disableTimingGenerator();
  void startMonoshotSample();
  void readOutputRegs();
  void sample();

  uint16_t ambient;
  uint16_t temperature;
  int32_t i, q;
  uint16_t amplitude;
  int16_t phase;
  int16_t distanceMillimeters;

  uint16_t frameDelayTimeMs;

private:
  bool timingGeneratorEnabled = false;
  uint8_t lastError = 0;
  uint8_t address = 0x58;
};
