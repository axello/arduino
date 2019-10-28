/* Sketch to demo Firebase on ESP32

  Axel Roest, 2019
*/

#if !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32) 
#error "This sketch runs only on ESP32 or ESP8266 target"
#endif 

#include <WiFi.h>
#include "secrets.h"

#include "Firebase.h"
#include "NeoPixel.h"
#include "Drawdemo.h"

FirebaseData firebaseData;

const int debugPin = 4;

/********************************
 * Function headers
 ********************************/
void wifiSetup();
void setNeopixel(float hue);
bool showErrorMessage();

/********************************
 * Functions
 ********************************/
void setup() {
  
  Serial.begin(115200);
  pinMode(debugPin, INPUT_PULLUP);

  String path = "/esp32/academy";
  wifiSetup();
  fbSetup();
  LedRGBOFF();
  displaySetup();
  
//  firebaseSetTest(path);
//  firebaseGetTest(path);
}

void loop() {
  static time_t lastTime = millis();
//  static int counter = 0;
  static int errorCount = 0;
  
  if (Firebase.getDouble(firebaseData, "/lights/academy/hue")) {
     float hue = firebaseData.floatData();
     setNeopixels((float) hue);
  } else {
    if (showErrorMessage()) {
      Serial.println(firebaseData.errorReason());
    }
    if (++errorCount > 2) {
      // lights must be off
      LedRGBOFF();
      errorCount = 0;
    }
    
  }
  
//  counter++;
//  if (millis() > (lastTime + 1000)) {
//    Serial.println(counter);
//    counter = 0;
//    lastTime = millis();
//  }
}

bool showErrorMessage() {
  return (digitalRead(debugPin) == 1);
}

void wifiSetup() {
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connected to gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.println();

  OLEDDisplayUiState state;
  OLEDDisplay *oled = (OLEDDisplay *) &display;
  Serial.println((unsigned int) &display);
//  drawFrameWifi(&display, &state, 0, 0);
}

void setNeopixels(float hue) {
  int rgb[3];

  LedRGBON(hue);
}
