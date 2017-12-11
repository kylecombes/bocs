
// this constant won't change. It's the pin number of the sensor's output:
#define DRAWER_PING_PIN 7
#define DRAWER_ECHO_PIN 6
const int switchPin = 5;
bool lastSwitchValue = 1;
bool handshakeCompleted = false;     // Set to true when registration with computer completed
unsigned long nextBroadcastTime = 0;  // Used for doing handshake
#define BROADCAST_INTERVAL 500        // Also used for doing handshake
// Go back into handshake mode after 3 seconds of no heartbeat from computer
unsigned long expectedHeartbeatByTime = 0;
#define HEARTBEAT_TIMEOUT 10000

// ----- Begin rotary configuration ----- //
#include <Servo.h>
#define ROTARY_SERVO_PIN 9
#define ROTARY_SERVO_TELEGRAPH 0
#define ROTARY_SERVO_CLOSED 112
#define ROTARY_SERVO_TRELLIS 180
Servo rotaryServo;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize rotary servo
  rotaryServo.attach(ROTARY_SERVO_PIN);
  rotaryServo.write(ROTARY_SERVO_CLOSED);

  // Initialize drawer ultrasonic rangefinder
  pinMode(DRAWER_PING_PIN, OUTPUT);
  pinMode(DRAWER_PING_PIN, INPUT);
}

void loop() {
  if (handshakeCompleted) {

    checkSerialForMessages();

//    checkDrawerForItem();
    
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

    if (Serial.available() > 0 && Serial.readString() == "ba-dump") {
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    }
  } else { // We have yet to establish a connection with the computer
    if (Serial.available() > 0) { // The computer is responding
      String msg = Serial.readString();
      handshakeCompleted = msg.equals("Hello from computer\n");
      Serial.println(handshakeCompleted ? "Connected" : "Not connected");
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    } else if (millis() > nextBroadcastTime) { // Cry out for a computer
      Serial.println("Hello from arduino_drawer");
      nextBroadcastTime = millis() + BROADCAST_INTERVAL;
    }
  }
}

void checkSerialForMessages() {

  String msg = "";
  while (Serial.available() > 0) {
    msg = Serial.readString();
    
    if (msg == "ba-dump") {
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    }

    msg.trim();
    char outputId = msg[0];
    String payload = msg.substring(1); // The message payload is everything beyond the output identifier

    if (outputId == 'R') { // Rotary position
      Serial.print("Recognized rotary command. Position: ");
      if (payload[0] == 'T') { // Telegraph
        Serial.println("telegraph");
        rotaryServo.write(ROTARY_SERVO_TELEGRAPH);
      } else if (payload[0] == 'A') { // Trellis
        Serial.println("Trellis");
        rotaryServo.write(ROTARY_SERVO_TRELLIS);
      } else if (payload[0] == 'C') {
        Serial.println("closed");
        rotaryServo.write(ROTARY_SERVO_CLOSED);
      }
    }
  }
}

void checkDrawerForItem() {

  // Emit sound
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(DRAWER_ECHO_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(DRAWER_ECHO_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(DRAWER_ECHO_PIN, LOW);
  
  
  long duration = pulseIn(DRAWER_ECHO_PIN, HIGH); // Read ping
  long cm = microsecondsToCentimeters(duration); // Convert to cm
  
  // SWITCH
  int testSwitch = digitalRead(switchPin);
  
  // Send readings to computer when drawer is closed
  if (testSwitch != lastSwitchValue) {
    if (testSwitch == 1) { // Door open
      Serial.print("{\"event_id\": \"3\", \"data\": \"");
      Serial.print(6-cm);
      Serial.println("\"}");
    } else {
      Serial.println("{\"event_id\": \"3\"}");
    }
    lastSwitchValue = testSwitch;
  }
  
  
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
