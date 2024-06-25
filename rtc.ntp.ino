#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP32Time.h>

// Wi-Fi credentials
const char *ssid = "BA Escuela";
const char *password = "";

// NTP server details
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800; // Adjust to your timezone
const int daylightOffset_sec = 3600; // Adjust for daylight savings if applicable

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, 60000); // Update every minute

// Initialize ESP32Time
ESP32Time rtc;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP client
  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  // Get current epoch time from NTP
  time_t now = timeClient.getEpochTime();

  // Set time on the RTC
  rtc.setTime(now);

  // Print the current time
  printLocalTime();
}

void loop() {
  // Example to print the time every 10 seconds
  delay(10000);
  printLocalTime();
}

void printLocalTime() {
  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n", 
                rtc.getYear(), rtc.getMonth(), rtc.getDay(), 
                rtc.getHour(), rtc.getMinute(), rtc.getSecond());
}
