#include <Servo.h>

Servo myservo;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);
}

void loop() {

  // put your main code here, to run repeatedly:
//  myservo.write(70);
//  delay(1700);
//  myservo.write(90);
//  delay(1000);
  if (Serial.available() > 0) {
    int pos = Serial.readString().toInt();
    Serial.println("Setting servo to pos " + (String)pos);
    myservo.write(pos);
  }
}
