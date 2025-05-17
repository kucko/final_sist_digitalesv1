#pragma once

class TemperatureRange {
private:
  float minTemp;
  float maxTemp;

public:
  TemperatureRange(float minTemp, float maxTemp)
      : minTemp(minTemp), maxTemp(maxTemp) {}

  bool isWithinRange(float value) const {
    return value >= minTemp && value <= maxTemp;
  }

  float getMin() const { return minTemp; }
  float getMax() const { return maxTemp; }
  void set(float min, float max) {
    minTemp = min;
    maxTemp = max;
  }
};
