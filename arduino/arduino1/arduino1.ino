#include <Adafruit_Trellis.h>
#include "BlinkPattern.h"

// This Arduino interfaces with a 16-character x 2-line LCD, a numeric keypad, an Adafruit Trellis keypad,
// a keypad door actuation servo, and a telegraph input.

//////////////////// ----- BEGIN CONFIGURATION ------ /////////////////////
// Handshake (pairing) and heartbeat
bool handshakeCompleted = false;
unsigned long nextBroadcastTime = 0;
#define BROADCAST_INTERVAL 500
// Go back into handshake mode after 3 seconds of no heartbeat from computer
unsigned long expectedHeartbeatByTime = 0;
#define HEARTBEAT_TIMEOUT 10000


// ----- Begin start button light config ----- //
#define START_BUTTON_LED_PIN 10
bool startButtonLightOn = false;

// ----- End start button light config ----- //


// ----- Begin start button config ----- //
#define START_BUTTON_PIN 11
short lastStartButtonState = LOW;
//#define TELEGRAPH_BUTTON_READ_DELAY 20 // (ms) If we read the button too frequently, it doesn't work
//unsigned long lastTelegraphButtonReadTime = 0;
unsigned long lastStartButtonDebounceTime = millis(); // The last time the button state changed
BlinkDef startButtonBlinkPattern;

// ----- End start button config ----- //


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

  // Configure Trellis keypad
  trellis.begin(0x70);

  // Configure telegraph button
  pinMode(TELEGRAPH_BUTTON_PIN, INPUT);

  // Configure start button
  pinMode(START_BUTTON_PIN, INPUT);

  // Configure start button LED
  pinMode(START_BUTTON_LED_PIN, OUTPUT);
  updateStartButtonLEDState();
  
  // Configure computer comms
  Serial.begin(9600); // Initialize serial with baudrate of 9600 bps

}

void loop() {
  if (handshakeCompleted) { // We have already established a connection to the computer
    
    // Check computer comms
    checkSerialForMessages();
  
    // Check start button
    checkStartButton();
  
    // Update start button light state if necessary
    updateStartButtonLEDState();
  
    // Check keypad for input
    checkForKeypadInput();

    // Check for Trellis button presses
    checkTrellisButtons();
  
    // Move door if necessary
  //  maybeMoveDoor();
  
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
      handshakeCompleted = msg.equals("Hello from computer");
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
  
  if (Serial.available() > 0) {
    String msg = Serial.readString();

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
      myservo.write(scaleServoPosition(payload.toInt()));
    }
    else if (outputId == 'T') { // Set the Trellis LEDs
      setTrellisLights(payload.toInt());
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
