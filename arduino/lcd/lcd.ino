/* Demonstration sketch for PCF8574T I2C LCD Backpack 
Uses library from https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads GNU General Public License, version 3 (GPL-3.0) */
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define TOP_LINE 0
#define BOTTOM_LINE 1

#define LCD_LINE_LENGTH 16
#define LCD_LINE_COUNT 2
#define LCD_SCROLL_DELTA 3

LiquidCrystal_I2C  lcd(0x3D,2,1,0,4,5,6,7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS
StaticJsonBuffer<1000> jsonBuffer; // 1KB of 2KB total

// Used for scrolling text lines
long lastPrintTime = 0; // ms
short updateInterval = 700; // ms between printing lines
short initialDelay = updateInterval * 3; // time to wait before scrolling
String lines[2];
short scrollPos[] = {0, 0};
short scrollMax[] = {0, 0};
short lineLengths[] = {0, 0};

void setup()
{
  // activate LCD module
  lcd.begin(LCD_LINE_LENGTH, LCD_LINE_COUNT); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0) {
    JsonObject& root = jsonBuffer.parse(Serial);
    if (root.success()) { // Successfully parsed JSON from computer
      if (root.containsKey("0")) { // First line
        setText(root["0"], TOP_LINE);
      }
      if (root.containsKey("1")) { // Second line
        setText(root["1"], BOTTOM_LINE);
      }
    }
  }

  maybeUpdateDisplay();
}

void setText(String str, short line) {
  lines[line] = str;
  // Set needed scroll amount (if necessary)
  lineLengths[line] = str.length();
  scrollMax[line] = lineLengths[line] - LCD_LINE_LENGTH;
  if (scrollMax[line] < 0) {
    scrollMax[line] = 0; // Don't need to scroll (shorter than line length)
  }
  scrollPos[0] = 0;
  scrollPos[1] = 0;
}

void maybeUpdateDisplay() {
  
  // Return if enough time hasn't elapsed or we have no more lines to print
  if (millis() > lastPrintTime + updateInterval) {
    // Scroll the first line
    for (short line = 0; line < 2; ++line) {
      if (scrollPos[line] < scrollMax[line]) {
        scrollPos[line] += LCD_SCROLL_DELTA;
        if (scrollPos[line] > scrollMax[line]) {
          scrollPos[line] = scrollMax[line];
        }
      }
    }
    
    lastPrintTime = millis();
  }
  // Update the text on the screen
  printSubstring(lines[TOP_LINE], scrollPos[0], LCD_LINE_LENGTH, TOP_LINE);
  printSubstring(lines[BOTTOM_LINE], scrollPos[1], LCD_LINE_LENGTH, BOTTOM_LINE);
}

void printSubstring(String str, short startPos, short numChars, short lcdLine) {
  short strLen = str.length();
  short endPos = startPos + numChars;
  if (endPos > strLen) {
    endPos = strLen;
  }
  lcd.setCursor(0, lcdLine);
  lcd.print(str.substring(startPos, endPos));
}

