#include "helper.h"
// #include "icaps2_fis.h"
// #include "icaps2_fis.c"
#include "TinyGPS++.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

Queue* anomalies = {0};

LatLng currentLocation;


QNode* collectAnomalies(){
  return nullptr;
}

void addAnomalies(QNode *nodes, uint8_t size){
  
}

void getCurrentLocation(){
  currentLocation = {9.0f, 35.2f};
}

void followRoute(float skew, float speed){
  Serial.printf("Moving with speed of %A and a skew of %A", speed, skew);
}

void hasLeftAnomaly(QNode *anomaly, long startTime){
  delay(500);
}
int distanceFromHead(LatLng pos){
  return 2;
}
void setup(){
  anomalies = createQueue(); 
  QNode* a = collectAnomalies();
  icaps2_init();
  addAnomalies(a,0);
}
 #define USE_SERIAL Serial
 
void loop(){
  getCurrentLocation();
  if(currentLocation > anomalies->Head->data->position){
    dequeue(anomalies);
  }

  if(distanceFromHead(currentLocation) < 2){ // if the distance from the current location and the closest anomaly is less than 2 metres 
    long long int startTime = 12312324324;
    icaps2_run(anomalies->Head->data, followRoute);//carry out fuzzy suggestion
    hasLeftAnomaly(anomalies->Head, startTime); //leave the function after anomaly has been left
  }
  followRoute(BLANK_SKEW,DEFAULT_MOTOR_SPEED);
}