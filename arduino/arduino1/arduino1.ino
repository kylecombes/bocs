#include <Adafruit_Trellis.h>
#include "BlinkPattern.h"
#include "Keypad.h"
#include <Servo.h>
#include "TrellisBlinkPattern.h"

// PINOUT
// Pins 2-8 used by keypad
#define TELEGRAPH_BUTTON_PIN 9
#define CUBE_SERVO_PIN 10
#define KEYPAD_SERVO_PIN 11
#define START_BUTTON_PIN 12

// This Arduino interfaces with a numeric keypad, an Adafruit Trellis keypad, a start button,
// an LED start button ring, a keypad door actuation servo, and a telegraph input momentary switch.

//////////////////// ----- BEGIN CONFIGURATION ------ /////////////////////
// Handshake (pairing) and heartbeat
bool handshakeCompleted = false;
bool usingHandshake = true; // Set to false during debug for to disable handshake
unsigned long nextBroadcastTime = 0;
#define BROADCAST_INTERVAL 1000
// Go back into handshake mode after 3 seconds of no heartbeat from computer
unsigned long expectedHeartbeatByTime = 0;
#define HEARTBEAT_TIMEOUT 20000


// ----- Begin start button light config ----- //
#define START_BUTTON_LED_PIN 13
bool startButtonLightOn = false;
BlinkDef startButtonBlinkPattern;

// ----- End start button light config ----- //


// ----- Begin start button config ----- //
short lastStartButtonState = LOW;
//#define TELEGRAPH_BUTTON_READ_DELAY 20 // (ms) If we read the button too frequently, it doesn't work
//unsigned long lastTelegraphButtonReadTime = 0;
unsigned long lastStartButtonDebounceTime = millis(); // The last time the button state changed

// ----- End start button config ----- //


// ----- Begin Adafruit Trellis config ----- //
Adafruit_Trellis trellis = Adafruit_Trellis();
#define TRELLIS_NUM_BUTTONS 16 // If you change this to be more than 16, you'll also need to change the data type
                               // the values are encoded using from short to something else.
TrellisBlinkPattern trellisBlinkPattern;

// ----- End Adafruit Trellis config ----- //


// ----- Begin numeric keypad config ----- //

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

#define KEYPAD_SERVO_OPEN 120  // The maximum PWM value for the servo
#define KEYPAD_SERVO_CLOSED 18 // The minimum PWM value for the servo
Servo keypadServo;

// ----- End keypad door servo config ----- //


// ----- Begin telegraph button config ----- //

short lastTelegraphButtonState = LOW;
//#define TELEGRAPH_BUTTON_READ_DELAY 20 // (ms) If we read the button too frequently, it doesn't work
//unsigned long lastTelegraphButtonReadTime = 0;
unsigned long lastTelegraphDebounceTime = 0; // The last time the button state changed
#define DEBOUNCE_DELAY 10 // ms to wait between polling button pin
unsigned long buttonDepressTime = 0;

// ----- End telegraph button config ----- //


// ----- Begin cube servo config ----- //

Servo cubeServo;
#define CUBE_TELEGRAPH 20
#define CUBE_CLOSED 112
#define CUBE_TRELLIS 180

// ----- End cube servo config ----- //

///////////////////// -----  END CONFIGURATION ----- ////////////////////


//////////////////// ------ BEGIN LOGIC ------ ////////////////////

void setup() {
  
  // Configure keypad door actuator
  keypadServo.attach(KEYPAD_SERVO_PIN);
  keypadServo.write(KEYPAD_SERVO_CLOSED);

  // Configure Trellis keypad
  trellis.begin(0x70);

  // Configure telegraph button
  pinMode(TELEGRAPH_BUTTON_PIN, INPUT);

  // Configure start button
  pinMode(START_BUTTON_PIN, INPUT);

  // Configure start button LED
  pinMode(START_BUTTON_LED_PIN, OUTPUT);
  updateStartButtonLEDState();

  // Configure cube servo
  cubeServo.attach(CUBE_SERVO_PIN);
  cubeServo.write(CUBE_CLOSED);

  // Configure computer comms
  Serial.begin(9600); // Initialize serial with baudrate of 9600 bps

}

void loop() {
  if (handshakeCompleted || !usingHandshake) { // We have already established a connection to the computer
    
    // Check computer comms
    checkSerialForMessages();
  
    // Check start button
    checkStartButton();
  
    // Update start button light state if necessary
    updateStartButtonLEDState();
  
    // Check keypad for input
    checkForKeypadInput();

    // Check for Trellis button presses
    maybeUpdateTrellisLights();
    checkTrellisButtons();
  
    // Check telegraph button
    checkTelegraphButton();


    // Heartbeat stuff
    unsigned long curTime = millis();

    if (curTime > nextBroadcastTime) {
      Serial.println("ba-dump"); // Send heartbeat message
      nextBroadcastTime += BROADCAST_INTERVAL;
    }

    if (curTime > expectedHeartbeatByTime) {
      // Haven't heard heartbeat from computer in too long
      handshakeCompleted = false; // Go back into pairing mode
    }

  } else { // We have yet to establish a connection with the computer
    if (Serial.available() > 0) { // The computer is responding
      String msg = Serial.readString();
      handshakeCompleted = msg.equals("Hello from computer\n");
      Serial.println(handshakeCompleted ? "Connected" : "Not connected");
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    } else if (millis() > nextBroadcastTime) { // Cry out for a computer
      Serial.println("Hello from arduino1");
      nextBroadcastTime = millis() + BROADCAST_INTERVAL;
    }
  }
}

