#include <Adafruit_Fingerprint.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SWITCH 14

#define ROWS 4
#define COLS 4

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, 7, 6};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 20, 4);

String numberString = "";

#define mySerial Serial2

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); 

int getFingerprintID();
int getFingerprintEnroll();
String handleKeypadInput();

char id;
enum State {
  WAITING_FOR_INPUT,
  ENROLLING,
  COMPARING
};

State currentState = WAITING_FOR_INPUT;

void setup() {
  pinMode(SWITCH, INPUT_PULLUP);
  Serial.begin(9600);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor");
  } else {
    Serial.println("Did not find fingerprint sensor");
    while (1) {
      delay(1);
    }
  }
}
void loop() {
  lcd.init();
  lcd.backlight();
  if (digitalRead(SWITCH)) {
    switch (currentState) {
      case WAITING_FOR_INPUT:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ingresa ID (1-127):");
        currentState = ENROLLING; // Cambia el estado a ENROLLING
        break;

      case ENROLLING:
        String result = handleKeypadInput();
        if (result != "-1") {
          id = result.toInt();
          if (id > 0 && id <= 127) { // ID válido
            Serial.print("Enrolling ID #");
            Serial.println(id);
            if (getFingerprintEnroll()) {
              Serial.println("Fingerprint enrolled successfully!");
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Huella guardada");
              currentState = WAITING_FOR_INPUT; // Regresar al estado de espera
            }
            else {
              Serial.println("Enrollment failed, please try again.");
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Error de guardado");
              currentState = WAITING_FOR_INPUT;
            }
          }
          else {
            Serial.println("Invalid ID, please try again.");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("ID invalido");
            currentState = WAITING_FOR_INPUT; // Regresar al estado de espera
          }
        }
        break;
    }
  }
  else {
    if (getFingerprintID() != -1) {
      Serial.println("Fingerprint recognized!");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Huella reconocida");
    }
    else {
      Serial.println("Fingerprint not recognized.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Huella no reconocida");
    }
    delay(50);
    currentState = WAITING_FOR_INPUT; // Regresar al estado de espera
  }
}

String handleKeypadInput() {
  char key = keypad.getKey(); // Obtener la tecla presionada
  if (key) {
    if (key == 'D') {
      int numberValue = numberString.toInt();
      if (numberValue > 0 && numberValue <= 127) {
        String validString = numberString;
        numberString = "";
        return validString;
      } else {
        Serial.println("Error: Valor fuera de rango (0-255)");
        numberString = "";
        return "-1";
      }
    } else {
      if (isDigit(key)) {
        numberString += key;
      }
    }
  }
  return "-1";
}

int getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
  }
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
  }
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  Serial.print("Creating model for #"); Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return p;
}

int getFingerprintID() {
  int p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return -1;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }
  p = finger.fingerSearch();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Found a print match!");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -1;
    case FINGERPRINT_NOTFOUND:
      Serial.println("Did not find a match");
      return -1;
    default:
      Serial.println("Unknown error");
      return -1;
  }
  return finger.fingerID;
}
