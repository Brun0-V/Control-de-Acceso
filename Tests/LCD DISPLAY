//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.begin(20, 4);
  lcd.backlight();

}


void loop()
{
  for (int i = 0; i < 4; i++) {
    lcd.clear();
    lcd.setCursor(0, i);
    lcd.print("Linea " + String(i + 1));
    delay(500);
  }
}
