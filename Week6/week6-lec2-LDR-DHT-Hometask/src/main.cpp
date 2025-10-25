#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Pin Configuration
#define DHTPIN 14          // DHT sensor data pin
#define DHTTYPE DHT11      // using DHT11 sensor

#define LDR_PIN 34       // LDR connected to ADC pin 34
#define SDA_PIN 21         // OLED SDA pin
#define SCL_PIN 22         // OLED SCL pin

//OLED Display Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// DHT Sensor Setup 
DHT dht(DHTPIN, DHTTYPE);

// Setup Function 
void setup() {
  Serial.begin(115200);
  Serial.println("Environmental Monitoring System");

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);  //initialize I2C communication between ESP32 and OLED

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed!");
    for (;;); // stop program if display is not connected properly
  }
  display.clearDisplay();   //clear any garbage from display before printing
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Initialize DHT sensor
  dht.begin();
  delay(1000);   //delay of 1 sec to stablize seensor
}

void loop() {    //main loop
  //read temperature and humidity from DHT
  float temperature = dht.readTemperature();  //read temperature in Celsius
  float humidity = dht.readHumidity();  //read humidity in percentage

  if (isnan(temperature) || isnan(humidity)) {   //check if DHT sensor reading failed
    Serial.println("Error reading DHT sensor!");
    return;   //skip the rest of the loop if data is invalid
  }

  //read light intensity value from LDR
  int adcValue = analogRead(LDR_PIN); //read analog value (0–4095 range)
  float voltage = (adcValue / 4095.0) * 3.3;  //convert ADC value to voltage

  //print readings to Serial Monitor
  Serial.printf("Temp: %.2f°C | Humidity: %.2f%% | LDR ADC: %d | Voltage: %.2f V\n",
                temperature, humidity, adcValue, voltage);

  //display readings on OLED
  //step 5: show data on oled screen
display.clearDisplay();          //clear old readings from display
display.setTextSize(1);          //set normal readable text size

//display heading title
display.setCursor(0, 0);         //set position for title at top
display.println("Env Monitoring Sys");   //print title

//display temperature reading
display.setCursor(0, 16);        //move cursor to next line for temperature
display.print("Temp: ");         //print label for temperature
display.print(temperature, 1);   //print actual temperature value with 1 decimal
display.println(" C");           //add unit celsius at the end

//display humidity reading
display.setCursor(0, 28);  
display.print("Humidity: ");  
display.print(humidity, 1); 
display.println(" %");           //add percentage symbol

//display ldr (light intensity)reading
display.setCursor(0, 40);        
display.print("Light: ");     
display.print(adcValue);         //show raw adc value from LDR sensor
display.println(" ADC");         //add unit label ADC

//display voltage converted from ldr reading
display.setCursor(0, 52);      
display.print("Volt: ");      
display.print(voltage, 2);       //print voltage value with 2 decimals
display.println(" V");           //add unit volts

display.display();               //refresh old screen to show updated values
delay(2000);                     //pause for 2 seconds before next update
