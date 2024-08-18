#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
  // For UNO and others without hardware serial, we must use software serial...
  // pin #2 is IN from sensor (GREEN wire)
  // pin #3 is OUT from arduino  (WHITE wire)
  // Set up the serial port to use softwareserial..
  SoftwareSerial mySerial(2, 3);

  #else
  // On Leonardo/M0/etc, others with hardware serial, use hardware serial!
  // #0 is green wire, #1 is white
  #define mySerial Serial2

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


int readNumber();
int getFingerprintEnroll();
int getFingerprintID();

bool estado;

//Credentials
  const char* ssid = "ESP32-AP"; // SSID for the ESP32 access point
  const char* password = "password123"; // Password for the access point

  const char* validUsername = "admin";
  const char* validPassword = "password123";


AsyncWebServer server(80);

const char* html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
      <title>Login Page</title>
  </head>
  <body>
      <h2>Login Page</h2>
      <form action="/login" method="post">
          <div class="container">
              <label for="uname"><b>Username</b></label>
              <input type="text" placeholder="Enter Username" name="uname" required>
              <br>
              <label for="psw"><b>Password</b></label>
              <input type="password" placeholder="Enter Password" name="psw" required>
              <br>
              <button type="submit">Login</button>
          </div>
      </form>
  </body>
  </html>
  )rawliteral";

const char* successPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Login Successful</title>
    <script>
        function changeEstado() {
            fetch('/changeEstado')
                .then(response => response.text())
                .then(data => alert(data));
        }
    </script>
</head>
<body>
    <h2>Login Successful</h2>
    <p>Welcome! You have successfully logged in.</p>
    <button onclick="changeEstado()">Change Estado</button>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(apIP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html);
  });

  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request){
    String username = "";
    String password = "";

    if (request->hasParam("uname", true)) {
      username = request->getParam("uname", true)->value();
    }
    if (request->hasParam("psw", true)) {
      password = request->getParam("psw", true)->value();
    }

    if (username.equals(validUsername) && password.equals(validPassword)) {
      request->send_P(200, "text/html", successPage);
      Serial.println("Login successful!");
    } else {
      request->send(401, "text/plain", "Invalid username or password.");
      Serial.println("Invalid username or password.");
    }
  });
  
  server.on("/changeEstado", HTTP_GET, [](AsyncWebServerRequest *request){
    estado = !estado; // Toggle the value of estado
    String message = "Estado changed to: " + String(estado);
    request->send(200, "text/plain", message);
    Serial.println(message);
  });
  // Start server
  server.begin();

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor");
  } 
  else {
    Serial.println("Did not find fingerprint sensor");
    while (1) { delay(1); }
  }
}
int id;
enum State {
  WAITING_FOR_INPUT,
  ENROLLING,
  COMPARING
};

State currentState = WAITING_FOR_INPUT;

void loop() {
  if (estado) {
    switch (currentState) {
      case WAITING_FOR_INPUT:
        Serial.println("Ready to enroll a fingerprint!");
        Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
        currentState = ENROLLING;
      break;

      case ENROLLING:
        if (Serial.available() > 0) {
          id = Serial.parseInt();
          if (id > 0 && id <= 127) { // Valid ID
            Serial.print("Enrolling ID #");
            Serial.println(id);
            if (getFingerprintEnroll()) {
              Serial.println("Fingerprint enrolled successfully!");
              currentState = WAITING_FOR_INPUT;
              estado = false; // Reset estado after enrollment
            }
          } 
          else {
            Serial.println("Invalid ID, please try again.");
            currentState = WAITING_FOR_INPUT;
          }
        }
      break;
    }
  }
  else{   //COMPARE
    if(getFingerprintID() != -1){
      Serial.println("yipeeeeeeee");
    }
    else{ Serial.println("not YIPEEEEEEEE"); } 
    delay(50);  
  }
}


int readNumber(){
  int num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

int getFingerprintEnroll(){
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
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

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
  Serial.println("Place same finger again");
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
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

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

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

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

  return true;
}

int getFingerprintID() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
