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
  displaySetup();
  wifiSetup();
  fbSetup();
  LedRGBOFF();
  
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
//     OLEDDisplayUiState state;
//  OLEDDisplay *oled = (OLEDDisplay *) &display;
//
//      drawFrameNet(oled, &state, 0, 0);

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
  OLEDDisplayUiState state;
  OLEDDisplay *oled = (OLEDDisplay *) &display;

  int index = 0;
  int tries = 0;
  char wifi[40];
  char password[40];
  int str_len; 
  
  do {
    str_len = wifiSSID[index].length() + 1;
    wifiSSID[index].toCharArray(wifi, str_len);
    str_len = wifiPassword[index].length() + 1;
    wifiPassword[index].toCharArray(password, str_len);

    WiFi.begin(wifi, password);
    Serial.print("Connecting to Wi-Fi '");
    Serial.print(wifi);
    Serial.println("'");
    drawFrameWifiConnecting(oled);
 
    while(WiFi.status() != WL_CONNECTED && tries < 20) {
      Serial.print(".");
      delay(300);
      ++tries;
    }
    if (tries >= 20) {
        if (++index >= wifiCount) {
          index = 0;
        }
        tries = 0;
    }
  } while  (WiFi.status() != WL_CONNECTED);
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connected to gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.println();


//  Serial.print(F("Wifi display found at: "));
//  unsigned long *adres = (unsigned long *) &display;
//
//  Serial.printf("%p\n",display);
  drawFrameWifi(oled, &state, 0, 0);

//Er is gewoon iets heel raars aan de hand met display!
//    display.clear();
//    display.setTextAlignment(TEXT_ALIGN_CENTER);
//    display.setFont(Roboto_Condensed_Bold_Bold_16);
//    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
//    // on how to create xbm files
//    display.drawXbm(  (128 - WiFi_width) / 2, 0, WiFi_width, WiFi_height, WiFi_bits);
//    display.drawString( 64, WiFi_height + 4, WiFi.localIP().toString());
//    ui.disableIndicator();

}

void setNeopixels(float hue) {
  int rgb[3];

  LedRGBON(hue);
}
