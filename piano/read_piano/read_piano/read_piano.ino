// Pins to read binary state off of piano
const byte ONES_PLACE = 10;
const byte TWOS_PLACE = 11;
const byte FOURS_PLACE = 12;
byte pianoKey; // Resulting piano key pressed

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
  Serial.println(pianoKey);
  delay(1000);
}

byte readState(byte pin1, byte pin2, byte pin3) {

  // Sense all pin states
  bool read1 = digitalRead(pin1);
  bool read2 = digitalRead(pin2);
  bool read3 = digitalRead(pin3);

  Serial.print(read1);
  Serial.print(", ");
  Serial.print(read2);
  Serial.print(", ");
  Serial.println(read3);
  byte state = read1 + 2*read2 + 4*read3;
  return state;
}

byte writeState() {
  byte info;
  while (Serial.available() > 0) {
    info = Serial.read();
    Serial.println(info - '0');
    return(info - '0');
  }
}

