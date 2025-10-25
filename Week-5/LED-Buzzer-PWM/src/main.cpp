//Title: controlling LED Brightness and Buzzer sound using PWM

// This program demonstrates how to use PWM (Pulse Width Modulation)
// to control both the brightness of an LED and the sound patterns
// of a buzzer using ESP32. 
// The LED gradually fades in and out, while the buzzer plays
// a few simple sound patterns and a short melody.

#include <Arduino.h>

#define LED_PIN     18      //LED is connected to GPIO pin 18
#define BUZZER_PIN  27      //buzzer is connected to GPIO pin 27

//PWM Configuration
#define LED_CH      0       //PWM channel for LED (each device needs its own channel)
#define BUZZER_CH   1       // seprate PWM channel for Buzzer
#define LED_FREQ    5000    //frequency for LED PWM (higher = smoother brightness)
#define BUZZER_FREQ 2000    //default buzzer frequency in Hz
#define RESOLUTION  8       //8-bit resolution (means duty cycle can vary from 0–255) 256 levels/parts

void setup() {
  //initialize PWM for LED

  ledcSetup(LED_CH, LED_FREQ, RESOLUTION);   //sets up a PWM channel with 3 parameters

  ledcAttachPin(LED_PIN, LED_CH);   //links the chosen GPIO pin to the configured PWM channel

  //initialize PWM for Buzzer

  ledcSetup(BUZZER_CH, BUZZER_FREQ, 10);   //here we use 10-bit resolution for smoother sound control (0–1023 range)
  ledcAttachPin(BUZZER_PIN, BUZZER_CH);       //links the chosen GPIO pin to the configured PWM channel
}

void loop() {
  // 1. LED brightness Fade
  for (int d = 0; d <= 255; d++) {     // d is for duty cycle. Gradually increase brightness from 0 to 255 using PWM duty cycle
    // ledcWrite(channel, dutyCycle) 
    ledcWrite(LED_CH, d);    //sets brightness or output strength for the pin
    delay(5); //short delay to make fading visible
  }

  for (int d = 255; d >= 0; d--) {   //now gradually decrease brightness back to zero
    ledcWrite(LED_CH, d);
    delay(5);
  }

  // 2. Buzzer Beep pattern

  for (int i = 0; i < 3; i++) {   //produce 3 short beeps, each with slightly higher frequency

    ledcWriteTone(BUZZER_CH, 2000 + i * 300);  // generates a tone using PWM..increase pitch for each beep
    delay(200); 
    ledcWrite(BUZZER_CH, 0);   // stop tone (turn off buzzer)
    delay(200);  //short gap between beeps
  }

  //3. Buzzer frequency Sweep
  // This loop slowly increases frequency to make a “rising pitch” effect
  for (int f = 400; f <= 3000; f += 100) {
    ledcWriteTone(BUZZER_CH, f);  //generate increasing frequency
    delay(15);
  }
  ledcWrite(BUZZER_CH, 0);        //stop buzzer after sweep
  delay(300);

  // 4. Simple Melody
  // Play a small sequence of musical notes
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};  // frequencies of notes
  for (int i = 0; i < 8; i++) {
    ledcWriteTone(BUZZER_CH, melody[i]);  // play each note one by one
    delay(250);                           // short delay for each note
  }
  ledcWrite(BUZZER_CH, 0);                // stop buzzer after melody

  delay(1000);  // waits for 1 sec before repeating the whole pattern again
}
