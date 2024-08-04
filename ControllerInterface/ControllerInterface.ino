// #include "SoftwareSerial.h"

#include <Ramp.h>
#ifndef GPSAvailable

#define GPSAvailable 1
#include <TinyGPS.h>
TinyGPS gps;

#endif
#define UART1 21
#define UART2 22 

#define NumOfAnomalies 22

#define ft 0.293

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
    traversalTime  = 1000.* (float)abs(waypoints[cursor-1].lng.getValue() - waypoints[cursor].lng.getValue());
  }

  Serial.println("Moving would take" );
  Serial.println(traversalTime);
  gpsLocation.lat.go( waypoints[cursor].lat.getValue(),traversalTime, CUBIC_IN);
  gpsLocation.lng.go( waypoints[cursor].lng.getValue(),traversalTime, CUBIC_IN);
  cursor+=1;
}



void setup(){
  //     Serial.begin(0);  // it will try to detect the baud rate for 20 seconds

  // Serial.print("\n==>The baud rate is ");
  //   if (Serial.baudRate() == 0) {
  // Serial.end();
  // Serial.println(Serial.baudRate());
    Serial.begin(9600);
  // Serial.println(Serial.baudRate());
  //   }
    populateAnomalies();

    pinMode(1, OUTPUT);

    pinMode(UART1, OUTPUT);
    pinMode(UART2, OUTPUT);
    
    #ifdef GPSAvailable
      // initialize GPS
      // pinMode(GpsPIN, INPUT);
    #endif
    updateGpsLocation(true);
}

void loop(){
  if (gpsLocation.lng.isFinished()) {
    updateGpsLocation();
  }
  float lat = gpsLocation.lat.update();

  digitalWrite(1, HIGH);
  delay(500);
  digitalWrite(1,LOW);


  
  Serial.println("Writing");
  Serial.write("Hello");
  Serial.readString();
  
}                                                                                                                                                                                                                                                                                                                                                