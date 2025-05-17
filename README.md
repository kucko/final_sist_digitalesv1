# final_sist_digitalesv1
Esta version enciende el led local 

🧪 Sensor de Temperatura con Arduino UNO R4 WiFi
Este proyecto implementa un sistema de monitoreo de temperatura con interfaz web usando Clean Architecture en Arduino UNO R4 WiFi.

📦 Características
📈 Lectura cada 10s desde sensor MAX6675

💡 Activación de alerta (luz) si la temperatura está fuera de rango

🧠 Almacenamiento circular de las últimas 3 horas de datos en RAM

🌐 Servidor web embebido con:

Gráfico en tiempo real (Chart.js)

Configuración de rangos mínimo y máximo

Descarga de lecturas en CSV

🧰 Estructura del Proyecto

sensor_system/
```

├── src/
│   ├── domain/                # Entidades y objetos de valor
│   ├── application/           # Casos de uso
│   ├── interfaces/            # Adaptadores web y visuales
│   ├── infrastructure/        # Drivers concretos (sensor, almacenamiento)
│   └── main.cpp               # Punto de entrada
├── platformio.ini             # Configuración PlatformIO
└── README.md
```
⚙️ Requisitos
Arduino UNO R4 WiFi (Renesas RA4M1)

Sensor MAX6675 + termopar tipo K

Plataforma de desarrollo: PlatformIO en VS Code

📋 platformio.ini

```
[env:uno_r4_wifi]
platform = renesas-ra
board = uno_r4_wifi
framework = arduino
lib_deps =
  robertalexa/MAX6675
  arduino-libraries/WiFiS3
```
🚀 Instrucciones
Clona este repositorio y ábrelo con PlatformIO

Conecta tu UNO R4 WiFi por USB

Reemplaza las credenciales WiFi en main.cpp:
```
const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";
```
Compila y sube el código

Abre el monitor serial para ver la IP local del dispositivo

Accede en tu navegador a http://[TU_IP]

🌐 Interfaz Web
La página web incluye:

📊 Gráfico en tiempo real de temperatura

⚙️ Formulario para cambiar rangos de alerta

📥 Botón para descargar los datos como archivo CSV

🧱 Basado en Clean Architecture
Este sistema está organizado por capas:

domain/ → entidades puras como SensorReading, TemperatureRange

application/ → lógica de negocio (caso de uso ProcessReadingUseCase)

interfaces/ → presentación web, alertas por LED

infrastructure/ → sensor MAX6675, buffer de almacenamiento circular



