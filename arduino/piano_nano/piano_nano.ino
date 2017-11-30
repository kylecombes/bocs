/*********************************************
 * Piano-Raspi Interface Code
 * author: Connor Novak
 * date: Nov 29, 2017
 *********************************************/


// Define pins for keys
const int Key1 = 2;
const int Key2 = 3;
const int Key3 = 4;
const int Key4 = 5;
const int Key5 = 6;
const int Key6 = 7;
const int Key7 = 8;
const int INNER_LIMIT = 9;
const int OUTER_LIMIT = 10;

int button_pressed = 0;               // Current button state
int last_button_pressed = 0;          // Most recent button state
int curr_state = 0;                   // Current extended/retracted state of piano (1 for out, 0 for in)
bool handshake_completed = false;     // Set to true when registration with computer completed
unsigned long next_broadcast_time = 0;  // Used for doing handshake
#define BROADCAST_INTERVAL 500        // Also used for doing handshake

// Setup function ----------S----------S----------S----------S
void setup() {
  Serial.begin(9600);

  // Sets pin modes
  pinMode(Key1, INPUT);
  pinMode(Key2, INPUT);
  pinMode(Key3, INPUT);
  pinMode(Key4, INPUT);
  pinMode(Key5, INPUT);
  pinMode(Key6, INPUT);
  pinMode(Key7, INPUT);
  pinMode(INNER_LIMIT, INPUT);
  pinMode(OUTER_LIMIT, INPUT);
}

// Loop function ----------L----------L----------L----------L
void loop() {
  if (handshake_completed) {

    // Extends or retracts piano based on incoming msgs
    curr_state = checkState(curr_state);
  
    if (curr_state) {
      last_button_pressed = checkKeys(last_button_pressed);
    }

  } else { // Try to register with the computer
    if (Serial.available() > 0) { // The computer is responding
      String msg = Serial.readString();
      handshake_completed = msg.equals("Hello from computer");
      Serial.println(handshake_completed ? "Connected" : "Not connected");
    } else if (millis() > next_broadcast_time) { // Cry out for a computer
      Serial.println("Hello from pianoduino");
      next_broadcast_time = millis() + BROADCAST_INTERVAL;
    }
  }
}


// Functions ----------F----------F----------F----------F----------F

// Function to update piano state (extended/retracted) based on incoming msg; assumes serial port opened
int checkState(int state) {
  
  if (Serial.available()) {
    int msg = Serial.read() - 48;

    // If received retract msg
    if (msg == 0) {
      state = 0;
      // TODO: Write servo to retract
      
      // Waits until cart is fully retracted
      while (true) {
        //if (digitalRead(INNER_LIMIT)) {
        // Can do while (digitalRead(INNER_LIMIT) < INNER_LIMIT) {}
        if (true) {
          break;
        }
      }
    }

    // If received extend msg
    else if (msg == 1) {
      state = 1;
      // TODO: Write servo to extend
      
      // Waits until cart is fully extended
      while (true) {
        //if (digitalRead(OUTER_LIMIT)) {
        if (true) {
          break;
        }
      }
    }
  }

  return state;
}

// Function to send msg given pressed keys
int checkKeys(int last_button_pressed) {

  // Initialize temporary variables
  int button_pressed;
  
  // Update pressed button
  if (digitalRead(Key1) == HIGH)
  {
    button_pressed = 1;
  }
  else if (digitalRead(Key2) == HIGH)
  {
    button_pressed = 2;
  }
  else if (digitalRead(Key3) == HIGH)
  {
    button_pressed = 3;
  }
  else if (digitalRead(Key4) == HIGH)
  {
    button_pressed = 4;
  }
  else if (digitalRead(Key5) == HIGH)
  {
    button_pressed = 5;
  }
  else if (digitalRead(Key6) == HIGH)
  {
    button_pressed = 6;
  }
  else if (digitalRead(Key7) == HIGH)
  {
    button_pressed = 7;
  }
  else {
    button_pressed = 0;
  }

  // Send message if button_pressed has changed
  if (button_pressed != last_button_pressed) {
    //String output = String(button_pressed);
    Serial.println("{\"event_id\":\"2\",\"data\":\"" + String(button_pressed) + "\"}");
  }

  return button_pressed;
}

