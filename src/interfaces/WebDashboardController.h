#pragma once
#include <WiFiS3.h>
#include "domain/TemperatureRange.h"
#include "infrastructure/StorageAdapter.h"

class WebDashboardController {
private:
  WiFiServer server;
  TemperatureRange* rango;
  StorageAdapter* almacenamiento;

public:
  WebDashboardController(TemperatureRange* r, StorageAdapter* a)
      : server(80), rango(r), almacenamiento(a) {}

  void begin() {
    server.begin();
    Serial.println("Servidor web (con gráfico) iniciado en puerto 80");
  }

  void loop() {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Cliente conectado");
      String req = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          req += c;
          if (c == '\n' && req.length() == 1) break;
        }
      }

      if (req.indexOf("GET /readings") >= 0) {
        sendJSON(client);
      } else if (req.indexOf("GET /set_range?") >= 0) {
        updateRange(req);
        sendText(client, "Rango actualizado.");
      } else if (req.indexOf("GET /csv") >= 0) {
        sendCSV(client);
      } else {
        sendDashboard(client);
      }

      delay(1);
      client.stop();
      Serial.println("Cliente desconectado");
    }
  }

private:
  void sendText(WiFiClient& client, const String& message) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println(message);
  }

  void updateRange(const String& req) {
    int minPos = req.indexOf("min=");
    int maxPos = req.indexOf("max=");

    if (minPos > 0 && maxPos > 0) {
      float min = req.substring(minPos + 4, req.indexOf('&')).toFloat();
      float max = req.substring(maxPos + 4, req.indexOf(' ', maxPos)).toFloat();
      rango->set(min, max);
      Serial.print("Nuevo rango: ");
      Serial.print(min);
      Serial.print(" - ");
      Serial.println(max);
    }
  }

  void sendJSON(WiFiClient& client) {
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

  void sendCSV(WiFiClient& client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/csv");
    client.println("Content-Disposition: attachment; filename=\"lecturas.csv\"");
    client.println("Connection: close");
    client.println();

    client.println("timestamp,temperatura");

    int count = almacenamiento->getCount();
    for (int i = 0; i < count; i++) {
      SensorReading r = almacenamiento->get(i);
      client.print(r.timestamp);
      client.print(",");
      client.println(r.calibratedValue);
    }
  }

  void sendDashboard(WiFiClient& client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Monitor de Temperatura</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <h2>Temperatura - Tiempo real</h2>
  <canvas id="chart" width="400" height="200"></canvas>
  <h3>Configurar rango</h3>
  <form onsubmit="setRange(); return false;">
    Min <input type="number" id="min" step="0.1"> °C
    Max <input type="number" id="max" step="0.1"> °C
    <button type="submit">Actualizar</button>
  </form>
  <br>
  <a href="/csv" download><button>Descargar CSV</button></a>

<script>
let chartCtx = document.getElementById('chart').getContext('2d');
let chart = new Chart(chartCtx, {
  type: 'line',
  data: { labels: [], datasets: [{ label: '°C', data: [], borderWidth: 1, borderColor: 'blue', tension: 0.2 }] },
  options: { scales: { y: { beginAtZero: true } } }
});

function updateChart() {
  fetch('/readings')
    .then(res => res.json())
    .then(data => {
      chart.data.labels = data.map(r => Math.round(r.ts / 1000) + \"s\");
      chart.data.datasets[0].data = data.map(r => r.t);
      chart.update();
    });
}

function setRange() {
  let min = document.getElementById('min').value;
  let max = document.getElementById('max').value;
  fetch(`/set_range?min=${min}&max=${max}`)
    .then(res => res.text())
    .then(msg => alert(msg));
}

setInterval(updateChart, 5000);
updateChart();
</script>
</body>
</html>
    )rawliteral");
  }
};
