#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Updater.h>

/*
 * au(host, port, build_name)
 * it connects to host at port, and send an HTTP/1.1 request
 * it sends an header with the current version of the software
 * the server might reply with 304 if the client is up to date
 * or send the new .bin file, which will be applied and restart
 */

int au(const char *host, int port, const char *build_name) {
  char tmp[1024];
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ERR] no wifi");
    return -1;
  }

  Serial.print("[INFO] autoupdate ");
  Serial.print(host);
  Serial.print(":");
  Serial.print(port);
  Serial.print(" - name: ");
  Serial.println(build_name);

  WiFiClient client;
  if (!client.connect(host, port)){
    Serial.println("[ERR] can't connect");
    return -2;
  }

  client.print(String("GET /autoupdate/")+build_name+" HTTP/1.1\r\n"+
    "Host: "+host+"\r\n"+
    "x-AU: %VER%1522157740%\r\n"+
    "Connection: close\r\n\r\n");

  String cmdline = client.readStringUntil('\n');
  Serial.print("[INFO] cmdline: "); Serial.println(cmdline);
  cmdline = cmdline.substring(cmdline.indexOf(" ")+1);

  if (cmdline.startsWith("304")) {
    Serial.println("[INFO] nothing to update");
    client.stop();
    return 0;
  }
  if (!cmdline.startsWith("200")) {
    Serial.println("[ERR] Server error");
    return -4;
  }

  int clen = 0;
  while (client.available()) {
    String header = client.readStringUntil('\n');
    header.trim();
    if (!header.length()) break;  
    Serial.print("[INFO] "); Serial.println(header);
    if (header.startsWith("Content-Length:")) {
      int i = header.indexOf(":");
      String v = header.substring(i+1);
      v.trim();
      Serial.print("[INFO] update content-length: ");
      Serial.println(v);
      clen = atoi(v.c_str());
    }
  }

  if (!clen) {
    Serial.println("[ERR] no content length");
    client.stop();
    return -8;
  }
  if (!Update.begin(clen)) {
    Serial.println("[ERR] can't update");
    client.stop();
    return -16;
  }

  Serial.println("[INFO] flashing .bin file");
  size_t written = Update.writeStream(client);
  Serial.print("update stored "); Serial.print(written); Serial.println(" bytes");

  if (!Update.end()) {
    Serial.println("[ERR] update failed, no space?");
    Update.printError(Serial);
    client.stop();
    ESP.restart();
    return -17;
  }

  if (!Update.isFinished()) {
    Serial.println("[ERR] Update not finished, something went wrong");
    client.stop();
    ESP.restart();
    return -18;
  }

  Serial.println("Update successfully completed. Rebooting...");
  Serial.println();
  ESP.restart();
  return 1;
}


