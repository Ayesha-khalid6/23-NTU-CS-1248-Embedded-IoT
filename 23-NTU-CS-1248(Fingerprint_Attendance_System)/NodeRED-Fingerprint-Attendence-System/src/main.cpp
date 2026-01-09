#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>

// ================= CONFIG =================
const char* ssid = "Warning";
const char* password = "Rohan1808";

const char* mqttServer = "192.168.32.1";
const int mqttPort = 1883;
const char* publishTopic = "home/fingerprint";
const char* subscribeTopic = "home/fingerprint/action";

// ================= OLED ===================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= FINGERPRINT =============
#define RXPIN 16
#define TXPIN 17
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger(&mySerial);

// ================= MQTT ===================
WiFiClient espClient;
PubSubClient client(espClient);

// ================= TIME ===================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// ================= STATE ==================
uint8_t lastFingerID = 0;

// ================= FUNCTION DECLARATIONS ===
void connectWiFi();
void connectMQTT();
void scanFingerprint();
void sendFingerprintData(uint8_t id, String name, String mode);
String getDateString();
String getTimeString();
void mqttCallback(char* topic, byte* payload, unsigned int length);

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Starting...");
  display.display();
  delay(1000); // show once

  // WiFi
  connectWiFi();

  // MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  connectMQTT();

  // Fingerprint init
  mySerial.begin(57600, SERIAL_8N1, RXPIN, TXPIN);
  finger.begin(57600);
  if (!finger.verifyPassword()) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("FP Sensor Error");
    display.display();
    while(true); // stop execution
  }

  // Time client
  timeClient.begin();
  timeClient.update();

  // Final ready message (only once)
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("System Ready");
  display.println("Place Finger");
  display.display();
}

// ================= LOOP ===================
void loop() {
  if (!client.connected()) connectMQTT();
  client.loop();
  timeClient.update();

  scanFingerprint(); // scan for finger

  delay(50); // small delay to avoid watchdog resets
}

// ================= WIFI ===================
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  unsigned long startAttempt = millis();
  bool wifiDisplayed = false;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (!wifiDisplayed && millis() - startAttempt > 2000) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Connecting to WiFi...");
      display.display();
      wifiDisplayed = true;
    }

    if (millis() - startAttempt > 20000) { // 20 sec timeout
      Serial.println("Failed to connect WiFi");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WiFi Failed");
      display.display();
      while (true); // stop execution
    }
  }

  Serial.println("Connected!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.println("System Ready");
  display.println("Place Finger");
  display.display();
  delay(1000);
}

// ================= MQTT ===================
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    if (client.connect("ESP32_Attendance")) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5s");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.print("MQTT received: ");
  Serial.println(msg);

  display.clearDisplay();
  display.setCursor(0, 0);

  if (msg == "enroll") {
    display.println("Enroll Mode Activated");
    display.display();
    delay(1000);
    display.clearDisplay();
    display.println("Place Finger");
    display.display();
  } else if (msg == "attendance") {
    display.println("Attendance Mode Activated");
    display.display();
    delay(1000);
    display.clearDisplay();
    display.println("Place Finger");
    display.display();
  }
}

// ================= FINGERPRINT =================
void scanFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return;

  uint8_t id = finger.fingerID;
  if (id == lastFingerID) return; // ignore same finger
  lastFingerID = id;

  String name = "Student_" + String(id);
  sendFingerprintData(id, name, "attendance");

  // wait until finger removed
  while (finger.getImage() != FINGERPRINT_NOFINGER) delay(50);
  lastFingerID = 0;
}

// ================= SEND DATA =================
void sendFingerprintData(uint8_t id, String name, String mode) {
  DynamicJsonDocument doc(256);
  doc["finger_id"] = id;
  doc["name"] = name;
  doc["mode"] = mode;
  doc["date"] = getDateString();
  doc["time"] = getTimeString();

  String payload;
  serializeJson(doc, payload);
  client.publish(publishTopic, payload.c_str());

  Serial.println("Attendance Sent:");
  Serial.println(payload);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Attendance Sent");
  display.println("ID: " + String(id));
  display.println(name);
  display.display();
  delay(1500);

  display.clearDisplay();
  display.println("Place Finger");
  display.display();
}

// ================= TIME HELPERS =================
String getDateString() {
  time_t rawTime = timeClient.getEpochTime();
  struct tm *timeInfo = localtime(&rawTime);

  int d = timeInfo->tm_mday;
  int m = timeInfo->tm_mon + 1;
  int y = timeInfo->tm_year + 1900;

  return String(d) + "/" + String(m) + "/" + String(y);
}

String getTimeString() {
  return timeClient.getFormattedTime();
}
