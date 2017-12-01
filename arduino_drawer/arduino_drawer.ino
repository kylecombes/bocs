
// this constant won't change. It's the pin number of the sensor's output:
const int pingPin = 7;
const int echoPin = 6;
const int switchPin = 5;
bool lastSwitchValue = 1;

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
  // establish variables for duration of the ping, and the distance result
  long duration, cm;
  
  // SENSOR
  sendPing(pingPin);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);

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
