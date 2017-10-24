const int Key1 = 2;
const int Key2 = 3;
const int Key3 = 4;
const int Key4 = 5;
const int Key5 = 6;
const int Key6 = 7;
const int Key7 = 8;
//const int Key8 = 9;
const int ONES = 9;
const int TWOS = 10;
const int FOURS = 11;
int input = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Key1, INPUT);
  pinMode(Key2, INPUT);
  pinMode(Key3, INPUT);
  pinMode(Key4, INPUT);
  pinMode(Key5, INPUT);
  pinMode(Key6, INPUT);
  pinMode(Key7, INPUT);
//  pinMode(Key8, INPUT);

  pinMode(ONES, OUTPUT);
  pinMode(TWOS, OUTPUT);
  pinMode(FOURS, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("Input Number");
//  while (Serial.available() == 0) {}
//  input = Serial.read();

//  input = input - 48;
if(digitalRead(Key1)==HIGH)
{
  input=1;
}
if(digitalRead(Key2)==HIGH)
{
  input=2;
}
if(digitalRead(Key3)==HIGH)
{
  input=3;
}
if(digitalRead(Key4)==HIGH)
{
  input=4;
}
if(digitalRead(Key5)==HIGH)
{
  input=5;
}
if(digitalRead(Key6)==HIGH)
{
  input=6;
}
if(digitalRead(Key7)==HIGH)
{
  input=7;
}

  Serial.println(input);

  if (input >= 4)
  {
    digitalWrite(FOURS, HIGH);
    Serial.println("FOURS=HIGH");
    input = input - 4;
    
  }
  else
  {digitalWrite(FOURS,LOW);}
  if (input >= 2)
  {
    digitalWrite(TWOS, HIGH);
    Serial.println("TWOS=HIGH");
    input = input - 2;
  }
  else
  {digitalWrite(TWOS,LOW);}
  if (input >= 1)
  {
    digitalWrite(ONES, HIGH);
    Serial.println("ONES=HIGH");
    input = input - 1;
  }
  else
  {digitalWrite(ONES,LOW);}
  Serial.println(input);

}
