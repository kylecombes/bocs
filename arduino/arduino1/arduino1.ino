// This Arduino interfaces with a 16-character x 2-line LCD, a numeric keypad, a keypad door
// actuation servo, and a telegraph input.

// -------------------- BEGIN CONFIGURATION -------------------- //
// Communication with computer using JSON to allow sending key-value pairs
#include <ArduinoJson.h>
StaticJsonBuffer<200> jsonBuffer; // 200 chars (seems to be the max given our other memory requirements)


// ----- Begin LCD config ----- //

#include <Wire.h> // For I2C
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define LCD_LINE_LENGTH 16  // Number of characters per line
#define LCD_LINE_COUNT 2    // Lines of text on the LCD
#define LCD_SCROLL_DELTA 3  // Number of characters to scroll at a time
#define TOP_LINE 0          // Index of top line
#define BOTTOM_LINE 1       // Index of bottom line
#define UPDATE_INTERVAL 10  // ms between display refreshes
#define SCROLL_INTERVAL 800 // ms between scrolling
#define INITIAL_PAUSE 1500  // ms to wait before scrolling after setting text
#define REPLAY_DELAY 3000   // ms to wait before looping back to beginning

String lines[2];              // The lines of text to display and scroll
short scrollPos[] = {0, 0};   // Used to keep track of the current scroll positions of each line
short scrollMax[] = {0, 0};   // Used to keep track of the maximum scroll positions for each line
short lineLengths[] = {0, 0}; // Used to keep track of the length of the strings for each line
long lastScrollTime = millis() + INITIAL_PAUSE; // wait 3 seconds before beginning scrolling
long lastPrintTime = 0;       // The last time the display was refreshed (to limit the refresh rate)
String spaces = "                "; // Used for clearing single lines

LiquidCrystal_I2C  lcd(0x3D, 2, 1, 0, 4, 5, 6, 7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS

// ----- End LCD config ----- //


// ----- Begin numeric keypad config ----- //

#include "Keypad.h"

#define ROWS 4 // Number of rows on keypad
#define COLS 3 // Number of columns on keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5};  // Pin mappings
byte colPins[COLS] = {4, 3, 2};     // Pin mappings
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Necessary to init here for some reason

// ----- End numeric keypad config ----- //


// ----- Begin keypad door servo config ----- //

#include <Servo.h>
#define SERVO_PIN 11  // The pin the servo is connected to on the Arduino
#define SERVO_MAX 37  // The maximum PWM value for the servo
#define SERVO_MIN 8   // The minimum PWM value for the servo
#define KEYPAD_DOOR_POSITION_DELTA 10 // Number of degrees to move servo by on each position update
#define KEYPAD_DOOR_POSITION_CHANGE_DELAY 10 // Number of ms to wait between updating servo position (so it doesn't just jump suddenly)

Servo myservo;  // create servo object to control a servo
short goalKeypadDoorPosition = SERVO_MIN;
short currentKeypadDoorPosition = SERVO_MIN;
short keypadDoorDeltaDirection = -1;
long lastKeypadDoorMoveTime = 0;

// ----- End keypad door servo config ----- //

// -------------------- END CONFIGURATION -------------------- //


// -------------------- BEGIN LOGIC ------------------ //

void setup() {
  // Configure computer comms
  Serial.begin(9600); // Initialize serial with baudrate of 9600 bps
  
  // Configure keypad door actuator
  myservo.attach(SERVO_PIN);
  myservo.write(150);

  // Configure LCD module
  lcd.begin(LCD_LINE_LENGTH, LCD_LINE_COUNT); // for 16 x 2 LCD module
  lcd.setBacklightPin(3, POSITIVE);
  Serial.begin(9600);
}

void loop() {
  // Check computer comms
  checkSerialForMessages();

  // Update message on LCD if necessary
  maybeUpdateDisplay();

  // Move door if necessary
  maybeMoveDoor();
}

// ---------- Begin computer communication logic ---------- //
void checkSerialForMessages() {
  
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
}

// ---------- End computer communication logic ---------- //


// ---------- Begin keypad logic ---------- //

void checkForKeypadInput() {
  char key = keypad.getKey();
  if (key != 0) { // Key pressed
    Serial.println("{\"event_id\": 0, \"data\": \"" + (String)key + "\"}");
  }
}

// ---------- End keypad logic ---------- //


// ---------- Begin keypad door servo logic ---------- //

// Scale the servo position input (range: [0, 1]) to the proper output value (range: [SERVO_MIN, SERVO_MAX])
int scaleServoPosition(float pos) {
  return ((int)((float)(SERVO_MAX - SERVO_MIN)) * pos) + SERVO_MIN;
}

void maybeMoveDoor() {
  if (abs(goalKeypadDoorPosition - currentKeypadDoorPosition) > KEYPAD_DOOR_POSITION_DELTA &&
      millis() > KEYPAD_DOOR_POSITION_CHANGE_DELAY + lastKeypadDoorMoveTime) {
    currentKeypadDoorPosition += KEYPAD_DOOR_POSITION_DELTA * keypadDoorDeltaDirection;
    myservo.write(scaleServoPosition(currentKeypadDoorPosition));
  }
}

// ---------- End keypad door servo logic ---------- //


// ---------- Begin LCD logic ---------- //

void maybeUpdateDisplay() {
  if (millis() > lastPrintTime + UPDATE_INTERVAL) {
    if (millis() > lastScrollTime + REPLAY_DELAY) {
      scrollPos[0] = 0; // Doesn't work with scrolling multiple lines
      lastScrollTime = millis() + INITIAL_PAUSE;
    }
    updateDisplay();
  }
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

// ---------- End LCD logic ---------- //
