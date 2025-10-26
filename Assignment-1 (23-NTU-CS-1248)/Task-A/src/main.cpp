/*
Assignment 1 - Task A
Name: Ayesha Khalid
Reg no: 23-NTU-CS-1248

Description:
Button 1 cycles through 4 LED modes:
  0: Both OFF
  1: Alternate Blink
  2: Both ON
  3: PWM Fade (LED3 only)
Button 2 resets LEDs to OFF
OLED displays current mode/state
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//pin setup
#define LED1 4
#define LED2 2
#define LED3 18     // PWM fade LED
#define BUTTON_MODE 32
#define BUTTON_RESET 33
#define OLED_SDA 21
#define OLED_SCL 22

//OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//PWM setup
#define FREQ 5000
#define RESOLUTION 8
#define PWM_CH 0   // for LED3 (fade)

//variables
int mode = 0;   // current LED mode (0–3)
bool lastButton1 = HIGH;
bool lastButton2 = HIGH;
unsigned long lastDebounce = 200;

void showMode(int m) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32 LED Modes");
  display.print("Mode: ");
  display.println(m);
  switch (m) {
    case 0: display.println("Both OFF"); break;
    case 1: display.println("Alternate Blink"); break;
    case 2: display.println("Both ON"); break;
    case 3: display.println("PWM Fade (LED3)"); break;
  }
  display.display();
}

//setup
void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);

  // setup PWM for LED3 fade
  ledcSetup(PWM_CH, FREQ, RESOLUTION);
  ledcAttachPin(LED3, PWM_CH);

  showMode(mode);
  Serial.println("Task A Ready...");
}

//loop
void loop() {
  bool b1 = digitalRead(BUTTON_MODE);
  bool b2 = digitalRead(BUTTON_RESET);

  //button 1(for next Mode)
  if (b1 == LOW && lastButton1 == HIGH && millis() - lastDebounce > 250) {
    mode = (mode + 1) % 4;
    showMode(mode);
    lastDebounce = millis();
  }

  //button 2(reset to OFF)
  if (b2 == LOW && lastButton2 == HIGH && millis() - lastDebounce > 250) {
    mode = 0;
    showMode(mode);
    lastDebounce = millis();
  }

  lastButton1 = b1;
  lastButton2 = b2;

  //LED behavior according to Mode
  switch (mode) {
    case 0: //both OFF
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      ledcWrite(PWM_CH, 0);
      break;

    case 1: // Alternate blink
      ledcWrite(PWM_CH, 0);          //to make sure that fade LED is off
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      delay(400);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      delay(400);
      break;

    case 2: //both ON (only LED1 and LED2)
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      ledcWrite(PWM_CH, 0); //ensure LED3 off
      break;

    case 3: // PWM fade (only LED3)
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW); //turn off other LEDs
      for (int d = 0; d <= 255; d++) {
        ledcWrite(PWM_CH, d);
        delay(5);
      }
      for (int d = 255; d >= 0; d--) {
        ledcWrite(PWM_CH, d);
        delay(5);
      }
      break;
  }
}
