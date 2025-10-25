#include <Arduino.h>   //basic arduino functions for platformio

const int ledPin = 4;      //first led pin set at pin 4
const int led2pin = 2;     //second led pin

void setup() {
  pinMode(ledPin, OUTPUT);     //set both leds as output
  pinMode(led2pin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);   //turn on first led
  digitalWrite(led2pin, LOW);   //turn off second led
  delay(500);                   //wait half second

  digitalWrite(ledPin, LOW);    //turn off first led
  digitalWrite(led2pin, HIGH);  //turn on second led
  delay(500);                   //wait half second
}
