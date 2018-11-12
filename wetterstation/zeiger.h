#include <ESP32Servo.h>
Servo myservo;  // create servo object to control a servo
#define SERVOPIN 17
float servomap[][2]={{400,2235},{1500,1230},{5000,600}};

int currentUS;



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

void setServo() {
  myservo.writeMicroseconds(currentUS);
}


void zeiger_setup() {
  myservo.attach(SERVOPIN);
  currentUS = servomap[0][1];
  setServo();
}
