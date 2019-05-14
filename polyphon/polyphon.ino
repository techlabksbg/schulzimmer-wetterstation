#define PIN 26

#include "mario.h"

unsigned long periods[128];

void computePeriods() {
  periods[0]=0;
  for (int i=1; i<128; i++) {
    periods[i] = (unsigned long)(1000000.0/(440.0*pow(2,(i-69)/12.0)));
  }
}

// pulsed
void playTune() {
  const int PULSE = 30;
  unsigned long tnow = millis();
  unsigned long usnow = micros();
  unsigned long notes[POLY];
  unsigned long pulse[POLY];
  unsigned int evPtr = 0;
  for (int c=0; c<POLY; c++) {
    notes[c]=0;
  }
  while (evPtr<numEv) {
    // Wait for tone change event
    while (millis()-tnow < (unsigned long)(evTime[evPtr])) {
      // Make sound on Channels
      for (int c=0; c<POLY; c++) {
        if (notes[c]!=0) {
          usnow=micros();
          unsigned long d = usnow-pulse[c];
          if (d>=notes[c]) {
            if (d-notes[c]<PULSE) {          
              digitalWrite(PIN, HIGH);
              delayMicroseconds(30);
              digitalWrite(PIN, LOW);
            }
            pulse[c] = usnow;
          }
        }
      }
    }
    // midi 69 -> 440 Hz
    notes[evChannels[evPtr]] = periods[evNotes[evPtr]];
    pulse[evChannels[evPtr]] = micros();
    evPtr++;
  }
  digitalWrite(PIN, LOW);
}

// triangle
void playTuneTriangle() {
  const int PULSE = 30;
  unsigned long tnow = millis();
  unsigned long usnow = micros();
  unsigned long notes[POLY];
  unsigned long pulse[POLY];
  unsigned int evPtr = 0;
  for (int c=0; c<POLY; c++) {
    notes[c]=0;
  }
  while (evPtr<numEv) {
    // Wait for tone change event
    while (millis()-tnow < (unsigned long)(evTime[evPtr])) {
      // Make sound on Channels
      int val = 0;
      usnow=micros();
      for (int c=0; c<POLY; c++) {
        if (notes[c]!=0) {
          val += 60*(usnow % notes[c])/notes[c];
        }
      }
      if (val>255) {
        val=255;
      }
      dacWrite(PIN,val);
    }
    
    notes[evChannels[evPtr]] = periods[evNotes[evPtr]];
    pulse[evChannels[evPtr]] = micros();
    evPtr++;
  }
  digitalWrite(PIN, LOW);
}

void setup() {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,LOW);
  computePeriods();
}

void loop() {
  // put your main code here, to run repeatedly:
  playTuneTriangle();
  delay(5000);
}
