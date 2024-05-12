#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL2tR6ZJyMn"
#define BLYNK_TEMPLATE_NAME "temperature and humidity"

#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ota.h"

char auth[] = "MLJlzsR0BLrYFJOeN32NrIZoWkiiwGd9";
char ssid[] = "Tip-jar";
char pass[] = "PASSWORD1234LOL";


#define DHTPIN 2  // What digital pin we're connected to
#define led A0    // LED connected to GPIO pin D4 (physical pin 2 on NodeMCU)

#define DHTTYPE DHT11  // Uncomment whatever type you're using: DHT11, DHT22, or DHT21
// #define DHTTYPE DHT22
// #define DHTTYPE DHT21

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

Adafruit_NeoPixel strip1(120, D5, NEO_GRB + NEO_KHZ800);   // Strip with 8 LEDs
Adafruit_NeoPixel strip2(120, D6, NEO_GRB + NEO_KHZ800);  // Strip with 151 LEDs

WidgetTerminal terminal(V3);  // Terminal widget on V3
WidgetLED alarmLed(V4);       // LED widget for the alarm on V4

int brightness = 255;                // Initial brightness value
bool discoMode = false;              // Flag for disco mode
int discoSpeed = 50;                 // Initial disco speed value
unsigned long discoModeTimeout = 0;  // Timeout for disco mode activation

void setup() {
  Serial.begin(115200);
  OTAinit();

  Blynk.begin(auth, ssid, pass);

  dht.begin();
  timer.setInterval(1000L, sendSensor);

  strip1.begin();
  strip1.show();  // Initialize all pixels to 'off'

  strip2.begin();
  strip2.show();  // Initialize all pixels to 'off'

  Blynk.virtualWrite(V8, brightness);  // Set initial brightness on the app
  Blynk.virtualWrite(V8, brightness);  // Sync initial brightness with the board

  Blynk.virtualWrite(V9, discoMode);  // Set initial disco mode on the app
  Blynk.virtualWrite(V9, discoMode);  // Sync initial disco mode with the board

  Blynk.virtualWrite(V10, discoSpeed);  // Set initial disco speed on the app
  Blynk.virtualWrite(V10, discoSpeed);  // Sync initial disco speed with the board
}

void loop() {
  ArduinoOTA.handle();

  Blynk.run();
  timer.run();

  if (discoMode ) {
    discoEffect(strip1, discoSpeed);
    discoEffect(strip2, discoSpeed);
  }
}

BLYNK_WRITE(V0)  // RGB widget for strip1 on V0
{
  int r = param[0].asInt();
  int g = param[1].asInt();
  int b = param[2].asInt();

  setColor(strip1, r, g, b);
  terminal.println("Strip 1 Color:");
  terminal.print("  R: ");
  terminal.println(r);
  terminal.print("  G: ");
  terminal.println(g);
  terminal.print("  B: ");
  terminal.println(b);
  terminal.flush();
}

BLYNK_WRITE(V1)  // RGB widget for strip2 on V1
{
  int r = param[0].asInt();
  int g = param[1].asInt();
  int b = param[2].asInt();

  setColor(strip2, r, g, b);
  terminal.println("Strip 2 Color:");
  terminal.print("  R: ");
  terminal.println(r);
  terminal.print("  G: ");
  terminal.println(g);
  terminal.print("  B: ");
  terminal.println(b);
  terminal.flush();
}

BLYNK_WRITE(V8)  // Slider widget on V8
{
  brightness = param.asInt();
  setBrightness(strip1, brightness);
  setBrightness(strip2, brightness);
  terminal.print("Brightness: ");
  terminal.println(brightness);
  terminal.flush();
}

BLYNK_WRITE(V9)  // Switch widget on V9
{
  discoMode = param.asInt();
  if (!discoMode)
    discoModeTimeout = 0;  // Reset the timeout when disco mode is turned off
  terminal.print("Disco Mode: ");
  terminal.println(discoMode ? "ON" : "OFF");
  terminal.flush();
}

BLYNK_WRITE(V10)  // Slider widget on V10
{
  discoSpeed = 255 - param.asInt();  // Swap the speed values
  terminal.print("Disco Speed: ");
  terminal.println(discoSpeed);
  terminal.flush();
}

BLYNK_WRITE(V2)  // Button widget on V2
{
  if (param.asInt() == 1) {
    discoModeTimeout = millis();        // Set the current time as the start time of disco mode
    discoMode = true;                   // Activate disco mode
    Blynk.virtualWrite(V9, discoMode);  // Sync disco mode with the app
    Blynk.virtualWrite(V4, LOW);        // Turn off the LED widget
    digitalWrite(led, LOW);             // Turn off the LED
    terminal.println("Disco mode activated for 60 seconds!");
    terminal.flush();
  } else if (param.asInt() == 0) {
    discoMode = false;
    Blynk.virtualWrite(V9, discoMode);  // Sync disco mode with the app

    terminal.println("Disco mode OFF");
    terminal.flush();
  }
}

void setColor(Adafruit_NeoPixel& strip, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

void setBrightness(Adafruit_NeoPixel& strip, uint8_t brightness) {
  strip.setBrightness(brightness);
  strip.show();
}

void discoEffect(Adafruit_NeoPixel& strip, int speed) {
  static uint32_t prevTime = 0;
  static uint8_t colorIndex = 0;

  if (millis() - prevTime >= (255 - speed)) {
    prevTime = millis();
    colorIndex = (colorIndex + 1) % 256;

    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(strip, (i + colorIndex) & 255));
    }
  }
  strip.show();
}

uint32_t Wheel(Adafruit_NeoPixel& strip, byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 0, 255 - WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(255 - WheelPos * 3, 255 - WheelPos * 3, WheelPos * 3);
}

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    terminal.println("Failed to read from DHT sensor!");
    terminal.flush();
    return;
  }

  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

  terminal.print("Temperature: ");
  terminal.print(t);
  terminal.println(" °C");
  terminal.print("Humidity: ");
  terminal.print(h);
  terminal.println(" %");
  terminal.flush();

  if (t >= 25) {
    digitalWrite(led, HIGH);
    Blynk.virtualWrite(V4, HIGH);
  } else {
    digitalWrite(led, LOW);
    Blynk.virtualWrite(V4, LOW);
  }
}