// ---------- Begin computer communication logic ---------- //
void checkSerialForMessages() {

  String msg = "";
  while (Serial.available() > 0) {
    char nextChar = Serial.read();
    if (nextChar == '\n') break;
    msg += (String)nextChar;
  }
  
  if (msg.length() > 0) {
    
    if (msg == "ba-dump") {
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    }

    msg.trim();
    char outputId = msg[0];
    String payload = msg.substring(1); // The message payload is everything beyond the output identifier

    if (outputId == 'S') { // Turn start button LED ring on/off
      if (payload.length() > 1) {
        startButtonBlinkPattern = parseBlinkPattern(payload);
      } else { // Length is 1, so just turning on or off indefinitely
        startButtonBlinkPattern = parseBlinkPattern(payload+"10"); // Just pretend we have a duration (doesn't affect behavior)
      }
    }
    else if (outputId == 'k') { // Keypad door position
      keypadServo.write(payload == "1" ? KEYPAD_SERVO_OPEN : KEYPAD_SERVO_CLOSED);
    }
    else if (outputId == 'T') { // Set the Trellis LEDs
      parseTrellisBlinkPattern(payload);
    } else if (outputId == 'C') { // Rotate cube
      if (payload[0] == 'T') {
        cubeServo.write(CUBE_TELEGRAPH);
      } else if (payload[0] == 'C') {
        cubeServo.write(CUBE_CLOSED);
      } else if (payload[0] == 'A') {
        cubeServo.write(CUBE_TRELLIS);
      }
    }
  }
}

BlinkDef parseBlinkPattern(String msg) {
  struct BlinkDef def;
  short pos = 0;
  String curTime = ""; // Build up the current number as we parse it
  for (short i = 0; i < msg.length(); ++i) {
    if (msg[i] == 'T' || msg[i] == 'F') {
      if (curTime.length() > 0) {
        def.durations[pos++] = curTime.toInt(); // Save the parsed time for the previous period and increment pos
        curTime = "";
      }
      def.isOn[pos] = msg[i] == 'T';
    } else if (msg[i] == 'R') {
      def.repeats = true;
    } else {
      curTime += msg[i]; // Append a number
    }
  }
  if (curTime.length() > 0) {
    // Save the last parsed time
    def.durations[pos] = curTime.toInt();
  }
  def.nextUpdateTime = millis();
  return def;
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
  if (startButtonBlinkPattern.nextUpdateTime != 0 && millis() > startButtonBlinkPattern.nextUpdateTime) {
    // Turn the LED on if startButtonBlinkPattern.isOn at the current position is true, off otherwise
    digitalWrite(START_BUTTON_LED_PIN, startButtonBlinkPattern.isOn[startButtonBlinkPattern.currentIndex] ? HIGH : LOW);

    // Set how long to stay at this state (all in ms)
    startButtonBlinkPattern.nextUpdateTime = millis() + startButtonBlinkPattern.durations[startButtonBlinkPattern.currentIndex];
    
    // Increment to the next position in the pattern
    ++startButtonBlinkPattern.currentIndex;
    
    // Check if we reached the end of the pattern
    if (startButtonBlinkPattern.durations[startButtonBlinkPattern.currentIndex] == 0) { // We reached the end of the pattern
      if (startButtonBlinkPattern.repeats) { // Pattern repeating enabled
        startButtonBlinkPattern.currentIndex = 0;
      } else { // Pattern repeating disabled
        startButtonBlinkPattern.nextUpdateTime = 0; // Disable updating state
      }
    }
  }
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

void maybeUpdateTrellisLights() {
  unsigned long curTime = millis();
  if (curTime > trellisBlinkPattern.nextUpdateTime) {
    ++trellisBlinkPattern.currentIndex;
    // See if we've finished the sequence
    if (trellisBlinkPattern.currentIndex >= trellisBlinkPattern.length) {
      if (trellisBlinkPattern.repeats) { // Repeat the sequence
        trellisBlinkPattern.currentIndex = 0;
      } else { // Turn off all the lights and do nothing
        setTrellisLights(0);
        trellisBlinkPattern.nextUpdateTime = curTime + 1000000; // Delay running this again for a while
        return;
      }
    }
    setTrellisLights(trellisBlinkPattern.lights[trellisBlinkPattern.currentIndex]);
    trellisBlinkPattern.nextUpdateTime = curTime + trellisBlinkPattern.durations[trellisBlinkPattern.currentIndex];
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

void parseTrellisBlinkPattern(String str) {
  TrellisBlinkPattern newPattern;
  String time = "";
  String lights = "";
  newPattern.length = 0;
  newPattern.repeats = false;
  bool parsingLights = true;
  for (short i = 0; i < str.length(); ++i) {
    if (str[i] == 'R') {
      newPattern.repeats = true;
    } else if (str[i] == ',') {
      newPattern.lights[newPattern.length] = lights.toInt();
      parsingLights = false;
    } else if (str[i] == ';') {
      newPattern.durations[newPattern.length] = time.toInt();
      time = "";
      lights = "";
      ++newPattern.length;
      parsingLights = true;
    } else if (parsingLights) {
      lights += str[i];
    } else {
      time += str[i];
    }
  }
//  Serial.println("First delay: " + (String)newPattern.durations[0]);
  setTrellisLights(newPattern.lights[0]);
  newPattern.nextUpdateTime = millis() + newPattern.durations[0];
  trellisBlinkPattern = newPattern;
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
