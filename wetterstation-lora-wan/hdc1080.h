#include <Wire.h>
#include "ClosedCube_HDC1080.h"
ClosedCube_HDC1080 hdc1080;

void hdc1080_begin() {
  //Wire.begin(21,22);
  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX);
}

double hdc1080_temp;
double hdc1080_humidity;

void hdc1080_measure() {
  hdc1080.setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  hdc1080.readRegister();
  hdc1080_temp = hdc1080.readTemperature();
  hdc1080_humidity = hdc1080.readHumidity();
}

