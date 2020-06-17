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

  uint32_t readReg(uint8_t reg);
  void writeReg(uint8_t reg, uint32_t value);

  void init();
  void resetAndWait();
  void setStandardRuntimeSettings();
  void setChannelAndBrightness(OPT3101Channel ch, OPT3101Brightness br = OPT3101Brightness::Adaptive);
private:
  void setMonoshotMode(uint8_t frameCount = 1);
public:
  void setFrameTiming(uint16_t subFrameCount);
  void startMonoshotMeasurement();
  void calibrateInternalCrosstalk();
  void startTimingGenerator();
  void readOutputRegs();
  void monoshotAndRead();

  uint16_t ambient;
  uint16_t temperature;
  int32_t i, q;
  uint16_t amplitude;
  int16_t phase;
  int16_t distanceMillimeters;

  uint16_t frameDelayTimeMs;

protected:
  uint8_t lastError = 0;

private:
  uint8_t address = 0x58;
};
