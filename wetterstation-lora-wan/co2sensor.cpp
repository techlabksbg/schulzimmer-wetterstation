#include "co2sensor.h"

#include <HardwareSerial.h>
HardwareSerial co2Serial(2);


CO2Sensor::CO2Sensor() {
  
}

void CO2Sensor::begin() {
  begin(13,12);
}

void CO2Sensor::begin(int co2rx, int co2tx) {
  co2Serial.begin(9600, SERIAL_8N1, co2rx, co2tx);  // RX, TX
  enableABC();
}


void CO2Sensor::measure() {
  status = 0;
  ppmCO2 = readCO2UART();
  if (ppmCO2!=0 && ppmCO2!=410 && status) {
    status = 1;
  } else {
    status = 0;
  }
  if (status==1) {
    co2s+=ppmCO2;
    n++;
  } else {
    clearSums();
  }
}

void CO2Sensor::clearSums() {
  co2s=0.0;
  n = 0;
}


void CO2Sensor::compute() {
  if (n>0) {
    ppmCO2 = (unsigned int) (co2s/n);
    status = 1;
  } 
}


void CO2Sensor::clearSerialBuffer() {
  while (co2Serial.available()>0) {
    co2Serial.read();
    delay(2);
  }
}

// The following code is derived from another source, 
// I cannot trace back any more :-(


unsigned char CO2Sensor::getCheckSum(unsigned char *packet) {
  unsigned char checksum = 0;
  for (unsigned char i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  return (255-checksum)+1;
}


unsigned int CO2Sensor::readCO2UART(){
  unsigned char cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
  unsigned char response[9]; // for answer
  unsigned int  ppm=0;
  status = 1;
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
      temp = response[4] - 40;
    }
  }
  clearSerialBuffer();
  if (ppm==0) status=0;
  return ppm;
}

// Enables auto-calibration (every 24h the lowest co2-value is calibrated to 410 (or something like that))
void CO2Sensor::enableABC() {
  unsigned char cmd[9] = {0xFF,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86};
  clearSerialBuffer();
  co2Serial.write(cmd, 9); //send command
  Serial.println("ABC enabled");  
}




