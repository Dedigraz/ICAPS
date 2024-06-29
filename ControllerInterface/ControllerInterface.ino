// #include "SoftwareSerial.h"
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

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
} Anomaly;
const Anomaly anomalies[12];

void populateAnomalies(){
  
}

void setup(){
    Serial.begin(115200);
    pinMode(1, OUTPUT);
    
}

void loop(){
    digitalWrite(1, HIGH);
    Serial.println("Writing");
  Serial.write("Hello");
}                                                                                                                                                                                                                                                                                                                                                