// include libraries
#include <Servo.h>

const int switchPin = 4;
const int echoPin = 5;
const int pingPin = 6;
#define CANDY_SERVO_PIN 9
#define TOKEN_SERVO_PIN 10

#define BROADCAST_INTERVAL 500        // Also used for doing handshake
#define ROTARY_TELEGRAPH 20
#define ROTARY_CLOSED 120
#define ROTARY_TRELLIS 180  
#define HEARTBEAT_TIMEOUT 10000

bool lastSwitchValue = 1;
bool handshakeCompleted = false;     // Set to true when registration with computer completed
bool usingHandshake = true;
unsigned long nextBroadcastTime = 0;  // Used for doing handshake
// Go back into handshake mode after 3 seconds of no heartbeat from computer
unsigned long expectedHeartbeatByTime = 0;

Servo candyServo;
Servo tokenServo;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  
  candyServo.attach(CANDY_SERVO_PIN);
  tokenServo.attach(TOKEN_SERVO_PIN);

  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
}


void loop() {
  if (handshakeCompleted || !usingHandshake) {

    checkSerialForMessages();
    
    // SWITCH
    int testSwitch = digitalRead(switchPin);
    
    // Send readings to computer when drawer is closed
    if (testSwitch != lastSwitchValue) {
      // SENSOR
      delay(500); // Let readings stabilize
      short duration = (takeReading(pingPin, echoPin) + takeReading(pingPin, echoPin) + takeReading(pingPin, echoPin))/3;
      float cm = microsecondsToCentimeters(duration);
        
      if (testSwitch == 1) { // Door open
        Serial.print("{\"event_id\": \"3\", \"data\": \"");
        Serial.print(cm);
        Serial.println("\"}");
      } else {
        Serial.println("{\"event_id\": \"3\"}");
      }
      lastSwitchValue = testSwitch;
    }
    
    // Heartbeat stuff
    unsigned long curTime = millis();

    if (curTime > nextBroadcastTime) {
      Serial.println("ba-dump"); // Send heartbeat message
      nextBroadcastTime += BROADCAST_INTERVAL;
    }

    if (curTime > expectedHeartbeatByTime && usingHandshake) {
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
      Serial.println("Hello from arduino_drawer");
      nextBroadcastTime = millis() + BROADCAST_INTERVAL;
    }
  }
}


void sendPing(int pin) {
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  
}


void checkSerialForMessages() {

  if (Serial.available() > 0) {
    String msg = Serial.readString();
    if (msg == "ba-dump") {
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    }

    msg.trim();
    char outputId = msg[0];
    String payload = msg.substring(1); // The message payload is everything beyond the output identifier

    if (outputId == 'R') { // Turn start button LED ring on/off
      if (payload[0] == 'C') {
        dispenseItem('c');
      } else if (payload[0] == 'T') {
        dispenseItem('t');
      }
    }
  }
}
float microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return ((float)microseconds) / 14.5;
}

void dispenseItem(char item) {
  // Dependent on which item to dispense, runs the dispensing process on
  // the appropriate servo
  if (item == 'c') {
    Serial.println("dispensing candy");
    candyServo.write(110);
    delay(2200);
    candyServo.write(90);
  } else if (item == 't') {
    Serial.println("dispensing token");
    tokenServo.write(90);
    delay(500);
    tokenServo.write(70);
  }
}

short takeReading(int trig, int echo) {
  sendPing(trig);
  short duration = pulseIn(echo, HIGH);
  return duration;
}

