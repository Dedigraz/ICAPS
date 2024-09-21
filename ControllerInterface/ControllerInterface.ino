// #include "SoftwareSerial.h"
/*****************************
DO NOT USE GPIO PINS 9,10,11, 6, 7, 8
*****************************/
#include <Ramp.h>
#include "float.h"
#include "InterpolationLib.h"
#include "math.h"


#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>


#define GPSAvailable 0
#define GpsPIN 12

#include <TinyGPS.h>
TinyGPS gps;

// #endif
#define UART1 21
#define UART2 22 

#define NumOfAnomalies 22

#define ft 0.293

// DC Motor Driver Pins
#define LEFT_MOTOR_ENABLE 12
#define LEFT_MOTOR_IN1 13
#define LEFT_MOTOR_IN2 14
#define RIGHT_MOTOR_ENABLE 15
#define RIGHT_MOTOR_IN1 16
#define RIGHT_MOTOR_IN2 17

// Servo Motor Pin
#define STEERING_SERVO_PIN 18

Servo steeringServo;

// Motor speed constants
#define MAX_SPEED 255
#define NORMAL_SPEED 200
#define SLOW_SPEED 100
#define STOP_SPEED 0

// Steering angle constants
#define CENTER_ANGLE 90
#define MAX_LEFT_ANGLE 45
#define MAX_RIGHT_ANGLE 135
// #define traversalTime 500 //in ms
float roadWidth;

enum AnomalyType{
  POTHOLE, 
  SPEEDBUMP,
  COLLISION,
  ACCIDENT_SCENE,
  CRACK,
  MANHOLE
};
typedef struct {
  rampFloat lat;
  rampFloat lng;
} GpsLocation;

typedef struct {
  float lat;
  float lng;
  float offset; // distance of the anomaly from the center of the road
  float width; // in meters
  float height;
  enum AnomalyType type;
} Anomaly;


// struct Anomaly anomalies[12];
Anomaly* anomalies;
GpsLocation gpsLocation = {0};
int cursor=0;
// float lat;
// float lng;

const char* ssid = "CarWifi";
const char* password = "ICAPSWifi";
const char* wsHost = "your-api-host.com";
const int wsPort = 80;
const char* wsCommandPath = "/ws/command";
const char* wsUpdatePath = "/ws/update";

WebSocketsClient webSocketCommand;
WebSocketsClient webSocketUpdate;

void setup() {
  Serial.begin(115200);
  Serial.println("Check 0");
  setupWiFi();
  setupMotors();
  Serial.println("Check 1");
  webSocketCommand.begin(wsHost, wsPort, wsCommandPath);
  webSocketCommand.onEvent(webSocketCommandEvent);
  Serial.println("Check 2");
  
  webSocketUpdate.begin(wsHost, wsPort, wsUpdatePath);
  webSocketUpdate.onEvent(webSocketUpdateEvent);
  Serial.println("Check 3");
  
  populateAnomalies();

  Serial.println("Check 4");
  pinMode(1, OUTPUT);
  pinMode(UART1, OUTPUT);
  pinMode(UART2, OUTPUT);
  Serial.println("Check 5");
  
  if (GPSAvailable)
    pinMode(GpsPIN, INPUT);

  updateGpsLocation(true);
}

void loop() {
  Serial.println("Check 6");
  webSocketCommand.loop();
  webSocketUpdate.loop();

  Serial.println("Check 7");
  if (gpsLocation.lng.isFinished()) {
    updateGpsLocation();
    sendUpdate();
  }

  Serial.println("Check 8");
  float currentLat = gpsLocation.lat.update();
  float currentLng = gpsLocation.lng.update();

  // Check for nearby anomalies
  Serial.println("Check 9");
  for (int i = 0; i < NumOfAnomalies; i++) {
    float distance = calculateDistance(currentLat, currentLng, anomalies[i].lat, anomalies[i].lng);
    if (distance < 10) { // If within 10 meters of an anomaly
      executeManeuver(&anomalies[i]);
      break;
    }
  }

  Serial.println("Check 10");
  // Normal driving when no anomalies are detected
  moveForward(NORMAL_SPEED);

  serial_printi("Lat: %f, Lng: %f ", currentLat, currentLng);
  Serial.println();

  delay(100); // Short delay to prevent overwhelming the system
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void webSocketCommandEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("Command WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("Command WebSocket connected");
      break;
    case WStype_TEXT:
      handleCommand((char*)payload);
      break;
  }
}

void webSocketUpdateEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("Update WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("Update WebSocket connected");
      break;
  }
}

void handleCommand(char* payload) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  
  String command = doc["command"];
  if (command == "start") {
    // Implement start logic
  } else if (command == "stop") {
    // Implement stop logic
  } else if (command == "route") {
    // Update route information
    JsonArray route = doc["route"];
    // Process and store route information
  }
}

void sendUpdate() {
  DynamicJsonDocument doc(1024);
  doc["lat"] = gpsLocation.lat.update();
  doc["lng"] = gpsLocation.lng.update();
  doc["maneuver"] = "current_maneuver"; // Replace with actual maneuver

  String updateString;
  serializeJson(doc, updateString);
  webSocketUpdate.sendTXT(updateString);
}

void populateAnomalies(){
  anomalies = (Anomaly*) malloc(sizeof(Anomaly) * NumOfAnomalies);
  roadWidth = 20 * ft; //width of the road
  anomalies[0] =  { 9.53709, 6.46616, 0, 0.5f * roadWidth, 2.5 * ft, CRACK };
  anomalies[1] =  { 9.53708, 6.46587, 0, roadWidth, 3.3 * ft, SPEEDBUMP };
  anomalies[2] =  { 9.53704, 6.46561, 0, roadWidth, 2.75 * ft, SPEEDBUMP };
  anomalies[3] =  { 9.53703, 6.46549, 0, roadWidth, 3.0 * ft, SPEEDBUMP };
  anomalies[4] =  { 9.53700, 6.46523, 0, roadWidth, 12 * ft , CRACK };
  anomalies[5] =  { 9.53696, 6.46452, 3.5 * ft,  2.8 *ft,3*ft , POTHOLE };
  anomalies[6] =  { 9.53697, 6.46437, 1.875 * ft,  8*ft, 4*ft, POTHOLE };
  anomalies[7] =  { 9.53690, 6.46370, 6.5*ft, 2.5*ft, 4*ft, POTHOLE };
  anomalies[8] =  { 9.53775, 6.45959, 4*ft, 5.25*ft, 8.0*ft, POTHOLE };
  anomalies[9] =  { 9.53779, 6.45947, 1.5*ft, 14*ft, 5.5*ft, POTHOLE };
  anomalies[10] = { 9.53744, 6.45620, 8*ft, 1*ft, 1.5*ft, POTHOLE };
  anomalies[11] = { 9.53734, 6.45604, -7.0 * ft, 2.5*ft, 3*ft, CRACK };
  anomalies[12] = { 9.53732, 6.45602, -4.3 *ft, 6*ft, 5*ft, CRACK };
  anomalies[13] = { 9.53705, 6.45567, 0, 7*ft, 3*ft, POTHOLE };
  anomalies[14] = { 9.53647, 6.45452, 0, roadWidth, 2*ft, POTHOLE };
  anomalies[15] = { 9.53625, 6.45444, 7*ft, 5*ft, ft, POTHOLE };
  anomalies[16] = { 9.53629, 6.45477, 0, 1.5*ft, roadWidth, POTHOLE };
  anomalies[17] = { 9.53721, 6.46071, -5.5*ft, 5*ft, 3*ft, POTHOLE };
  anomalies[18] = { 9.53680, 6.46428, 0, roadWidth, 3.5*ft, SPEEDBUMP };
  anomalies[19] = { 9.53691, 6.46473, 3.5*ft, 9*ft, 3*ft, SPEEDBUMP };
  anomalies[20] = { 9.53695, 6.46519, 0, roadWidth, 3*ft, SPEEDBUMP };
  anomalies[21] = { 9.53703, 6.46720, 0,roadWidth ,7.3*ft, SPEEDBUMP };
}

