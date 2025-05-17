#pragma once
#include "domain/SensorReading.h"
#include "domain/TemperatureRange.h"

class ProcessReadingUseCase {
private:
  TemperatureRange* range;

public:
  ProcessReadingUseCase(TemperatureRange* r) : range(r) {}

  bool execute(SensorReading reading) {
    return range->isWithinRange(reading.calibratedValue);
  }
};
