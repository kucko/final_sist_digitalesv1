#include "domain/SensorReading.h"
#include "domain/TemperatureRange.h"
#include "application/ProcessReadingUseCase.h"
#include "interfaces/LedAlertPresenter.h"
#include "infrastructure/Max6675SensorAdapter.h"

Max6675SensorAdapter sensor(7, 6, 5); // SCK, CS, SO
TemperatureRange rango(10.0, 50.0);
ProcessReadingUseCase processReading(&rango);
LedAlertPresenter alertLed(8); // Luz de emergencia en pin 8

void setup() {
  Serial.begin(9600);
}

void loop() {
  SensorReading lectura;
  lectura.timestamp = millis();
  lectura.rawValue = sensor.readTemperature();
  lectura.calibratedValue = lectura.rawValue;
  lectura.isValid = !isnan(lectura.rawValue);

  bool dentroDelRango = processReading.execute(lectura);
  alertLed.showAlert(!dentroDelRango);

  Serial.print("Temp: ");
  Serial.print(lectura.calibratedValue);
  Serial.println(dentroDelRango ? " (ok)" : " (fuera de rango)");

  delay(10000);
}
