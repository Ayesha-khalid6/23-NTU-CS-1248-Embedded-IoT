#include <Arduino.h>     //included for Arduino functions like pinMode and digitalWrite

#define LED 4            //LED connected to GPIO pin 4

//Created a pointer for the timer — this will store the hardware timer configuration
hw_timer_t *My_timer = NULL;

//IRAM_ATTR means the function is stored directly in RAM (for faster interrupt response)
void IRAM_ATTR onTimer() {  // This function runs automatically every time the timer interrupt occurs
  // Read the current state of the LED and set it to the opposite (toggle effect)
  digitalWrite(LED, !digitalRead(LED));
}

void setup() {
  pinMode(LED, OUTPUT);     //setting the LED pin as output mode

//initialize the timer
//parameters: timer number (0–3), prescaler (80), count-up mode (true)
//prescaler 80 means 1 tick = 1 microsecond (because 80MHz / 80 = 1MHz)
  My_timer = timerBegin(0, 80, true);   

  //attach interrupt function (onTimer) to the timer
  //the last argument 'true' means the interrupt will trigger on the rising edge
  timerAttachInterrupt(My_timer, &onTimer, true);

  // Set the alarm — how many microseconds before the interrupt should occur
  // Here, 1,000,000 µs = 1 sec
  // 'true' means the timer will automatically repeat
  timerAlarmWrite(My_timer, 1000000, true);

  //enable the alarm — without enabling, the timer won’t start working
  timerAlarmEnable(My_timer);

  //now, the timer will call onTimer() every 1 second, and the LED will blink
}

void loop() {
  //nothing to write here — the LED blinking is fully handled by the timer interrupt
}
