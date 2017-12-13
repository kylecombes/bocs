
// this constant won't change. It's the pin number of the sensor's output:
const int pingPin = 6;
const int echoPin = 5;
const int switchPin = 4;
bool lastSwitchValue = 1;
bool handshakeCompleted = false;     // Set to true when registration with computer completed
bool usingHandshake = true;
unsigned long nextBroadcastTime = 0;  // Used for doing handshake
#define BROADCAST_INTERVAL 500        // Also used for doing handshake
// Go back into handshake mode after 3 seconds of no heartbeat from computer
unsigned long expectedHeartbeatByTime = 0;
#define HEARTBEAT_TIMEOUT 10000

void setup() {
  // initialize serial communication:
  Serial.begin(9600);

  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
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

void loop() {
  if (handshakeCompleted || !usingHandshake) {
    
    // SENSOR
    sendPing(pingPin);
    short duration = pulseIn(echoPin, HIGH);
    float cm = microsecondsToCentimeters(duration) - 30.0;
    
    // SWITCH
    int testSwitch = digitalRead(switchPin);
    
    // Send readings to computer when drawer is closed
    if (testSwitch != lastSwitchValue) {
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

float microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return ((float)microseconds) / 14.5;
}
