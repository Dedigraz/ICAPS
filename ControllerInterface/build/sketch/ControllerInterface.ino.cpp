#include <Arduino.h>
#line 1 "C:\\Users\\Ebun\\source\\ICAPS\\ControllerInterface\\ControllerInterface.ino"
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

struct Anomaly {
  float lat;
  float lng;
  float width;
  float height;
  enum AnomalyType type;
};
#line 21 "C:\\Users\\Ebun\\source\\ICAPS\\ControllerInterface\\ControllerInterface.ino"
void setup();
#line 27 "C:\\Users\\Ebun\\source\\ICAPS\\ControllerInterface\\ControllerInterface.ino"
void loop();
#line 21 "C:\\Users\\Ebun\\source\\ICAPS\\ControllerInterface\\ControllerInterface.ino"
void setup(){
    Serial.begin(115200);
    pinMode(1, OUTPUT);
    
}

void loop(){
    digitalWrite(1, HIGH);
    Serial.println("Writing");
  Serial.write("Hello");
}                                                                                                                                                                                                                                                                                                                                                
