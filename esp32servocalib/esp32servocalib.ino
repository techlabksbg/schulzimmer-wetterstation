#include <ESP32Servo.h>
Servo myservo;  // create servo object to control a servo
#define SERVOPIN 17
float servomap[][2]={{400,2200},{1500,1200},{5000,600}};


int getServoUS(int ppm) {
  if (ppm < servomap[0][0]) {
    return servomap[0][1];
  }
  if (ppm > servomap[2][0]) {
    return servomap[2][1];
  }
  int us = 0;
  int i = 0;
  if (ppm>=servomap[1][0]) {
    i=1;
  }
  us = (int)(servomap[i][1]+(servomap[i+1][1]-servomap[i][1])*(ppm-servomap[i][0])/(servomap[i+1][0]-servomap[i][0]));
  return us;
}

void setServo(int us) {
  myservo.writeMicroseconds(us);
}


void setup() {
  // put your setup code here, to run once:
  myservo.attach(SERVOPIN);
  Serial.begin(115200);
}

void loop() {
  int d = 4000;
  for (int i=400; i<=1500; i+=100) {
    Serial.println(i);
    setServo(getServoUS(i)); 
    delay(d);
  }
  for (int i=2000; i<=5000; i+=500) {
    Serial.println(i);
    setServo(getServoUS(i)); 
    delay(d);
  }
  for (int i=5000; i>=1500; i-=500) {
    Serial.println(i);
    setServo(getServoUS(i)); 
    delay(d);
  }
  for (int i=1400; i>=400; i-=100) {
    Serial.println(i);
    setServo(getServoUS(i)); 
    delay(d);
  }
}
