# 1 "C:\\Users\\Ebun\\source\\ICAPS\\ControllerInterface\\ControllerInterface.ino"
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
void setup(){
    Serial.begin(115200);
    pinMode(1, 0x1);

}

void loop(){
    digitalWrite(1, 0x1);
    Serial.println("Writing");
  Serial.write("Hello");
}
