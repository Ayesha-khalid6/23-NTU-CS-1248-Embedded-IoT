//Assignment 1 - Task A
//Name: Ayesha Khalid
//Reg no: 23-NTU-CS-1248


#include<Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include<Arduino.h>
const int button=25;
int buttonstate=0;
const int led1=2;
const int led2=4; 
const int led3=18;
volatile bool ledmode;
bool buttonpressed=false;
unsigned long timesincestart = 0;
unsigned long pressduration=0;
#define BUZZER_PIN 27
#define PWM_CH 0
#define FREQ 5000
#define RES 8
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);
void setup() {
  pinMode(button, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  ledcSetup(PWM_CH,FREQ,RES);
  ledcAttachPin(BUZZER_PIN,PWM_CH);
  Wire.begin(21,22);
  display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
}
void loop() {
  buttonstate=digitalRead(button);
  if (buttonstate==LOW && !buttonpressed){ 
    buttonpressed=true;
    timesincestart=millis();
  }
  if(buttonstate==HIGH && buttonpressed){ 
    buttonpressed=false;
    pressduration=millis()-timesincestart;
  } 
  if (pressduration == 0) {
    
    return;
}
  else if(pressduration<1500){
        ledmode=true;
  }
  else{
        ledmode=false;
  }
  delay(50);
  if(ledmode==true){
       ledcWrite(PWM_CH, 0); // turns off buzzer PWM output
       display.clearDisplay();
       display.setCursor(0,0);
       display.println();
       display.println("Toggle");
       display.display();
       digitalWrite(led1, HIGH);
       digitalWrite(led2, LOW);
       digitalWrite(led3, LOW);
       delay(500);
        digitalWrite(led1,LOW);
       digitalWrite(led2, HIGH);
       digitalWrite(led3, LOW);
       delay(500);
        digitalWrite(led1,LOW);
       digitalWrite(led2, LOW);
       digitalWrite(led3, HIGH);
       delay(500);
  }
  else if(ledmode==false){
       display.clearDisplay();
       display.setCursor(0,0);
       display.println();
       display.println("Playing     Buzzer");
       display.display();
       digitalWrite(led1, LOW);
       digitalWrite(led2, LOW);
       digitalWrite(led3, LOW);
       for(int i=0; i<3; i++){
       ledcWriteTone(PWM_CH,2000+i*400); 
       delay(150);
       ledcWrite(PWM_CH,0);
       delay(150);}
  }}
