#pragma once
#include <WiFiS3.h>
#include "../domain/TemperatureRange.h"
#include "../infrastructure/StorageAdapter.h"

class WebConfigController {
private:
  WiFiServer server;
  TemperatureRange* rango;
  StorageAdapter* almacenamiento;

public:
  WebConfigController(TemperatureRange* r, StorageAdapter* a)
      : server(80), rango(r), almacenamiento(a) {}

  void begin() {
    server.begin();
    Serial.println("Servidor web iniciado en puerto 80");
  }

  void loop() {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Cliente conectado");
      String request = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          request += c;
          if (c == '\n') {
            // Fin de encabezado HTTP
            if (request.length() == 1) break;
            request = "";
          }

          if (request.indexOf("GET / ") >= 0) {
            enviarPaginaPrincipal(client);
          } else if (request.indexOf("GET /set_range?") >= 0) {
            actualizarRangosDesdeQuery(request);
            enviarTexto(client, "Rango actualizado correctamente");
          } else if (request.indexOf("GET /readings") >= 0) {
            enviarLecturasJSON(client);
          }
        }
      }
      delay(1);
      client.stop();
      Serial.println("Cliente desconectado");
    }
  }

private:
  void enviarTexto(WiFiClient& client, const String& mensaje) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println(mensaje);
  }

  void enviarPaginaPrincipal(WiFiClient& client) {
    String html = "<html><body><h1>Monitor de Temperatura</h1>"
                  "<p>Rango actual: " + String(rango->getMin()) + " °C - " + String(rango->getMax()) + " °C</p>"
                  "<form action='/set_range' method='get'>"
                  "Min: <input name='min' type='number' step='0.1'><br>"
                  "Max: <input name='max' type='number' step='0.1'><br>"
                  "<input type='submit' value='Actualizar'>"
                  "</form><br>"
                  "<a href='/readings'>Ver lecturas recientes</a>"
                  "</body></html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(html);
  }

  void actualizarRangosDesdeQuery(const String& request) {
    int minPos = request.indexOf("min=");
    int maxPos = request.indexOf("max=");

    if (minPos > 0 && maxPos > 0) {
      float min = request.substring(minPos + 4, request.indexOf('&')).toFloat();
      float max = request.substring(maxPos + 4, request.indexOf(' ', maxPos)).toFloat();
      rango->set(min, max);
      Serial.println("Rango actualizado desde web");
    }
  }

  void enviarLecturasJSON(WiFiClient& client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();

    client.print("[");
    int count = almacenamiento->getCount();
    for (int i = 0; i < count; i++) {
      SensorReading r = almacenamiento->get(i);
      client.print("{\"t\":");
      client.print(r.calibratedValue);
      client.print(",\"ts\":");
      client.print(r.timestamp);
      client.print("}");
      if (i < count - 1) client.print(",");
    }
    client.println("]");
  }
};
