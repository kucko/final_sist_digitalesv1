#pragma once
#include <MAX6675.h>

class Max6675SensorAdapter {
private:
  MAX6675 sensor;

public:
  Max6675SensorAdapter(int sck, int cs, int so)
      : sensor(sck, cs, so) {}

  float readTemperature() {
    return sensor.readCelsius();
  }
};
