#include <ESP32Servo.h>
Servo myservo;  // create servo object to control a servo
#define SERVOPIN 17
float servomap[][2]={{400,2250},{1500,1250},{5000,650}};

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
  Serial.printf("Set servo to %d\n", currentUS);
}

void myservo_attach() {
  Serial.printf("ATTACHED at %d\n",currentUS);
  myservo.writeMicroseconds(currentUS);
  myservo.attach(SERVOPIN);
  myservo.writeMicroseconds(currentUS);
}

void myservo_detach() {
  myservo.detach();
  Serial.println("DETACHED");
}



void zeiger_setup() {
  myservo_attach();
  currentUS = servomap[0][1];
  setServo();
  delay(500);
  myservo_detach();
}
