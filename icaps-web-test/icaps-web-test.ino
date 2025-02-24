#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

#include "TinyGPS++.h"
#include "math.h"
#include <ArduinoJson.h>
#include <ESP32Servo.h>

#include "time.h"

// Define the RX and TX pins for Serial 2
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600

#define WIFI_SSID "CarWifi" //"NOKIA-A130"
#define WIFI_PASS "ICAPSWifi" //"NancyG00ber"

#define WS_URL "192.168.10.163"
#define WS_PORT 5007


Servo steeringServo;
float roadWidth, lat, lng = 0;
String maneuver = "DRIVING";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 0;

int cursor = 0;
// The TinyGPS++ object
TinyGPSPlus gps;
// Create an instance of the HardwareSerial class for Serial 2
HardwareSerial gpsSerial(2);

#define USE_SERIAL Serial
WiFiMulti WiFiMulti;

WebSocketsClient webSocket;


void hexdump(const void* mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*)mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++) {
    if (i % cols == 0) {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  USE_SERIAL.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  printLocalTime();
  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

      //send message to server when Connected
      webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);

      //send message to server
      webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      //  send data to server
      webSocket.sendBIN(payload, length);
      break;
    case WStype_ERROR:
      USE_SERIAL.printf("[WSc] error occured: %s\n length: %u", payload, length );
      break;
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
  //try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
}
void sendUpdate() {
  DynamicJsonDocument doc(1024);
  doc["userId"] = "Controller";
  doc["lat"] = lat;
  doc["lng"] = lat;
  doc["maneuver"] = maneuver;

  //Replace with actual maneuver
  // doc["timestamp"] = getTimestamp();
  // //Implement this function to get current timestamp
  // doc["speed"] = currentSpeed;
  // doc["heading"] = currentHeading;

  String updateString;
  serializeJson(doc, updateString);
  webSocket.sendTXT(updateString);
}


void setup() {
  USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);

  // Serial.setDebugOutput(true);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

  WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  //server address, port and URL
  // webSocket.beginSSL(WS_URL, WS_PORT, "/");
  webSocket.begin(WS_URL, WS_PORT, "/log");
  delay(100);

  //event handler
  webSocket.onEvent(webSocketEvent);

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();


  //Start Serial 2 with the defined RX and TX pins and a baud rate of 9600 gps
  Serial2.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 started at 9600 baud rate");
}

void loop() {
  //This sketch displays information every time a new sentence is correctly encoded.unsigned long start = millis();
  while (Serial2.available() > 0)
    gps.encode(Serial2.read());                                                                                                                                                                                           
  webSocket.loop();

  // Serial.printf("LAT =%.6f\n", gps.location.lat());
  // Serial.printf("LNG =%.6f\n", gps.location.lng());
  for(size_t i = 0; i < 1; i++){
    
    sendUpdate();
  }
}