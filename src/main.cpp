// main.cpp
#include <Arduino.h>
#include <WiFiS3.h>

// Dominios
#include "domain/SensorReading.h"
#include "domain/TemperatureRange.h"

// Casos de uso
#include "application/ProcessReadingUseCase.h"

// Adaptadores
#include "interfaces/LedAlertPresenter.h"
#include "interfaces/WebDashboardController.h"

// Infraestructura
#include "infrastructure/Max6675SensorAdapter.h"
#include "infrastructure/StorageAdapter.h"

// Pines del MAX6675
#define PIN_SCK 7
#define PIN_CS 6
#define PIN_SO 5

// Pin para luz de alerta
#define PIN_ALERTA 8

// Intervalo entre lecturas
#define INTERVALO_LECTURA 10000 // 10 segundos

// Credenciales WiFi (reemplaza con tus datos reales)
const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";

// Instancias de componentes
Max6675SensorAdapter sensor(PIN_SCK, PIN_CS, PIN_SO);
TemperatureRange rango(15.0, 45.0);
ProcessReadingUseCase procesador(&rango);
LedAlertPresenter alerta(PIN_ALERTA);
StorageAdapter almacenamiento;
WebDashboardController web(&rango, &almacenamiento);

unsigned long ultimoTiempo = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  web.begin();
}

void loop() {
  unsigned long ahora = millis();
  if (ahora - ultimoTiempo >= INTERVALO_LECTURA) {
    ultimoTiempo = ahora;

    SensorReading lectura;
    lectura.timestamp = ahora;
    lectura.rawValue = sensor.readTemperature();
    lectura.calibratedValue = lectura.rawValue;
    lectura.isValid = !isnan(lectura.rawValue);

    bool dentroRango = procesador.execute(lectura);
    alerta.showAlert(!dentroRango);
    almacenamiento.store(lectura);

    Serial.print("Temperatura: ");
    Serial.print(lectura.calibratedValue);
    Serial.println(dentroRango ? " °C (OK)" : " °C (FUERA DE RANGO)");
  }

  web.loop();
}

// domain/SensorReading.h
#pragma once

struct SensorReading {
  unsigned long timestamp;
  float rawValue;
  float calibratedValue;
  bool isValid;
};

// domain/TemperatureRange.h
#pragma once

class TemperatureRange {
private:
  float minTemp;
  float maxTemp;

public:
  TemperatureRange(float min, float max) : minTemp(min), maxTemp(max) {}

  bool isWithinRange(float value) const {
    return value >= minTemp && value <= maxTemp;
  }

  void set(float min, float max) {
    minTemp = min;
    maxTemp = max;
  }

  float getMin() const { return minTemp; }
  float getMax() const { return maxTemp; }
};

// application/ProcessReadingUseCase.h
#pragma once
#include "../domain/SensorReading.h"
#include "../domain/TemperatureRange.h"

class ProcessReadingUseCase {
private:
  TemperatureRange* range;

public:
  ProcessReadingUseCase(TemperatureRange* r) : range(r) {}

  bool execute(SensorReading reading) {
    return range->isWithinRange(reading.calibratedValue);
  }
};

// interfaces/LedAlertPresenter.h
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

// infrastructure/Max6675SensorAdapter.h
#pragma once
#include <max6675.h>

class Max6675SensorAdapter {
private:
  MAX6675 sensor;

public:
  Max6675SensorAdapter(int sck, int cs, int so) : sensor(sck, cs, so) {}

  float readTemperature() {
    return sensor.readCelsius();
  }
};

// infrastructure/StorageAdapter.h
#pragma once
#include "../domain/SensorReading.h"
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
