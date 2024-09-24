#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

HardwareSerial mySerial(2);  // Configura Serial2 en ESP32
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");

  // Inicia la comunicación con el sensor de huellas
  mySerial.begin(57600);  // Inicializa el puerto serial con la velocidad adecuada
  finger.begin(57600);  // Inicia el sensor con la velocidad adecuada

  if (finger.verifyPassword()) {
    Serial.println("Comunicación con el sensor establecida correctamente");
  } else {
    Serial.println("Error: Fallo en la verificación de la contraseña del sensor");
    while (1);  // Detiene el programa si no se verifica la contraseña
  }
}

void loop() {
  Serial.println("Esperando una huella dactilar...");
  uint8_t p = finger.getImage();

  if (p == FINGERPRINT_OK) {
    Serial.println("Huella capturada");

    p = finger.image2Tz();
    if (p == FINGERPRINT_OK) {
      Serial.println("Huella convertida");
    } else {
      Serial.println("Error al convertir la huella");
    }
  } else {
    Serial.println("Error al capturar la huella");
  }

  delay(1000);  // Pausa de 1 segundo antes del siguiente intento
}
