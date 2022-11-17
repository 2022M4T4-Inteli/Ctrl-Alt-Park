// Including librarys
#include "WiFi.h"

// Defining constants representing network name and password
const char * WIFI_FTM_SSID = "Grupo5";
const char * WIFI_FTM_PASS = "Grupo5-123";

void setup() {
  Serial.begin(115200);
  Serial.println("Starting SoftAP with FTM Responder support");

  // Starting wifi
  WiFi.softAP(WIFI_FTM_SSID, WIFI_FTM_PASS, 1, 0, 4, true);
}

void loop() {
  delay(1000);
}