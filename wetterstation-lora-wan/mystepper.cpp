#include "mystepper.h"
#include "Arduino.h"

#include <NeoPixelBus.h>

// See https://github.com/Makuna/NeoPixelBus/wiki

const uint8_t PixelPin = 25;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> mystrip(1, PixelPin);
NeoGamma<NeoGammaTableMethod> colorGamma;

RgbColor mycolor(0,0,0);



MyStepper::MyStepper() {
  for (int i=0; i<4; i++) {
    pinMode(motorpins[i],OUTPUT);
    digitalWrite(motorpins[i],LOW);
  }
  pinMode(limitpin, INPUT);
}

void MyStepper::setPixel(int ppm) {
  int i=0;
  while (ppm>=colormap[i+1][0]) {
    i++;
  }
  //Serial.printf("ppm=%d, i=%d ", ppm,i);
  float t = ((float)(ppm-colormap[i][0]))/(colormap[i+1][0]-colormap[i][0]);
  uint8_t rgb[3];
  for (int c=0; c<3; c++) {
    rgb[c] = (uint8_t)((t*colormap[i+1][c+1]+(1-t)*colormap[i][c+1]));
  }
  mycolor.R=rgb[0];
  mycolor.G=rgb[1];
  mycolor.B=rgb[2];
  //Serial.printf("r=%d, g=%d, b=%d, ppm=%d\n",rgb[0], rgb[1], rgb[2],ppm);
  mystrip.SetPixelColor(0, mycolor);
  mystrip.Show();
}

void MyStepper::makestep() {
  for (int i=0; i<4; i++) {
    digitalWrite(motorpins[i],i==(motorstep/2) || (motorstep%2==1 && i==(motorstep/2+1)%4));
  }
  double w=motorposition*1.0/fullpos;
  int ppm = (int)((0.4*w*w+(0.6)*w)*4600+400);
  setPixel(ppm);
}

void MyStepper::off() {
  for (int i=0; i<4; i++) {
    digitalWrite(motorpins[i],LOW);
  }
}

void MyStepper::forward() {
  motorposition++;
  motorstep=(motorstep+1)%8;
  makestep();
}

void MyStepper::backward() {
  motorposition--;
  motorstep=(motorstep+7)%8;
  makestep();
}

void MyStepper::gotoPPM(int ppm) {
  double m = (ppm-400.0)/(5000.0-400.0);
  double w = (sqrt(1288.0*m+81.0)-9.0)/28.0;
  gotoPos((int)(w*fullpos));
  setPixel(ppm);
}

void MyStepper::gotoPos(int pos) {
  Serial.printf("gotoPos(%d)\n",pos);
  if (abs(pos-motorposition)>20) {
    makestep();
    delay(20);
    int w=20;
    while (motorposition!=pos) {
      if (motorposition<pos) {
        forward();
      } else {
        backward();
      }
      if (abs(pos-motorposition)<15) {
        w = 20-abs(pos-motorposition);
      } else if (w>5) {
        w--;
      }
      delay(w);
      
    }
    delay(20);
    off();
  }
  off();
}

void MyStepper::demo() {
  for (int ppm=500; ppm<=3000; ppm+=500) {
    Serial.printf("Going to ppm=%d\n",ppm);
    gotoPPM(ppm);
    delay(1000);
    if (ppm>1500) ppm+=500;
  }
  gotoPPM(400);
}

void MyStepper::home() {
  mystrip.Begin();
  mystrip.SetPixelColor(0,colorGamma.Correct(RgbColor(0,0,255)));
  mystrip.Show();

  Serial.printf("Start home at pin=%d\n",digitalRead(limitpin));
  int failsave = fullpos;
  while (digitalRead(limitpin)==HIGH && failsave>0) {
    backward();
    failsave--;
    delay(5);
  }
  delay(500);
  Serial.printf("First home at pin=%d\n",digitalRead(limitpin));
  while (digitalRead(limitpin)==LOW) {
    forward();
    delay(10);
  }
  delay(200);
  Serial.printf("Second home at pin=%d\n",digitalRead(limitpin));
  failsave = 50;
  while (digitalRead(limitpin)==HIGH && failsave>0) {
    backward();
    failsave--;
    delay(40);
  }
  while (digitalRead(limitpin)==LOW) {
    forward();
    delay(80);
  }
  //Serial.printf("Last home at pin=%d\n",digitalRead(limitpin));
  motorposition=zeropos;
  off();  
  delay(100);
}
  
