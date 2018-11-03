unsigned int stationID = 1;
unsigned int packetID = 0;

#include "zeiger.h"
#include "myneopixel.h"

#include "MySensors.h"
MySensors mySensors;

// LoRa Stuff, see also (I think) https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/blob/master/esp32/libraries/LoRa/API.md
#include <LoRa.h>
#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6 // 433E6  //915E6
int spreadingFactor = 9;  // (7-12)
float bandwidth = 31.25E3; // {7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3};
#include <SPI.h>

void loraSetParams() {
  Serial.printf("Setting spread to %d and bandwith to %f\r\n", spreadingFactor,bandwidth);
  
  LoRa.setSpreadingFactor(spreadingFactor);
  // `spreadingFactor` - spreading factor, defaults to `7`
  // Supported values are between `6` and `12`. If a spreading factor of `6` is set, implicit header mode must be used to transmit and receive packets.

  LoRa.setSignalBandwidth(bandwidth);
  // signalBandwidth` - signal bandwidth in Hz, defaults to `125E3`.
  // Supported values are `7.8E3`, `10.4E3`, `15.6E3`, `20.8E3`, `31.25E3`, `41.7E3`, `62.5E3`, `125E3`, and `250E3`.
  
}


void loraSetup() {
  Serial.println("Setting up LoRa");
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);

  loraSetParams();
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Init OK!");

}

void calibrate() {
  for (int i=0; i<40; i++) {
    mySensors.measure();
    Serial.printf("%d of 40\r\n", i+1);
    Serial.printf("Status=%d, CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
      mySensors.status, mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);
      delay(30000);
  }
  mySensors.setZeroPoint();
  while(true) {
    mySensors.measure();
    Serial.printf("Status=%d, CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
      mySensors.status, mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);
      delay(5000);
  }  
}



void setup() {
  Serial.begin(115200);
  Serial.print("Setting up Sensors... ");
  mySensors.begin();
  Serial.println(" done.");
  //calibrate();
  Serial.println("Initializing LoRa");
  loraSetup();
  zeiger_setup();
  //neopixel_setup();
}

unsigned char packetBuffer[255];
unsigned char packetLength=0;

void writeByte(unsigned char b) {
  if (packetLength<254) {
    packetBuffer[packetLength++]=b;
  } else {
    Serial.printf("WARNING! Packet too long (>254), truncating!\r\n");
  }
}

void writeInt(unsigned int i) {
  writeByte((byte)(i & 0xff));
  writeByte((byte)(i>>8));
}

void addCheckSum() {
  if (packetLength<255) {
    unsigned char xorsum = 0;
    for (int i=0; i<packetLength; i++) {
      xorsum ^= packetBuffer[i];
    }
    packetBuffer[packetLength++]=xorsum;
  }
}

// Zehntel Grad, Nullpunkt bei -273
unsigned int tempToInt(float f) {
  return (unsigned int)(2730.0+10.0*f);
}

unsigned int humidToInt(float f) {
  return (unsigned int)(f*100);
}

void makePacket() {
  packetLength=0;
  writeInt(stationID);
  writeInt(packetID++);
  if (mySensors.status) {
    Serial.println("Building packet...");
    writeInt((unsigned int)(mySensors.ppmCO2));
    writeInt((unsigned int)(mySensors.approxppmCO2));
    writeInt((unsigned int)(mySensors.ppbTVOC));
    writeInt(tempToInt(mySensors.temp[0]));
    writeInt(tempToInt(mySensors.temp[1]));
    writeInt(tempToInt(mySensors.temp[2]));
    writeInt(humidToInt(mySensors.humidity));
    addCheckSum();
    Serial.printf("Packet complete, with length=%d\r\n",packetLength);
  }
}

void sendPacket() {
  if (packetLength>0) {
    long airTime = millis();
    Serial.println("About to send packet");
    LoRa.beginPacket();
    LoRa.write(packetBuffer, packetLength);
    LoRa.endPacket();
    airTime = millis()-airTime;
    Serial.printf("Sent %d bytes in %d ms.\r\n  ", packetLength, airTime);
    for (int i=0; i<packetLength; i++) {
      Serial.printf("%02x", packetBuffer[i]);
    }
    Serial.println();
  } else {
    Serial.println("Cowardly refusing sending a packet of length 0");
  }
}

int counter=0;



void loop() {
  Serial.println("Measuring...");
  int targetUS=-1;
  mySensors.measure();
  if (mySensors.status) {
    targetUS=getServoUS(mySensors.ppmCO2);
    Serial.printf("Target Servo: %d us\r\n",targetUS);
    Serial.printf("CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
      mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);    
    if (counter>5) {
      mySensors.compute();
      Serial.printf("[mean] CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
        mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);      
      counter = 0;
      makePacket();
      sendPacket();
    }
  } else {
    Serial.println("Not ready yet");
    Serial.printf("CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
      mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);
  }
  if (targetUS!=-1) {
    int diff =  targetUS - currentUS;
    int cur = currentUS;
    for (int i=0; i<250; i++) {
      currentUS = cur + diff*i/250;
      setServo();
      delay(19);
      //neopixel_update(mySensors.ppmCO2, i);
    }
  } else {
    //neopixelAnimate(5000);
    delay(5000);
  }
  counter++;
}
