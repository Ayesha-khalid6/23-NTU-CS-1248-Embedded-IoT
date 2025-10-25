#include <Arduino.h>

const int buttonPin = 32;
const int ledPin = 4;
volatile bool ledState = LOW;

//this function runs automatically when the button interrupt occurs
void IRAM_ATTR handleButton() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, FALLING);
}

void loop() {
  //main loop free for other tasks
}
