/*******************************************************
 * Project: IoT Fingerprint Attendance System
 * Hardware: ESP32 + AS608 + OLED 0.96"
 * Cloud: Blynk + Google Sheets
 *******************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6QzJtx4fV"
#define BLYNK_TEMPLATE_NAME "Fingerprint Attendance System"
#define BLYNK_AUTH_TOKEN "SUmGJS-2acHjOma3I7ieJaSzsdawXPcp"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <time.h>

// ---------------- WiFi & NTP ----------------
const char* ssid = "Warning";
const char* password = "Rohan1808";
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 5*3600;   // Pakistan GMT+5
const int daylightOffset_sec = 0;

// ---------------- Google Sheets ----------------
const char* GOOGLE_URL = "https://script.google.com/macros/s/AKfycbyymSk3q1GZhldUBV8mJGpc7JuDoosStaIkitAyxZYenrHp6vhsUWugv2IZHVLhWcQ/exec"; // Deploy your Apps Script

// ---------------- Blynk Variables ----------------
bool enrollMode = false;
bool attendanceMode = false;
String newUserName = "";

// ---------------- OLED Setup ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Fingerprint Setup ----------------
#define RX_PIN 16
#define TX_PIN 17
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger(&mySerial);

// ---------------- User Storage ----------------
#define MAX_USERS 50
String userNames[MAX_USERS];           // Finger ID -> Name
String lastAttendanceDate[MAX_USERS];  // Track last attendance date to prevent duplicates

// ---------------- Function Prototypes ----------------
String getCurrentDateTime();
void sendData(int id);
void startEnrollment(String name);
void scanFingerprint();
void oledMessage(String line1, String line2);

// ---------------- Blynk Callbacks ----------------
BLYNK_WRITE(V0){ enrollMode = param.asInt(); }       // Enroll button
BLYNK_WRITE(V1){ attendanceMode = param.asInt(); }  // Attendance button
BLYNK_WRITE(V4){ newUserName = param.asStr(); }     // Name input

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);

  // OLED Init
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();
  oledMessage("System Booting", "Please wait...");

  // Fingerprint sensor init
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);
  finger.begin(57600); // returns void

  // Verify sensor
  uint8_t p = finger.getTemplateCount();
  if (p != FINGERPRINT_OK){
      oledMessage("Fingerprint", "Sensor Error!");
      Serial.println("Fingerprint sensor not found!");
      while(1);
  }
  Serial.println("Fingerprint sensor ready!");
  Serial.println("Templates stored: " + String(finger.templateCount));

  // WiFi
  WiFi.begin(ssid, password);
  oledMessage("Connecting", "to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Blynk.run(); // keep Blynk alive
    delay(200);
    Serial.print(".");
  }
  oledMessage("WiFi Connected", WiFi.localIP().toString());

  // NTP Time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  Serial.println("Waiting for NTP time...");
  while(!getLocalTime(&timeinfo)){
      delay(500);
      Serial.print(".");
  }
  Serial.println("Time synced!");

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  oledMessage("System Ready", "Scan Finger");
}

// ---------------- Loop ----------------
void loop() {
  Blynk.run();

  // Enrollment workflow
  if(enrollMode){
    if(newUserName == ""){
      Blynk.virtualWrite(V2, "Enter Name First!");
      oledMessage("Enter Name", "");
    } else {
      startEnrollment(newUserName);
      newUserName = "";
    }
    enrollMode = false;
  }

  // Attendance workflow
  if(attendanceMode){
    scanFingerprint();
    attendanceMode = false;
  }
}

// ---------------- OLED ----------------
void oledMessage(String line1, String line2){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(line1);
  display.println(line2);
  display.display();
}

// ---------------- Time ----------------
String getCurrentDateTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "00-00-0000|00:00";
  }
  char dateStr[11]; 
  char timeStr[6];  
  strftime(dateStr, sizeof(dateStr), "%d-%m-%Y", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
  return String(dateStr) + "|" + String(timeStr);
}

// ---------------- Google Sheets ----------------
void sendData(int id){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String dt = getCurrentDateTime();
    int sep = dt.indexOf("|");
    String currentDate = dt.substring(0, sep);
    String currentTime = dt.substring(sep+1);

    // Prevent duplicate attendance for today
    if(lastAttendanceDate[id] == currentDate){
        Serial.println("Attendance already marked today for ID: " + String(id));
        Blynk.virtualWrite(V2, "Already marked today: " + userNames[id]);
        oledMessage("Already Marked", userNames[id]);
        return;
    }
    lastAttendanceDate[id] = currentDate;

    String url = String(GOOGLE_URL) +
      "?id=" + String(id) +
      "&name=" + userNames[id] +
      "&date=" + currentDate +
      "&time=" + currentTime;

    http.begin(url);
    int httpResponseCode = http.GET();
    if(httpResponseCode>0){
      Serial.println("Data sent to Google Sheets");
      Blynk.virtualWrite(V2, "Attendance Marked: " + userNames[id]);
      Blynk.virtualWrite(V3, userNames[id]);
      oledMessage("Attendance", "Marked: " + userNames[id]);
    } else{
      Serial.println("Error sending data");
      Blynk.virtualWrite(V2, "Error sending data");
      oledMessage("Error!", "Data not sent");
    }
    http.end();
  }
}

// ---------------- Enrollment ----------------
void startEnrollment(String name){
    int id = 1;
    while(userNames[id] != "" && id < MAX_USERS) id++;
    userNames[id] = name;

    oledMessage("Place Finger", name);
    Serial.println("Place finger to enroll...");
    while(finger.getImage() != FINGERPRINT_OK){
      Blynk.run();
      delay(100);
    }
    finger.image2Tz(1);

    oledMessage("Remove Finger", "");
    Serial.println("Remove finger");
    delay(2000);

    oledMessage("Place Again", "");
    while(finger.getImage() != FINGERPRINT_OK){
      Blynk.run();
      delay(100);
    }
    finger.image2Tz(2);

    finger.createModel();
    finger.storeModel(id);

    oledMessage("Enrolled!", name);
    Serial.println("Fingerprint stored for: " + name);

    Blynk.virtualWrite(V2, "Enrolled: " + name);
    Blynk.virtualWrite(V3, name);
}

// ---------------- Attendance ----------------
void scanFingerprint(){
  oledMessage("Place Finger", "");
  Serial.println("Place finger for attendance");
  while(finger.getImage() != FINGERPRINT_OK){
    Blynk.run();
    delay(100);
  }
  finger.image2Tz();
  if(finger.fingerFastSearch() == FINGERPRINT_OK){
    int id = finger.fingerID;
    Serial.println("Fingerprint matched ID: " + String(id));
    sendData(id);
  } else{
    Serial.println("Fingerprint not recognized");
    Blynk.virtualWrite(V2, "Fingerprint Not Recognized");
    oledMessage("Not Recognized","");
  }
}
