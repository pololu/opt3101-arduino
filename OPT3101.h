#pragma once

#include <Arduino.h>

// TODO: should probably make enum classes or something for the TX/HDR constants
static const uint8_t
  OPT3101_NO_PIN = 255,
  OPT3101_TX_SWITCH = 255,
  OPT3101_TX0 = 0,
  OPT3101_TX1 = 1,
  OPT3101_ADAPTIVE_HDR = 255,
  OPT3101_HDR0 = 0,
  OPT3101_HDR1 = 1;

class OPT3101
{
public:
  void setAddress(uint8_t address) { this->address = address; }
  uint8_t getAddress() { return address; }

  void setResetPin(uint8_t pin) { this->resetPin = pin; }
  uint8_t getResetPin() { return resetPin; }

  uint8_t getLastError() { return lastError; };

  uint32_t readReg(uint8_t reg);
  void writeReg(uint8_t reg, uint32_t value);

  void init();
  void resetAndWait();
  void setStandardRuntimeSettings();
  void setTxChannelAndHdr(uint8_t tx, uint8_t hdr);
  void setMonoshotMode(uint8_t frameCount = 1);
  void setFrameTiming(uint16_t subFrameCount);
  void startMonoshotMeasurement();
  void calibrateInternalCrosstalk();
  void startTimingGenerator();
  void readOutputRegs();
  void monoshotAndRead();

  int32_t i, q;
  uint16_t amplitude;
  int16_t phase;

  uint16_t frameDelayTimeMs;

protected:
  uint8_t lastError = 0;

private:
  uint8_t address = 0x58;
  uint8_t resetPin = OPT3101_NO_PIN;
};
