#include "MySensors.h"

#include <HardwareSerial.h>
HardwareSerial co2Serial(2);

#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;

#include <Wire.h>
#include "ClosedCube_HDC1080.h"
ClosedCube_HDC1080 hdc1080;


MySensors::MySensors() {
  
}

void MySensors::begin() {
  begin(13,12, 21,22);
}

void MySensors::begin(int co2rx, int co2tx, int sda, int scl) {
  co2Serial.begin(9600, SERIAL_8N1, co2rx, co2tx);  // RX, TX
  hdc1080.begin(0x40);
  while (!ccs.begin()) {
    Serial.println("VOC Sensor Init failed");
    ccs.setTempOffset(-65.0);
    delay(2000);
  }
  enableABC();
}


void MySensors::measure() {
  status = 0;
  hdc1080.setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  hdc1080.readRegister();
  temp[0] = hdc1080.readTemperature();
  humidity = hdc1080.readHumidity();
  if(ccs.available() && !ccs.readData()){
    temp[1] = (ccs.calculateTemperature()-32.0-25.0)/1.8;
    ppbTVOC = ccs.getTVOC();
    approxppmCO2 = ccs.geteCO2();
    status = 1;
  } else {
    ppbTVOC=0xffff;
    status = 0;
  }
  ppmCO2 = readCO2UART();
  if (ppmCO2!=0 && ppmCO2!=410 && ppbTVOC!=0xffff && status) {
    status = 1;
  } else {
    status = 0;
  }
  if (status==1) {
    for (int i=0; i<3; i++) temps[i]+=temp[i];
    humids+=humidity;
    co2s+=ppmCO2;
    aco2s+=approxppmCO2;
    tvocs+=ppbTVOC;
    n++;
  } else {
    clearSums();
  }
}

void MySensors::clearSums() {
  for (int i=0; i<3; i++) temps[i]=0.0;
  humids=0.0;
  co2s=0.0;
  aco2s=0.0;
  tvocs=0.0;
  n = 0;
}


void MySensors::compute() {
  if (n>0) {
    humidity = humids/n;
    for (int i=0; i<3; i++) temp[i]=temps[i]/n;
    ppmCO2 = (unsigned int) (co2s/n);
    approxppmCO2 = (unsigned int) (aco2s/n);
    ppbTVOC = (unsigned int)(tvocs/n);
    clearSums();
    status = 1;
  } 
}


void MySensors::clearSerialBuffer() {
  while (co2Serial.available()>0) {
    co2Serial.read();
    delay(2);
  }
}

// The following code is derived from another source, 
// I cannot trace back any more :-(


unsigned char MySensors::getCheckSum(unsigned char *packet) {
  unsigned char checksum = 0;
  for (unsigned char i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  return (255-checksum)+1;
}


unsigned int MySensors::readCO2UART(){
  unsigned char cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
  unsigned char response[9]; // for answer
  unsigned int  ppm=0;

  clearSerialBuffer();
  co2Serial.write(cmd, 9); //request measure
  
  memset(response, 0, 9); // wait a second...
  for (int i=0; co2Serial.available() == 0 && i<50; i++) { 
    delay(20);
  }
  if (co2Serial.available() > 0) {
    co2Serial.readBytes(response, 9);
    if (response[8] == getCheckSum(response) && response[5]==0) { // Checksum and status
      ppm = ((unsigned int)response[2])<<8 | response[3];
      temp[2] = response[4] - 40;
    }
  }
  clearSerialBuffer();
  if (ppm==0) status=0;
  return ppm;
}

// Enables auto-calibration (every 24h the lowest co2-value is calibrated to 410 (or something like that))
void MySensors::enableABC() {
  unsigned char cmd[9] = {0xFF,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86};
  clearSerialBuffer();
  co2Serial.write(cmd, 9); //send command
  Serial.println("ABC enabled");  
}

// Calibrates the current value to 410 ppm (or something like that)
// Better use manual anyway
void MySensors::setZeroPoint(){
  unsigned char cmd[9] = {0xFF,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78};
  clearSerialBuffer();
  co2Serial.write(cmd, 9); //send command
  Serial.println("Wrote setZeroPoint command");
}


