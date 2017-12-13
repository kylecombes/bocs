/*********************************************
 * Piano-Raspi Interface Code
 * author: Connor Novak
 * date: Nov 29, 2017
 *********************************************/

#include <Servo.h>  // Provides PWM library for linear servo

// Define pins for inputs/outputs
const int Key1 = 2;
const int Key2 = 3;
const int Key3 = 4;
const int Key4 = 5;
const int Key5 = 6;
const int Key6 = 7;
const int Key7 = 8;
const int INNER_LIMIT = 9;
const int OUTER_LIMIT = 10;
const int SERVO_PIN = 11;
#define STOP 90
#define IN 70
#define OUT 120
#define EXTEND_TIME 1000
#define RETRACT_TIME 1150

Servo myservo;

int button_pressed = 0;               // Current button state
int last_button_pressed = 0;          // Most recent button state
bool is_open = 0;                     // Current extended/retracted state of piano
bool handshake_completed = false;     // Set to true when registration with computer completed
unsigned long next_broadcast_time = 0;  // Used for doing handshake
#define BROADCAST_INTERVAL 2000       // Also used for doing handshake
// Go back into handshake mode after 3 seconds of no heartbeat from computer
unsigned long expectedHeartbeatByTime = 0;
#define HEARTBEAT_TIMEOUT 10000

// Setup function ----------S----------S----------S----------S
void setup() {
  Serial.begin(9600);

  // Sets pin modes
  pinMode(Key1, INPUT);
  pinMode(Key2, INPUT);
  pinMode(Key3, INPUT);
  pinMode(Key4, INPUT);
  pinMode(Key5, INPUT);
  pinMode(Key6, INPUT);
  pinMode(Key7, INPUT);
  pinMode(INNER_LIMIT, INPUT);
  pinMode(OUTER_LIMIT, INPUT);
  myservo.attach(SERVO_PIN);
  
}

// Loop function ----------L----------L----------L----------L
void loop() {
  if (handshake_completed) {

    // Extends or retracts piano based on incoming msgs
    checkSerial();
  
    if (is_open) {
      last_button_pressed = checkKeys(last_button_pressed);
    }

    // Heartbeat stuff
    unsigned long curTime = millis();

    if (curTime > next_broadcast_time) {
      Serial.println("ba-dump"); // Send heartbeat message
      next_broadcast_time += BROADCAST_INTERVAL;
    }

    if (curTime > expectedHeartbeatByTime) {
      // Haven't heard heartbeat from computer in too long
      handshake_completed = false; // Go back into pairing mode
    }
    
  } else { // Try to register with the computer
    if (Serial.available() > 0) { // The computer is responding
      String msg = Serial.readString();
      handshake_completed = msg.equals("Hello from computer\n");
      Serial.println(handshake_completed ? "Connected" : "Not connected");
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    } else if (millis() > next_broadcast_time) { // Cry out for a computer
      Serial.println("Hello from pianoduino");
      next_broadcast_time = millis() + BROADCAST_INTERVAL;
    }
  }
}


// Functions ----------F----------F----------F----------F----------F

// Function to update piano state (extended/retracted) based on incoming msg; assumes serial port opened
void checkSerial() {
  
//  String msg = "";
//  while (Serial.available() > 0) {
//    char nextChar = Serial.read();
//    if (nextChar == '\n') break;
//    msg += (String)nextChar;
//    Serial.print(nextChar);
//  }

  if (Serial.available() > 0) {
    String msg = Serial.readString();
    if (msg == "ba-dump\n") {
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    }

    // If received retract msg
    else if (msg[0] == '0') {
//      Serial.println("Closing");
      myservo.write(IN);
      delay(RETRACT_TIME);
      myservo.write(STOP);
      is_open = false;
    }

    // If received extend msg
    else if (msg[0] == '1') {
//      Serial.println("Opening");
      myservo.write(OUT);
      delay(EXTEND_TIME);
      myservo.write(STOP);
      is_open = true;
    } else {
      delay(30);
    }
  }
}

// Function to send msg given pressed keys
int checkKeys(int last_button_pressed) {

  // Initialize temporary variables
  int button_pressed;
  
  // Update pressed button
  if (digitalRead(Key1) == HIGH)
  {
    button_pressed = 1;
  }
  else if (digitalRead(Key2) == HIGH)
  {
    button_pressed = 2;
  }
  else if (digitalRead(Key3) == HIGH)
  {
    button_pressed = 3;
  }
  else if (digitalRead(Key4) == HIGH)
  {
    button_pressed = 4;
  }
  else if (digitalRead(Key5) == HIGH)
  {
    button_pressed = 5;
  }
  else if (digitalRead(Key6) == HIGH)
  {
    button_pressed = 6;
  }
  else if (digitalRead(Key7) == HIGH)
  {
    button_pressed = 7;
  }
  else {
    button_pressed = 0;
  }

  // Send message if button_pressed has changed
  if (button_pressed != last_button_pressed) {
    Serial.println("{\"event_id\":\"2\",\"data\":\"" + String(button_pressed) + "\"}");
  }

  return button_pressed;
}

