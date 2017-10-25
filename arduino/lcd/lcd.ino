/* Demonstration sketch for PCF8574T I2C LCD Backpack 
Uses library from https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads GNU General Public License, version 3 (GPL-3.0) */
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define TOP_LINE 0
#define BOTTOM_LINE 1

#define LCD_LINE_LENGTH 16
#define LCD_LINE_COUNT 2
#define LCD_SCROLL_DELTA 3

LiquidCrystal_I2C  lcd(0x3D,2,1,0,4,5,6,7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS

// Used for scrolling text lines
long lastPrintTime = 0; // ms
long updateInterval = 700; // ms between printing lines
long initialDelay = updateInterval * 3; // time to wait before scrolling
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
    String input = Serial.readStringUntil('\n');
    setText(input, TOP_LINE);
  }

  maybeUpdateDisplay();
}

void setText(String str, int line) {
  lines[line] = str;
  // Set needed scroll amount (if necessary)
  lineLengths[line] = str.length();
  scrollMax[line] = lineLengths[line] - LCD_LINE_LENGTH;
  if (scrollMax[line] < 0) {
    scrollMax[line] = 0; // Don't need to scroll (shorter than line length)
  }
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

void printSubstring(String str, int startPos, int numChars, int lcdLine) {
  int strLen = str.length();
  int endPos = startPos + numChars;
  if (endPos > strLen) {
    endPos = strLen;
  }
  lcd.setCursor(0, lcdLine);
  lcd.print(str.substring(startPos, endPos));
}

