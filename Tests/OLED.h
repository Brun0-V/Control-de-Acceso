
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Crear una instancia para la pantalla SH1106
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // No se usa el pin de reset
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Iniciar comunicación I2C
  Wire.begin();

  // Iniciar comunicación serial
  Serial.begin(115200);

  // Iniciar la pantalla OLED
  if (!display.begin(0x3C)) { // Dirección I2C por defecto
    Serial.println("No se encuentra la pantalla SH1106.");
    while (1);
  }

  // Limpiar buffer de pantalla
  display.clearDisplay();
  display.display();
  Serial.println("Ingresa el texto que deseas mostrar en la pantalla:");
}

void loop() {
  if (Serial.available() > 0) {
    String inputText = Serial.readStringUntil('\n'); // Leer la línea de texto ingresada
    displayText(inputText); // Mostrar el texto en la pantalla OLED
  }
}

void displayText(String text) {
  display.clearDisplay(); // Limpiar el buffer de pantalla
  display.setTextSize(1); // Establecer el tamaño del texto
  display.setTextColor(SH110X_WHITE); // Establecer el color del texto
  display.setCursor(0, 0); // Establecer el cursor al inicio
  display.println(text); // Escribir el texto ingresado
  display.display(); // Mostrar en pantalla
}
