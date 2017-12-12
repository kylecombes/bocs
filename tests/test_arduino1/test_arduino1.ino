/***********************************************************
 * Arduino 1 Test Code
 * @brief Code to test all outputs/inputs of BOCS Arduino 1
 * @author Connor Novak
 ***********************************************************/

#include <Servo.h>

// Constants
#define TRIG_PIN 7
#define ECHO_PIN 6
#define SWITCH_PIN 13
#define DISPENSER_PIN 11

Servo dispenser;

void setup() { //----------S----------S----------S----------S
  // declare pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);
  dispenser.attach(DISPENSER_PIN);

  Serial.begin(9600); // Opens serial monitor @ 9600 baud

  delay(1000);
  //testUltrasonicSensor(TRIG_PIN, ECHO_PIN);
  //testSwitch(SWITCH_PIN);
  testDispenser(dispenser);
}

void loop() { //----------L----------L----------L----------L

  delay(10000);
  Serial.println("Tests complete");

}

//----------F----------F----------F----------F----------F----------F

void testUltrasonicSensor(byte trig, byte echo) {
  Serial.println("Wave hand in front of ultrasonic sensor");
  Serial.print("Initial Reading: ");
  long init_reading = readSensor(trig, echo);
  Serial.println(init_reading);
  while(true) {
    Serial.print("Current Reading: ");
    Serial.println(readSensor(trig, echo));
    if (abs(init_reading - readSensor(trig, echo)) > 300) {
      Serial.println("Ultrasonic sensor test complete");
      break;
    }
  }
}


long readSensor(byte trigPin, byte echoPin) {
  //Ensures trigpin is low
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  return duration;
}

void testSwitch(byte pin) {

  Serial.println("Slide Drawer out until switch is depressed");
  while(true) {
    Serial.print("Current Reading: ");
    Serial.println(digitalRead(pin));
    if (!digitalRead(pin)) {
      Serial.println("Drawer open");
      break;
    }
  }

  Serial.println("Slide drawer in until switch is pressed");
  while(true) {
    Serial.print("Current Reading: ");
    Serial.println(digitalRead(pin));
    if (digitalRead(pin)) {
      Serial.println("Drawer closed");
      break;
    }
  }

  Serial.println("Drawer test complete");
}

void testDispenser(Servo servo) {
  Serial.println("Testing Dispenser Servo");
  servo.write(0);
  delay(5000);
  servo.write(180);
  delay(5000);
  servo.write(90);
  Serial.println("Servo Test Complete");
}

