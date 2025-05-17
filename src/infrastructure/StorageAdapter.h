#pragma once
#include "domain/SensorReading.h"

#define MAX_REGISTROS 1080

class StorageAdapter {
private:
  SensorReading buffer[MAX_REGISTROS];
  int currentIndex = 0;

public:
  void store(const SensorReading& lectura) {
    buffer[currentIndex] = lectura;
    currentIndex = (currentIndex + 1) % MAX_REGISTROS;
  }

  int getCount() const {
    return MAX_REGISTROS;
  }

  SensorReading get(int i) const {
    int index = (currentIndex + i) % MAX_REGISTROS;
    return buffer[index];
  }
};
