#include "SoftwareSerial.h"

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.println("Reading");
  char* x = Serial.readString();
  Serial.println(x);
}

void speedUp()
{

}

void speedDown()
{

}


void forward()
{

}
void backward()
{

}
void left()
{

}
void right()
{

}
void stop()
{

}
