#include "Keypad.h"
#include <Servo.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define LCD_LINE_LENGTH 16
#define LCD_LINE_COUNT 2
#define LCD_SCROLL_DELTA 3

// States
#define AWAITING_INPUT 0
#define ANSWERED 1

Servo myservo;  // create servo object to control a servo
const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[rows] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[cols] = {10, 9, 8}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

LiquidCrystal_I2C  lcd(0x3D,2,1,0,4,5,6,7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS

#define TOP_LINE 0
#define BOTTOM_LINE 1

String lines[2];
String answer = "23";
String question = "How two share a birthday to be at least 50%?";
String secondLinePrefix = "Input: ";
int state = AWAITING_INPUT;
String input = "";

void setup() {
  // Configure keypad and door actuator
  myservo.attach(11);  // attaches the servo on pin 11 to the servo object
  Serial.begin(9600);
  myservo.write(150);
  
  // Configure LCD module
  lcd.begin(LCD_LINE_LENGTH, LCD_LINE_COUNT); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  Serial.begin(9600);
  setText(question, TOP_LINE);
  setText(secondLinePrefix, BOTTOM_LINE);
}

void loop() {
  // Process any key input
  char key = keypad.getKey();
  if (state == AWAITING_INPUT && key != 0) {
    processKeyInput(key);
  }

  // Update message on LCD if necessary
  maybeUpdateDisplay();
}

void processKeyInput(char c) {

  if (c == '#') { // Answer submitted
    if (input.equals(answer)) { // Correct
      setText("Correct!", BOTTOM_LINE);
//      state = ANSWERED; // TODO Do this better (don't use delay)
    } else {
      setText("Incorrect", BOTTOM_LINE);
    }
    delay(300);
    setText(question, TOP_LINE);
    setText(secondLinePrefix + input, BOTTOM_LINE);
  } else {
    input += c;
    lines[BOTTOM_LINE] = secondLinePrefix + input;
  }
}

// LCD stuff

short scrollPos[] = {0, 0};
short scrollMax[] = {0, 0};
short lineLengths[] = {0, 0};

// Used for scrolling text lines
long lastPrintTime = 0; // ms
long updateInterval = 1000; // ms between printing lines
long initialDelay = updateInterval * 3; // time to wait before scrolling

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