void updateGpsLocation(bool init = false){
  Serial.println("Upating location...");
  
  
  GpsLocation waypoints[14] ={{9.537185, 6.467586},
                              {9.537005, 6.464765},
                              {9.536846, 6.462479},//
                              {9.537964, 6.458881},//
                              {9.537502, 6.460291},//
                              {9.538137, 6.458413},//
                              {9.536518, 6.454701},//
                              {9.536232, 6.454443},//
                              {9.536370, 6.454733},//
                              {9.537841, 6.457093},//
                              {9.537947, 6.458595},//
                              {9.536920, 6.461439},//
                              {9.536709, 6.463177},//
                              {9.537047, 6.467479}};//
    
  uint32_t traversalTime = 0;
  
  if(init== false){
    // traversalTime  = 1000.* (float)abs(waypoints[cursor-1].lng.getValue() - waypoints[cursor].lng.getValue());
    traversalTime = 1;
  }

  Serial.print("Moving would take" );
  Serial.print(traversalTime);
  Serial.println();

  gpsLocation.lat.go( waypoints[cursor].lat.update(),traversalTime *1000, LINEAR, ONCEFORWARD);
  gpsLocation.lng.go( waypoints[cursor].lng.update(),traversalTime*1000, LINEAR, ONCEFORWARD);
  cursor+=1;
}

void serial_printi(const char *format, ...) {
    char ch;
    bool flgInterpolate = false;
    va_list args;
    va_start(args, format);
    for (; *format; ++format) {
        ch = *format;
        if (flgInterpolate) {
            flgInterpolate = false;
            switch (ch) {
                case 'd':
                case 'i':
                    Serial.print(va_arg(args, int));
                    break;
                case 'u':
                    Serial.print(va_arg(args, unsigned int));
                    break;
                case 'c':
                    Serial.print((char)va_arg(args, int));
                    break;
                case 's':
                    Serial.print(va_arg(args, char*));
                    break;
                case 'x':
                case 'X':
                    Serial.print(va_arg(args, unsigned int), HEX);
                    break;
                case 'b':
                    Serial.print(va_arg(args, unsigned int), BIN);
                    break;
                case 'o':
                    Serial.print(va_arg(args, unsigned int), OCT);
                    break;
                case 'f':
                case 'F':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                    Serial.print(va_arg(args, double), 5);  // Print out decimals to 5 s.f.
                    break;
                case 'p':
                    Serial.print((uintptr_t)va_arg(args, void*), HEX);
                    break;
                case '%':
                    Serial.print('%');
                    break;
                default:
                    Serial.print('%');
                    Serial.print(ch);
                    break;
            }
        } else if (ch == '%') {
            flgInterpolate = true;
        } else if (ch == '\\' && *(format + 1) == 'n') {
            Serial.println();
            ++format;  // Skip the 'n'
        } else {
            Serial.print(ch);
        }
    }
    va_end(args);
}

void setupMotors() {
  pinMode(LEFT_MOTOR_ENABLE, OUTPUT);
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_ENABLE, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  
  steeringServo.attach(STEERING_SERVO_PIN);
  steeringServo.write(CENTER_ANGLE);
}

void setLeftMotorSpeed(int speed) {
  analogWrite(LEFT_MOTOR_ENABLE, abs(speed));
  digitalWrite(LEFT_MOTOR_IN1, speed > 0 ? HIGH : LOW);
  digitalWrite(LEFT_MOTOR_IN2, speed > 0 ? LOW : HIGH);
}

