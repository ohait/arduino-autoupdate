#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Updater.h>

#include "autoupdate.h"

const char* ssid = "your ssid";
const char* password = "your password";
const char* au_host = "your.host";
const int au_port = 80;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("INIT ");
  Serial.print(__FILE__);
  Serial.print(" ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  delay(100);
  // check if there is a new img
  au(au_host, au_port, "test");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5);
}
