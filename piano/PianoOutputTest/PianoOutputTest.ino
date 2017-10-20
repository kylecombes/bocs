const int analog = A0;
const int WRITE_D=12;
  const int a = 2;
int cycleDelay = 500;
  int V = 100;  
void setup() {
  // put your setup code here, to run once:
  
  
  Serial.begin(9600);
  
  pinMode(analog, OUTPUT);
  pinMode(WRITE_D, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(WRITE_D,HIGH);


//  analogWrite(analog, V);
//  Serial.println(V);
  delay(cycleDelay);
digitalWrite(WRITE_D,LOW);
    delay(cycleDelay);
//  if (V < 250)
//  {
//    V = V + 50;
//  }
//  else
//  {
//    V = 0;
//  }
}
