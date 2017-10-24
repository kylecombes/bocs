/* Demonstration sketch for PCF8574T I2C LCD Backpack 
Uses library from https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads GNU General Public License, version 3 (GPL-3.0) */
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define LINE_LENGTH 16
#define LINE_COUNT 2

LiquidCrystal_I2C  lcd(0x3D,2,1,0,4,5,6,7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS

int position = 0;
int line = 0;

// Used for scrolling text lines
long lastPrintTime = 0; // ms
long updateInterval = 1000; // ms between printing lines
long initialDelay = updateInterval * 3; // time to wait before scrolling
short scrollCount = 0;

void setup()
{
  // activate LCD module
  lcd.begin(LINE_LENGTH, LINE_COUNT); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    setText(input);
  }

  maybeUpdateDisplay();
}

void setText(String str) {

  lcd.setCursor(0,0);
  lcd.print(str);

  // Set needed scroll amount (if necessary)
  scrollCount = str.length() - LINE_LENGTH;
  if (scrollCount < 0) {
    scrollCount = 0; // Don't need to scroll (shorter than line length)
  }
  
}

void maybeUpdateDisplay() {

  // Return if enough time hasn't elapsed or we have no more lines to print
  if (millis() < lastPrintTime + updateInterval) return;

  if (scrollCount > 0) {
    lcd.scrollDisplayLeft();
    --scrollCount;
  }

  lastPrintTime = millis();
  
}

