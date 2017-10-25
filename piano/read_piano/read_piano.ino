#include <stdio.h>
// Pins to read binary state off of piano
const byte ONES_PLACE = 9;
const byte TWOS_PLACE = 10;
const byte FOURS_PLACE = 11;
byte pianoKey; // Resulting piano key pressed
int a = 0;
int b = 0;
int c = 0;
int d = 0;
int e = 0;
int f = 0;
int g = 0;
int fg = 0;

void setup() {
  // Setup piano reading pins
  pinMode(ONES_PLACE, INPUT);
  pinMode(TWOS_PLACE, INPUT);
  pinMode(FOURS_PLACE, INPUT);

  // Start serial monitor
  Serial.begin(9600);
}

void loop() {
  pianoKey = readState(ONES_PLACE, TWOS_PLACE, FOURS_PLACE);
 // Serial.println(pianoKey);
  if (pianoKey == 1)
  {
    a = a + 1;
    a=a%10;
    printScreen(a, b, c, d, e, f, g);
  }
  if (pianoKey == 2) {
    b = 3 * a;
  
      b=b%10;
    
    printScreen(a, b, c, d, e, f, g);
  }
  if (pianoKey == 3) {
    c = b + a;
    c=c%10;
    printScreen(a, b, c, d, e, f, g);
  }

  if (pianoKey == 4) {
    d = c;
    c = b;
    b = a;
    a = 0;
   
    printScreen(a, b, c, d, e, f, g);
  }
  if (pianoKey == 5) {
    e = d % a;
    printScreen(a, b, c, d, e, f, g);
  }
  if (pianoKey == 6) {
    fg = b * c + a;
    f = fg / 10;
    g = fg % 10;
    printScreen(a, b, c, d, e, f, g);
  }
  if (pianoKey == 7) {
    a = 0;
    b = 0;
    c = 0;
    d = 0;
    e = 0;
    f = 0;
    g = 0;
    fg = 0;
    printScreen(a, b, c, d, e, f, g);
  }
//  if (pianoKey==0){
//    printScreen(a, b, c, d, e, f, g);
//  }
  delay(100);
}
byte printScreen(int a, int b, int c, int d, int e, int f, int g) {
  //Print what would be displayed on the LCD screen
  Serial.print("a= ");
  Serial.print(a);
  Serial.print(", ");
  Serial.print("b= ");
  Serial.print(b);
  Serial.print(", ");
  Serial.print("c= ");
  Serial.print(c);
  Serial.print(", ");
  Serial.print("d= ");
  Serial.print(d);
  Serial.print(", ");
  Serial.print("e= ");
  Serial.print(e);
  Serial.print(", ");
  Serial.print("f= ");
  Serial.print(f);
  Serial.print(", ");
  Serial.print("g= ");
  Serial.println(g);
  
  byte hi;
  return hi;
}
byte readState(byte pin1, byte pin2, byte pin3) {

  // Sense all pin states
  bool read1 = digitalRead(pin1);
  bool read2 = digitalRead(pin2);
  bool read3 = digitalRead(pin3);

//  Serial.print(read1);
//  Serial.print(", ");
//  Serial.print(read2);
//  Serial.print(", ");
//  Serial.println(read3);
  byte state = read1 + 2 * read2 + 4 * read3;
  return state;
}

byte writeState() {
  byte info;
  while (Serial.available() > 0) {
    info = Serial.read();
    Serial.println(info - '0');
    return (info - '0');
  }
}

