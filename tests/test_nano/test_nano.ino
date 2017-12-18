#include <Servo.h>

// Define pins
#define KEY_1_PIN             2
#define KEY_2_PIN             3
#define KEY_3_PIN             4
#define KEY_4_PIN             5
#define KEY_5_PIN             6
#define KEY_6_PIN             7
#define KEY_7_PIN             8
#define EXTENDED_LIMIT_PIN    10
#define RETRACTED_LIMIT_PIN  9
#define SERVO_PIN            11
//                           12
//                           13

Servo slideServo;

void setup() {

  // Initialize pins
  pinMode(KEY_1_PIN, INPUT);
  pinMode(KEY_2_PIN, INPUT);
  pinMode(KEY_3_PIN, INPUT);
  pinMode(KEY_4_PIN, INPUT);
  pinMode(KEY_5_PIN, INPUT);
  pinMode(KEY_6_PIN, INPUT);
  pinMode(KEY_7_PIN, INPUT);

  
  slideServo.attach(SERVO_PIN);
  Serial.begin(9600);

  //Run test functions
  testSlide(EXTENDED_LIMIT_PIN, RETRACTED_LIMIT_PIN);
  testKeyboard();
}

void loop() {
  
  Serial.println("Tests Complete!");
  delay(10000);
}

// Keyboard button test function
void testKeyboard() {

  Serial.println("Start: Key Test");
  
  for (int i = 2; i < 9; i++) {
    Serial.print("Press Button ");
    Serial.println(i-1);
    while (true) {
      if (digitalRead(i)) {
        Serial.print("Button ");
        Serial.print(i-1);
        Serial.println(" Pressed!");
        break;
      }
    }
  }
}

void testSlide(byte extend, byte retract) {
  Serial.println("Testing Servo");
  
  Serial.println("Retracting");
  slideServo.write(80);
  while(true) {
    if (digitalRead(retract)) {
      break;
    }
  }
  
  Serial.println("Extending");
  slideServo.write(100);
  while(true) {
    if (digitalRead(extend)) {
      break;
    }
  }
  
  slideServo.write(90);
}

