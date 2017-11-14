#include <Adafruit_Trellis.h>

// This Arduino interfaces with a 16-character x 2-line LCD, a numeric keypad, an Adafruit Trellis keypad,
// a keypad door actuation servo, and a telegraph input.

//////////////////// ----- BEGIN CONFIGURATION ------ /////////////////////
// Communication with computer using JSON to allow sending key-value pairs
#include <ArduinoJson.h>
StaticJsonBuffer<200> jsonBuffer; // 200 chars (seems to be the max given our other memory requirements)
bool handshakeCompleted = false;


// ----- Begin start button light config ----- //
#define START_BUTTON_LED_PIN 10
bool startButtonLightOn = true;

// ----- End start button light config ----- //


// ----- Begin start button config ----- //
#define START_BUTTON_PIN 11
short lastStartButtonState = LOW;
//#define TELEGRAPH_BUTTON_READ_DELAY 20 // (ms) If we read the button too frequently, it doesn't work
//unsigned long lastTelegraphButtonReadTime = 0;
unsigned long lastStartButtonDebounceTime = 0; // The last time the button state changed

// ----- End start button config ----- //


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
unsigned long lastScrollTime = millis() + INITIAL_PAUSE; // Wait 3 seconds before beginning scrolling
unsigned long lastPrintTime = 0;    // The last time the display was refreshed (to limit the refresh rate)
String spaces = "                "; // Used for clearing single lines

LiquidCrystal_I2C  lcd(0x3D, 2, 1, 0, 4, 5, 6, 7); // 0x3D is the I2C bus address for an unmodified backpack -- THIS MAY CHANGE BETWEEN DISPLAYS

// ----- End LCD config ----- //


// ----- Begin Adafruit Trellis config ----- //

Adafruit_Trellis trellis = Adafruit_Trellis();
#define TRELLIS_NUM_BUTTONS 16 // If you change this to be more than 16, you'll also need to change the data type
                               // the values are encoded using from short to something else.

// ----- End Adafruit Trellis config ----- //


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
#define SERVO_PIN 9  // The pin the servo is connected to on the Arduino
#define SERVO_MAX 37  // The maximum PWM value for the servo
#define SERVO_MIN 8   // The minimum PWM value for the servo
#define KEYPAD_DOOR_POSITION_DELTA 10 // Number of degrees to move servo by on each position update
#define KEYPAD_DOOR_POSITION_CHANGE_DELAY 10 // Number of ms to wait between updating servo position (so it doesn't just jump suddenly)

Servo myservo;  // create servo object to control a servo
short goalKeypadDoorPosition = SERVO_MIN;
short currentKeypadDoorPosition = SERVO_MIN;
short keypadDoorDeltaDirection = -1;
unsigned long lastKeypadDoorMoveTime = 0;

// ----- End keypad door servo config ----- //


// ----- Begin telegraph button config ----- //

#define TELEGRAPH_BUTTON_PIN 12
short lastTelegraphButtonState = LOW;
//#define TELEGRAPH_BUTTON_READ_DELAY 20 // (ms) If we read the button too frequently, it doesn't work
//unsigned long lastTelegraphButtonReadTime = 0;
unsigned long lastTelegraphDebounceTime = 0; // The last time the button state changed
#define DEBOUNCE_DELAY 10 // ms to wait between polling button pin
unsigned long buttonDepressTime = 0;

// ----- End telegraph button config ----- //

///////////////////// -----  END CONFIGURATION ----- ////////////////////


//////////////////// ------ BEGIN LOGIC ------ ////////////////////

void setup() {
  
  // Configure keypad door actuator
  myservo.attach(SERVO_PIN);
  myservo.write(150);

  // Configure LCD module
//  lcd.begin(LCD_LINE_LENGTH, LCD_LINE_COUNT); // for 16 x 2 LCD module
//  lcd.setBacklightPin(3, POSITIVE);

  // Configure Trellis keypad
  trellis.begin(0x70);

  // Configure telegraph button
  pinMode(TELEGRAPH_BUTTON_PIN, INPUT);

  // Configure start button
  pinMode(START_BUTTON_PIN, INPUT);

  // Configure start button LED
  pinMode(START_BUTTON_LED_PIN, OUTPUT);
  
  // Configure computer comms
  Serial.begin(9600); // Initialize serial with baudrate of 9600 bps

}

