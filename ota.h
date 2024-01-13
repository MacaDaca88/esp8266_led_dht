#ifndef OTA_H
#define OTA_H


const char* ssid_ota = "Tip-jar";
const char* password_ota = "PASSWORD1234LOL";

void OTAinit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_ota, password_ota);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname("Wifi Leds");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setDrawColor(1);
      u8g2.setCursor(20, 20);
      u8g2.printf("Progress: %u%%\r", (progress / (total / 100)));
      int progressPX= map(progress, 0, 100, 0, 127);
      int h;
      int w;
      u8g2.drawBox(0, 50, 0, progressPX);
      u8g2.setCursor(20, 40);
      u8g2.print("Be Patient Dick");
      u8g2.sendBuffer();
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      u8g2.printf("Error[%u]: ", error);

      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

#endif