void setRightMotorSpeed(int speed) {
  analogWrite(RIGHT_MOTOR_ENABLE, abs(speed));
  digitalWrite(RIGHT_MOTOR_IN1, speed > 0 ? HIGH : LOW);
  digitalWrite(RIGHT_MOTOR_IN2, speed > 0 ? LOW : HIGH);
}

void setSteering(int angle) {
  steeringServo.write(constrain(angle, MAX_LEFT_ANGLE, MAX_RIGHT_ANGLE));
}

void moveForward(int speed) {
  setLeftMotorSpeed(speed);
  setRightMotorSpeed(speed);
}

void moveBackward(int speed) {
  setLeftMotorSpeed(-speed);
  setRightMotorSpeed(-speed);
}

void turnLeft(int speed, int angle) {
  setLeftMotorSpeed(speed / 2);
  setRightMotorSpeed(speed);
  setSteering(CENTER_ANGLE - angle);
}

void turnRight(int speed, int angle) {
  setLeftMotorSpeed(speed);
  setRightMotorSpeed(speed / 2);
  setSteering(CENTER_ANGLE + angle);
}

void stop() {
  setLeftMotorSpeed(STOP_SPEED);
  setRightMotorSpeed(STOP_SPEED);
  setSteering(CENTER_ANGLE);
}

void executeManeuver(Anomaly* detectedAnomaly) {
  float anomalyWidth = detectedAnomaly->width;
  float anomalyHeight = detectedAnomaly->height;
  float anomalyOffset = detectedAnomaly->offset;
  
  // Fuzzy logic to determine maneuver
  if (detectedAnomaly->type == POTHOLE) {
    if (anomalyWidth < 0.5 * roadWidth && abs(anomalyOffset) < 0.25 * roadWidth) {
      // Small pothole, try to straddle it
      moveForward(NORMAL_SPEED);
    } else if (anomalyOffset > 0) {
      // Pothole on the right, move left
      turnLeft(SLOW_SPEED, 15);
    } else {
      // Pothole on the left or center, move right
      turnRight(SLOW_SPEED, 15);
    }
  } else if (detectedAnomaly->type == SPEEDBUMP) {
    // Slow down for speed bumps
    moveForward(SLOW_SPEED);
  } else if (detectedAnomaly->type == COLLISION || detectedAnomaly->type == ACCIDENT_SCENE) {
    // Stop for collisions or accident scenes
    stop();
  } else if (detectedAnomaly->type == CRACK) {
    if (anomalyWidth < 0.25 * roadWidth) {
      // Small crack, proceed with caution
      moveForward(NORMAL_SPEED);
    } else {
      // Larger crack, slow down
      moveForward(SLOW_SPEED);
    }
  } else if (detectedAnomaly->type == MANHOLE) {
    if (abs(anomalyOffset) < 0.2 * roadWidth) {
      // Manhole in the center, straddle it
      moveForward(NORMAL_SPEED);
    } else if (anomalyOffset > 0) {
      // Manhole on the right, move slightly left
      turnLeft(NORMAL_SPEED, 10);
    } else {
      // Manhole on the left, move slightly right
      turnRight(NORMAL_SPEED, 10);
    }
  }
  
  // Execute the maneuver for a short duration
  delay(1000);
  
  // Return to normal driving
  moveForward(NORMAL_SPEED);
  setSteering(CENTER_ANGLE);
}


float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  // Haversine formula to calculate distance between two GPS coordinates
   float dLat = (lat2 - lat1) *
                      M_PI / 180.0;
        float dLon = (lon2 - lon1) * 
                      M_PI / 180.0;
 
        // convert to radians
        lat1 = (lat1) * M_PI / 180.0;
        lat2 = (lat2) * M_PI / 180.0;
 
        // apply formulae
        float a = pow(sin(dLat / 2), 2) + 
                   pow(sin(dLon / 2), 2) * 
                   cos(lat1) * cos(lat2);
        float rad = 6371;
        float c = 2 * asin(sqrt(a));
        return rad * c;
}