void loop() {
  if (handshakeCompleted) {
    
    // Check computer comms
    checkSerialForMessages();
  
    // Check start button
  //  checkStartButton();
  
    // Update start button light state if necessary
  //  updateStartButtonLEDState();
  
    // Check keypad for input
  //  checkForKeypadInput();
    
    // Update message on LCD if necessary
  //  maybeUpdateDisplay();
  
    // Check for Trellis button presses
    checkTrellisButtons();
  
    // Move door if necessary
  //  maybeMoveDoor();
  
    // Check telegraph button
  //  checkTelegraphButton();
  } else {
    if (Serial.available() > 0) { // The computer is responding
      String msg = Serial.readString();
      handshakeCompleted = msg.equals("Hello from computer\n");
    } else { // Cry out for a computer
      Serial.println("Hello from lcd1");
      delay(100);
    }
  }
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
      if (root.containsKey("sLED")) { // Turn start button LED ring on/off
        String val = root["sLED"];
        startButtonLightOn = val.equals("1"); // Pass "1" to turn on
      }
      if (root.containsKey("tLEDs")) { // Set the Trellis LEDs
        setTrellisLights(root["tLEDs"]);
      }
    } else {
      Serial.println("Could not parse JSON");
    }
  }
  jsonBuffer.clear();
}

// ---------- End computer communication logic ---------- //


// ---------- Begin start button logic ---------- //

void checkStartButton() {
  // Make sure enough time has elapsed between reads
//  if (millis() < lastTelegraphButtonReadTime + TELEGRAPH_BUTTON_READ_DELAY) return;
  
  int currentButtonState = digitalRead(START_BUTTON_PIN); // Get the current value

  if (currentButtonState != lastStartButtonState) { // The switch changed, due to noise or pressing
    lastStartButtonDebounceTime = millis(); // Reset the debouncing timer
  }

  delay(20); // Current workaround

  if ((millis() - lastStartButtonDebounceTime) > DEBOUNCE_DELAY && currentButtonState != lastStartButtonState && currentButtonState == HIGH) { // User actually depressed button
    Serial.println("{\"event_id\": \"0\"}"); // Send event to computer
  }

  // Save the currentButtonState
  lastStartButtonState = currentButtonState;

//  lastTelegraphButtonReadTime = millis();
}

// ---------- End telegraph button logic ---------- //


// ---------- Begin start button light logic ---------- //

void updateStartButtonLEDState() {
  digitalWrite(START_BUTTON_LED_PIN, startButtonLightOn ? HIGH : LOW);
}

// ---------- End start button light logic ---------- //


// ---------- Begin keypad logic ---------- //

void checkForKeypadInput() {
  char key = keypad.getKey();
  if (key != 0) { // Key pressed
    Serial.println("{\"event_id\": 6, \"data\": \"" + (String)key + "\"}");
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


// ---------- Begin Trellis logic ---------- //

void checkTrellisButtons() {
  unsigned short buttonStates = 0;
  if (trellis.readSwitches()) { // Button state has changed since last check
    for (short i = 0; i < TRELLIS_NUM_BUTTONS; ++i) {
      if (trellis.isKeyPressed(i)) {
        buttonStates |= 1 << i;
      }
    }
    Serial.println("{\"event_id\": \"1\", \"data\": \"" + (String)buttonStates + "\"}");
  }
}

// Set which lights should be on by passing a boolean array, where true is on and false is off
void setTrellisLights(short lightStatuses) {
  for (short i = 0; i < TRELLIS_NUM_BUTTONS; ++i) {
    if ((lightStatuses >> i) & 1) { // Light should be on
      trellis.setLED(i);
    } else { // Light should be off
      trellis.clrLED(i);
    }
  }
  trellis.writeDisplay();
}

// ---------- End Trellis logic ---------- //


// ---------- Begin telegraph button logic ---------- //

void checkTelegraphButton() {
  // Make sure enough time has elapsed between reads
//  if (millis() < lastTelegraphButtonReadTime + TELEGRAPH_BUTTON_READ_DELAY) return;
  
  int currentButtonState = digitalRead(TELEGRAPH_BUTTON_PIN); // Get the current value

  if (currentButtonState != lastTelegraphButtonState) { // The switch changed, due to noise or pressing
    lastTelegraphDebounceTime = millis(); // Reset the debouncing timer
  }

  delay(20); // Current workaround

  if ((millis() - lastTelegraphDebounceTime) > DEBOUNCE_DELAY && currentButtonState != lastTelegraphButtonState) { // Button state actually changed
    if (currentButtonState == HIGH) { // User depressed button
      buttonDepressTime = millis(); // Begin press duration stopwatch
    } else { // User depressed the button
      short depressDuration = millis() - buttonDepressTime;
      Serial.println("{\"event_id\": \"4\", \"data\": \"" + (String)depressDuration + "\"}"); // Send depress duration to computer
    }
  }

  // Save the currentButtonState
  lastTelegraphButtonState = currentButtonState;

//  lastTelegraphButtonReadTime = millis();
}

// ---------- End telegraph button logic ---------- //

//////////////////// ----- END LOGIC ----- ////////////////////
