#include <Arduino.h>   
#define LED_PIN       2        //on-board led pin (usually GPIO 2 on esp32)
#define BUTTON_PIN    32       //push button connected from GPIO32 to GND
#define DEBOUNCE_MS   50       //debounce delay time in milliseconds
#define DEBOUNCE_US   (DEBOUNCE_MS * 1000UL)   //convert debounce time into microseconds for timer use

hw_timer_t* debounceTimer = nullptr;//create a hardware timer pointer for debounce handling

volatile bool debounceActive = false;  //this flag is used to ignore extra button presses while debounce is active

//this function runs once after debounce delay to verify button press
void IRAM_ATTR onDebounceTimer() {
  
  if (digitalRead(BUTTON_PIN) == LOW) {   //if button is still pressed after debounce delay, treat it as valid press
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));      //toggle led state (on if off, off if on)
  }
  debounceActive = false;    //reset debounce flag so next press can be detected
}

//--- button interrupt service routine ---
void IRAM_ATTR onButtonISR() {   //this runs instantly when button is pressed (falling edge)
  //only start debounce timer if not already active
  if (!debounceActive) {
    debounceActive = true;
    //set timer to call onDebounceTimer() after debounce delay
    //false means timer runs one time only (not repeating)
    timerAlarmWrite(debounceTimer, DEBOUNCE_US, false);
    timerAlarmEnable(debounceTimer);
  }
}
void setup() {

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   //starts with led off

  //configure button as input with internal pull-up
  //it stays high normally and goes low when pressed
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(BUTTON_PIN, onButtonISR, FALLING);     //attach interrupt on button press (falling edge detection)

  //create hardware timer for debounce process
  //prescaler 80 means 1 tick = 1 microsecond
  debounceTimer = timerBegin(1, 80, true);     //timer number (0–3), prescaler (80), count-up mode (true)

  //attach debounce timer interrupt function
  //true means trigger on rising edge
  timerAttachInterrupt(debounceTimer, &onDebounceTimer, true);

  //no timer alarm active initially; it will start when button is pressed
}

void loop() {
  //nothing here because all work is handled by interrupts
  //led toggles automatically when button is pressed and debounced
}
