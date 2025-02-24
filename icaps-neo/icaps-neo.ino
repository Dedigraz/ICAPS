#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "helper.h"

// -- Adjust these to match your environment or reuse what you had in icaps-web-test
#define WIFI_SSID "CarWifi"
#define WIFI_PASS "ICAPSWifi"

#define WS_URL "192.168.10.163"  // Replace with your server IP or host name
#define WS_PORT 5007            // Replace with your server websocket port

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;


Queue *moves = {0};
LatLng currentLocation;
bool isRunning = false; // Whether to process moves


// Initialize motors the same way you already do
void initMotors() {
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);

  // Set default low outputs
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
}

// Standard setup
void setup()
{
#ifdef USE_SERIAL
  USE_SERIAL.begin(115200);
#endif

  initMotors();
  moves = createQueue();
  makeSquarePattern(moves, 5.0, DEFAULT_MOTOR_SPEED); 
  icaps2_init();
  USE_SERIAL.println("Please input anomaly string");

  // Setup Wi-Fi
  WiFi.disconnect();
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // Setup WebSocket client
  webSocket.begin(WS_URL, WS_PORT, "/log");  // The path "/log" matches the example
  delay(100);
  webSocket.onEvent(webSocketEvent);

  // If needed, configure time
  configTime(3600, 0, "pool.ntp.org"); // Example: offset=3600, server=pool.ntp.org
}

LatLng posFromStr(const char *po)
{
  USE_SERIAL.println("3");
  LatLng result;
  char pos[20] = "";
  strcpy(pos, po);
  USE_SERIAL.println("3.5");

  // Use strtok to split the string by commas
  char *token = strtok(pos, ",");
  if (token != nullptr)
  {
    result.lat = atof(token); // Convert latitude to float
  }
  USE_SERIAL.println("4.0");

  token = strtok(nullptr, ",");
  if (token != nullptr)
  {
    result.lng = atof(token); // Convert longitude to float
  }

  USE_SERIAL.println("4.1");
  return result;
}

Anomaly processAnomaly(const char *json)
{
  JsonDocument doc;
  // char* newJson = strcpy(newJson, json);
  // Deserialize the JSON document
  USE_SERIAL.println("1");
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return {0};
  }
  const char *pos = doc["position"];
  USE_SERIAL.println("2");
  Anomaly a = {.x = doc["x"], .width = doc["width"], .height = doc["height"], .type = doc["type"], .vibration = doc["vibration"], .position = posFromStr(pos)};

  return a;
}

// If you still accept commands from Serial, keep your process_data code
void process_data(const char *data) {
  USE_SERIAL.println(data);

  // Toggle movement based on text commands
  if (strcmp(data, "START") == 0) {
    isRunning = true;
    USE_SERIAL.println("Movement started");
    return; // no further processing
  } else if (strcmp(data, "STOP") == 0) {
    isRunning = false;
    USE_SERIAL.println("Movement stopped");
    // Optionally clear the queue
    while (!isEmpty(moves)) {
      dequeue(moves);
    }
    return; // no further processing
  }

  // If not a START/STOP command, check if JSON
  if (data[0] == '{') {
    Anomaly a = processAnomaly(data);
    USE_SERIAL.println("5");
    Move *emergencyMove = icaps2_run(&a);

    if (emergencyMove != NULL) {
      insertEmergencyMove(moves, emergencyMove);
    }
  }
} // end of process_data

void processIncomingByte(const byte inByte)
{
  static char input_line[max_input];
  static unsigned int input_pos = 0;

  switch (inByte)
  {

  case '\n':                   // end of text
    input_line[input_pos] = 0; // terminating null byte

    // terminator reached! process input_line here ...
    process_data(input_line);

    // reset buffer for next time
    input_pos = 0;
    USE_SERIAL.println("Please input anomaly string");
    break;

  case '\r': // discard carriage return
    break;

  default:
    // keep adding if not full ... allow for terminating null byte
    if (input_pos < (max_input - 1))
      input_line[input_pos++] = inByte;
    break;

  } // end of switch

} // end of processIncomingByte
// Event handler for websocket events
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
      // Send message to server when connected
      webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      // Use the same logic as process_data() to handle START/STOP/JSON
      // Convert payload to char*, or interpret it directly
      if (strcmp((char*)payload, "START") == 0) {
        isRunning = true;
        USE_SERIAL.println("Movement started");
      } else if (strcmp((char*)payload, "STOP") == 0) {
        isRunning = false;
        USE_SERIAL.println("Movement stopped");
        while (!isEmpty(moves)) {
          dequeue(moves);
        }
      } else if (payload[0] == '{') {
        // Likely a JSON anomaly
        Anomaly a = processAnomaly((char*)payload);
        USE_SERIAL.println("5");
        Move *emergencyMove = icaps2_run(&a);
        if (emergencyMove != NULL) {
          insertEmergencyMove(moves, emergencyMove);
        }
      }
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      // Echo the binary data back if you want
      webSocket.sendBIN(payload, length);
      break;
    case WStype_ERROR:
      USE_SERIAL.printf("[WSc] error occurred: %s\n length: %u", payload, length);
      break;
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      // Not used in a simple scenario
      break;
  }
  // Try reconnecting every 5s if the connection fails
  webSocket.setReconnectInterval(5000);
}

void loop()
{
  // Process WebSocket events
  webSocket.loop();

  // Process incoming serial data
  while (Serial.available() > 0)
  {
    processIncomingByte(Serial.read());
  }

  // Only move if isRunning is true
  if (isRunning) {
    processCurrentMove(moves, millis());
  }

  // do other stuff here like testing digital input (button presses) ...

  // getCurrentLocation();
  // if(currentLocation > anomalies->Head->data->position){
  //   dequeue(anomalies);
  // }

  // if(distanceFromHead(currentLocation) < 2){ // if the distance from the current location and the closest anomaly is less than 2 metres
  //   long long int startTime = 12312324324;
  //   icaps2_run(anomalies->Head->data, followRoute);//carry out fuzzy suggestion
  //   hasLeftAnomaly(anomalies->Head, startTime); //leave the function after anomaly has been left
  // }
  // followRoute(BLANK_SKEW,DEFAULT_MOTOR_SPEED);
}