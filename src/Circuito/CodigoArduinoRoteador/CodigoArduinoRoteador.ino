// Including librarys
#include "WiFi.h"

// Defining constants representing network name and password
// Define the slave number
const char * WIFI_FTM_SSID = "Slave_2_A1";
const char * WIFI_FTM_PASS = "RX6]F^SEIHu[4HYj";

void setup() {
  Serial.begin(115200);
  Serial.println("Starting SoftAP with FTM Responder support");

  // Starting wifi
  WiFi.softAP(WIFI_FTM_SSID, WIFI_FTM_PASS, 1, 0, 4, true);
}

void loop() {
  delay(1000);
}