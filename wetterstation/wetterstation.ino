#include "MySensors.h"

MySensors mySensors;

void setup() {
  Serial.begin(115200);
  mySensors.begin();
  Serial.println("Init done.");
}

void loop() {
  Serial.println("Measuring...");
  mySensors.measure();
  if (mySensors.status) {
    Serial.printf("CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
      mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);
  } else {
    Serial.println("Not ready yet");
    Serial.printf("CO2=%d ppm (approx %d), TVOC=%d bpm, Temp0=%f C, Temp1=%f C, Temp2=%f C, RelHum=%f %%\r\n",
      mySensors.ppmCO2, mySensors.approxppmCO2, mySensors.ppbTVOC, mySensors.temp[0], mySensors.temp[1], mySensors.temp[2], mySensors.humidity);
  }
  delay(5000);
}
