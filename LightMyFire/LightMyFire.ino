/* Sketch to demo Firebase on ESP32

  Axel Roest, 2019
*/

#if !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32) 
#error "This sketch runs only on ESP32 or ESP8266 target"
#endif 

#include <WiFi.h>
#include "secrets.h"

#include "Firebase.h"

//#include "icons.h"
//#include "fonts.h"
//#include "Drawdemo.h"

#include "Colorconversions.h"
#include "NeoPixel.h"

FirebaseData firebaseData;

/********************************
 * Function headers
 ********************************/
void wifiSetup();
void setNeopixel(float hue);

/********************************
 * Functions
 ********************************/
void setup() {
  
  Serial.begin(115200);
  String path = "/esp32/academy";
  wifiSetup();
  fbSetup();

  firebaseSetTest(path);
  firebaseGetTest(path);
}

void loop() {
  if (Firebase.getDouble(firebaseData, "/lights/academy/hue")) {
    float hue = firebaseData.floatData();
     Serial.print (hue);
     setNeopixels((float) hue);
//      Serial.print("Returned datatype: ");
//      Serial.println(firebaseData.dataType());
  } else {
    Serial.println(firebaseData.errorReason());
  }
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
}

void setNeopixels(float hue) {
  int rgb[3];

  hsi2rgb(hue * 360.0, 1.0, 0.5, rgb);
  Serial.printf("RGB: %u, %u, %u\n", rgb[0], rgb[1], rgb[2]);
  LedRGBON(hue);
}
