#include <Arduino.h>
#include <Servo.h>

Servo m1;

// Message containers, buffers.
String HORIZONTAL = "";
String VERTICAL = "";
int INT_HOR = 0, INT_VER = 0;

const int NO_of_ELEMENTS = 2;
String received_values[NO_of_ELEMENTS] = {"", ""};
bool receive_flags[NO_of_ELEMENTS] = {false, false};

// Special characters for communucating.
const char begin_msg = '>'; // DEC 62	BIN 00111110
const char msg_breaker = '*';   // DEC 42 BIN 00101010
const char comma = ',';
const char CR = 0b00001010; // carriage return

// Flags
bool MSG_BEGUN = false;
bool RECEIVING_HORIZONTAL = false;
bool RECEIVING_VERTICAL = false;
bool MSG_FINISHED = true;

// Pin variables
const int M1 = 5; // Pin 5
const int M2 = 6; // Pin 6

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.print("Setup done."); Serial.write(CR);  // DEBUG LINE
  m1.attach(6);
}

void loop() {
  while(Serial.available() <= 0);
  char received = Serial.read();

  switch (received)
  {
  case begin_msg:
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.write(received);
    if(!MSG_BEGUN) {
      MSG_BEGUN = true;
    }
    receive_flags[0] = true;
    receive_flags[1] = false;
    break;
  
  case msg_breaker:
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.write(received);
    receive_flags[0] = false;
    receive_flags[1] = true;
    break;
  
  case CR:
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.write(received);
    Serial.print("HOR: "); Serial.print(received_values[0]);
    Serial.print(" VER: "); Serial.print(received_values[1]);
    Serial.write(CR);

    m1.write(received_values[0].toInt());

    MSG_BEGUN = false;
    MSG_FINISHED = true;
    receive_flags[0] = false;
    receive_flags[1] = false;
    received_values[0] = "";
    received_values[1] = "";
    break;

  default:
    Serial.write(received);
    if(MSG_BEGUN && receive_flags[0]) received_values[0] += received;
    if(MSG_BEGUN && receive_flags[1]) received_values[1] += received;
    break;
  }
}