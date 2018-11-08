#include<Arduino.h>

// LoRa Stuff, see also (I think) https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/blob/master/esp32/libraries/LoRa/API.md
#include <LoRa.h>
#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6 // 433E6  //915E6 
int spreadingFactor = 9;  // (7-12)
float bandwidth = 31.25E3; // {7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3};
#include <SPI.h>

#include <WiFi.h>
const char *ssid = "InfLabKSBG";
const char *url = "http://192.168.1.20:4568/loraconcentrator/packetin/";
char buffer[600];
#include <HTTPClient.h>


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

// Cowardly copied from https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
void wifiScan() {
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing LoRa");
  loraSetup();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  wifiScan();
  delay(1000);
  Serial.printf("Connecting to %s...\r\n", ssid);
  WiFi.begin(ssid, "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("OK, waiting for LoRa packets...");
}

unsigned char packetBuffer[255];
unsigned char packetLength=0;

void loop() {
  int conf=0;
  int packetLength = LoRa.parsePacket();
  if (packetLength) {
    for (int i=0; LoRa.available(); i++) {
      if (i<sizeof(packetBuffer)) {
        packetBuffer[i]=(unsigned char)LoRa.read();
      } else {
        packetLength=0;
      }
    }
    if (packetLength) {
      int rssi = LoRa.packetRssi();
      Serial.printf("pkt len=%d, rssi=%d\r\n-->", packetLength, rssi);
      strcpy(buffer,url);
      int l = strlen(buffer);
      for (int i=0; i<packetLength; i++) {
        Serial.printf("%02x", packetBuffer[i]);
        sprintf(buffer+l+2*i, "%02x",packetBuffer[i]);
      }
      sprintf(buffer+l+2*packetLength, "?rssi=%d", rssi);
      Serial.println();
      Serial.println(buffer);
      HTTPClient http;
 
      http.begin(buffer); 
      int httpCode = http.GET();
      Serial.printf("HTTP-Code: %d\r\n", httpCode);
    }
  }
}
