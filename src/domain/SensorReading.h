#pragma once

struct SensorReading {
  unsigned long timestamp;
  float rawValue;
  float calibratedValue;
  bool isValid;
};
