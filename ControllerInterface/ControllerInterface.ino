// #include "SoftwareSerial.h"
#define NumOfAnomalies 12

enum AnomalyType{
  POTHOLE,
  SPEEDBUMP,
  COLLISION,
  ACCIDENT_SCENE
};

typedef struct {
  float lat;
  float lng;
  float width;
  float height;
  enum AnomalyType type;
} Anomaly;

// struct Anomaly anomalies[12];
Anomaly* anomalies;

void populateAnomalies(){
  anomalies = (Anomaly*) malloc(sizeof(Anomaly) * NumOfAnomalies);
  anomalies[1] =  { 37.7749, -122.4194, 0.0001, 0.0001, POTHOLE };
}

void setup(){
    Serial.begin(115200);
    pinMode(1, OUTPUT);
    populateAnomalies();
    
}

void loop(){
    digitalWrite(1, HIGH);
    Serial.println("Writing");
  Serial.write("Hello");
}                                                                                                                                                                                                                                                                                                                                                