#include <WiFi.h>
#include "time.h"

const char* ssid = "BA Escuela";          // Reemplaza con el nombre de tu red WiFi
const char* password = "";  // Reemplaza con la contraseña de tu red WiFi

const char* ntpServer = "ar.pool.ntp.org"; // Servidor NTP para obtener la hora
const long gmtOffset_sec = -10800;           // Offset GMT (ajusta según tu zona horaria)
const int daylightOffset_sec = 0;    // Ajuste para horario de verano (si aplica)

void setup() {
  Serial.begin(115200);

  // Conectar al WiFi en modo STA (Station)
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado a la red WiFi");

  // Iniciar la conexión con el servidor NTP para obtener la hora
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Esperar la sincronización de tiempo
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Fallo al obtener la hora");
    return;
  }

  // Imprimir la hora obtenida
  Serial.println(&timeinfo, "Hora obtenida: %A, %B %d %Y %H:%M:%S");
}

void loop() {
  // Obtener la hora actual en cada ciclo del loop
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println(&timeinfo, "Hora actual: %A, %B %d %Y %H:%M:%S");
  }
  delay(10000); // Espera de 10 segundos antes de la siguiente impresión de hora
}
