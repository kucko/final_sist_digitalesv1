#pragma once
#include <Arduino.h>

class LedAlertPresenter {
private:
  int pin;

public:
  LedAlertPresenter(int alertPin) : pin(alertPin) {
    pinMode(pin, OUTPUT);
  }

  void showAlert(bool isActive) {
    digitalWrite(pin, isActive ? HIGH : LOW);
  }
};
