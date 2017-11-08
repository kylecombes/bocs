#include "Keypad.h"
#include <Servo.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define LCD_LINE_LENGTH 16
#define LCD_LINE_COUNT 2
#define LCD_SCROLL_DELTA 3

#define SERVO_MAX 37
#define SERVO_MIN 8

Servo myservo;  // create servo object to control a servo
short goalKeypadDoorPosition = SERVO_MIN;
short currentKeypadDoorPosition = SERVO_MIN;
#define KEYPAD_DOOR_POSITION_DELTA 10 // degrees
#define KEYPAD_DOOR_POSITION_CHANGE_DELAY 10 // ms
short keypadDoorDeltaDirection = -1;
long lastKeypadDoorMoveTime = 0;

#define ROWS 4 //four rows
#define COLS 3 //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C  lcd(0x3D,2,1,0,4,5,6,7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS
StaticJsonBuffer<200> jsonBuffer; // 200 chars (seems to be the max with our other memory requirements)

#define TOP_LINE 0
#define BOTTOM_LINE 1

// LCD stuff

short scrollPos[] = {0, 0};
short scrollMax[] = {0, 0};
short lineLengths[] = {0, 0};

// Used for scrolling text lines
#define UPDATE_INTERVAL 10 // ms between display refreshes
#define SCROLL_INTERVAL 800 // ms between scrolling
#define INITIAL_PAUSE 1500 // ms to wait before scrolling after setting text
#define REPLAY_DELAY 3000 // ms to wait before scrolling back to beginning

String lines[2];
long lastScrollTime = millis() + INITIAL_PAUSE; // wait 3 seconds before beginning scrolling
long lastPrintTime = 0;
String spaces = "                ";

void setup() {
  // Configure keypad and door actuator
  myservo.attach(11);  // attaches the servo on pin 11 to the servo object
  Serial.begin(9600);
  myservo.write(150);
  
  // Configure LCD module
  lcd.begin(LCD_LINE_LENGTH, LCD_LINE_COUNT); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  Serial.begin(9600);
}

void loop() {
  // Process any key input
  char key = keypad.getKey();
  if (key != 0) {
    processKeyInput(key);
  }
  
  // Get any messages coming from the computer
  if (Serial.available() > 0) {
    String msg = Serial.readString();
    msg.trim();
    JsonObject& root = jsonBuffer.parse(msg);
    if (root.success()) { // Successfully parsed JSON from computer
      if (root.containsKey("0")) { // First line
        setText(root["0"], TOP_LINE);
      }
      if (root.containsKey("1")) { // Second line
        setText(root["1"], BOTTOM_LINE);
      }
      if (root.containsKey("bl")) { // Turn backlight on/off
        String bl = root["bl"];
        bool backlightOn = bl.equals("1");
        lcd.setBacklight(backlightOn ? HIGH : LOW);
      }
      if (root.containsKey("kpdr")) { // Keypad door position
//        goalKeypadDoorPosition = root["kpdr"];
        myservo.write(scaleServoPosition(root["kpdr"]));
      }
    } else {
      Serial.println("Could not parse JSON");
    }
  }
  jsonBuffer.clear();
  
  // Update message on LCD if necessary
  if (millis() > lastPrintTime + UPDATE_INTERVAL) {
    if (millis() > lastScrollTime + REPLAY_DELAY) {
      scrollPos[0] = 0; // Doesn't work with scrolling multiple lines
      lastScrollTime = millis() + INITIAL_PAUSE;
    }
    updateDisplay();
  }

  // Move the door if necessary
  maybeMoveDoor();
}

// Scale the servo position input (range: [0, 1]) to the proper output value (range: [SERVO_MIN, SERVO_MAX])
int scaleServoPosition(float pos) {
  return ((int)((float)(SERVO_MAX-SERVO_MIN))*pos) + SERVO_MIN;
}

void maybeMoveDoor() {
  if (abs(goalKeypadDoorPosition - currentKeypadDoorPosition) > KEYPAD_DOOR_POSITION_DELTA &&
      millis() > KEYPAD_DOOR_POSITION_CHANGE_DELAY + lastKeypadDoorMoveTime) {
    currentKeypadDoorPosition += KEYPAD_DOOR_POSITION_DELTA * keypadDoorDeltaDirection;
    myservo.write(scaleServoPosition(currentKeypadDoorPosition));
  }
}

void processKeyInput(char c) {
  Serial.println("{\"event_id\": 0, \"data\": \"" + (String)c + "\"}");
}

void setText(String str, short line) {
  lines[line] = str;
  // Set needed scroll amount (if necessary)
  lineLengths[line] = str.length();
  scrollMax[line] = lineLengths[line] - LCD_LINE_LENGTH;
  if (scrollMax[line] < 0) {
    scrollMax[line] = 0; // Don't need to scroll (shorter than line length)
    lcd.setCursor(0, line); // Clear the line so there aren't any residual characters
    lcd.print(spaces);
  }
  scrollPos[line] = 0;
  lastScrollTime = millis() + INITIAL_PAUSE;
}

void updateDisplay() {
  if (millis() > lastScrollTime + SCROLL_INTERVAL) { // Scroll the lines
    for (short line = 0; line < 2; ++line) {
      if (scrollPos[line] < scrollMax[line]) {
        scrollPos[line] += LCD_SCROLL_DELTA;
        if (scrollPos[line] > scrollMax[line]) {
          scrollPos[line] = scrollMax[line];
        }
        lastScrollTime = millis();
      }
    }
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

