// Copyright (C) Pololu Corporation.  See LICENSE.txt for details.

#pragma once

#include <Arduino.h>

enum class OPT3101Brightness : uint8_t {
  Low = 0,
  High = 1,
  Adaptive = 255
};

const uint8_t OPT3101ChannelAutoSwitch = 255;

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
  void configureDefault();
  void setChannel(uint8_t channel);
  void nextChannel();
  void setBrightness(OPT3101Brightness);
  void setMonoshotMode();
  void setContinuousMode();
  void setFrameTiming(uint16_t subFrameCount);
  void enableTimingGenerator();
  void disableTimingGenerator();
  void enableDataReadyOutput(uint8_t gpPin);
  void startSample();
  bool isSampleDone();
  void readOutputRegs();
  void sample();

  uint8_t channelUsed = 0;
  OPT3101Brightness brightnessUsed;
  uint16_t ambient;
  uint16_t temperature;
  int32_t i, q;
  uint16_t amplitude;
  int16_t phase;
  int16_t distanceMillimeters;

private:
  bool timingGeneratorEnabled = false;
  uint8_t lastError = 0;
  uint8_t address = 0x58;
  uint16_t startSampleTimeMs;
  uint16_t frameDelayTimeMs;
};
