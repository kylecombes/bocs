#define TRANS_DELAY 3000

String messages[] = {
  "{\"event_id\": 20, \"data\": \"I am getting a wee bit tired. I think I will watch a movie.\"}",
  "{\"event_id\": 20, \"data\": \"Well, the movie is over. Time for bed. Good night.\"}",
  "{\"event_id\": 0}"
};

long lastTransmitTime = 0;
short index = 0;
short len = 3;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  if (lastTransmitTime + TRANS_DELAY < millis()) {
    if (index < len) {
      Serial.println(messages[index++]);
    }
    lastTransmitTime = millis();
  }

}
