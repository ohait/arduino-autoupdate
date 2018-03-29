#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

#include "autoupdate.h"

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PWD";
const char* mqtt_server = "MQTT_SERVER";

void autoupdate() {
  au("AUTOUPDATE_HOST", 80, "mqtt");
}

WiFiClient client;
PubSubClient mqtt(client);

String mqtt_name;

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
  autoupdate();

  mqtt_name = String("ESP8266-")+ESP.getChipId();
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(mqtt_recv);
  mqtt_connect();
}

long mqtt_wait = 0;
void mqtt_connect() {
  if (millis()<mqtt_wait) return;

  if (!mqtt.connect(mqtt_name.c_str())) {
    mqtt_wait = millis()+5000;
    return;
  }
  mqtt_wait = 0;
  Serial.print("connected to mqtt: ");
  Serial.println(mqtt_server);

  mqtt.publish("MyHome/sys/boot", (String()+"name: "+mqtt_name+", ip: "+WiFi.localIP().toString()+", name: "+__FILE__).c_str()); 

  mqtt.subscribe("MyHome/sys/update");
}

void mqtt_recv(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, "MyHome/sys/update")==0) {
        Serial.println("checking for firmware update...");
        autoupdate();
        return;
    }
    Serial.print("ignored mqtt topic: '"); Serial.print(topic); Serial.println("'");
}

void loop() {
    if (!mqtt.connected()) mqtt_connect();
    else mqtt.loop();
    // put your main code here, to run repeatedly:
    delay(5);
}
