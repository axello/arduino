/* Blink without Delay

 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

  Version meant for the digispark
  Version meant for the nano
  Version meant for a NodeMCU (ESP8266), with the Blynk library for remote toggling
  
 Axel Roest
 This version uses an analog value, derived from a potmeter, to set the click speed
 
 */

#include <ESP8266WiFi.h>
// #include <BlynkSimpleEsp8266.h>
#include "secrets.h"

// constants

#define DEBUG 0
#ifdef DEBUG
#define BLYNK_PRINT Serial
#endif

const int ledPin =  D3;      // the number of the LED pin
const int switchPin =  D2;      // the number of the Switch pin
const int analogPin = A0;
const int onlineSwitch = D4;    // if low, no connection with Blynk
const int onlineLED = D0;       // show/hide connection with Blynk, on-board LED


// Variables will change :
int ledState = LOW;             // ledState used to set the LED
long interval = 200;            // interval at which to blink (milliseconds)
boolean clickEnabled = true;            // remote enabler/disabler
boolean blynkEnabled = false;            // Blynk enabler/disabler

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
const unsigned long int clickDuration = 40;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

// This function is called every time the device is connected to the Blynk.Cloud
// BLYNK_CONNECTED()
// {
//   // Change Web Link Button message to "Congratulations!"
//   Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
//   Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
//   Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
//   Serial.println(" | Blynk Connected");  

// }

void setup() 
{
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, OUTPUT);
  pinMode(onlineSwitch, INPUT_PULLUP);
   
#ifdef DEBUG
   Serial.begin(57600);
   Serial.println("Auto MuisKlikker Analog");
#endif

// # Blynk.begin(auth, ssid, pass);
}

void loop()
{
  unsigned long currentMillis = millis();
  static unsigned long lastBlynkButtonCheck = 0;

  // if click is disabled, we still need to 'end our click's high state'
  if (clickEnabled || ledState) {
    handleClick(currentMillis);
  }

  // check Blynk button every second
  if (currentMillis - lastBlynkButtonCheck > 1000) {
    // blynkEnabled = boolean(digitalRead(onlineSwitch));
    // blynkEnabled = false;
    digitalWrite(onlineLED, blynkEnabled);
  }
  if (blynkEnabled) {
    // Blynk.run();
  }
}

///////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////
void handleClick(unsigned long currentMillis)
{
  static const unsigned long buttonCheckInterval = 50;    // the amount of milliseconds after which we process keys, the key-repeat rate in ms
  static unsigned long lastCheck = 0;
  
  if (currentMillis - lastCheck > buttonCheckInterval) {
    handleAnalog();
    lastCheck = currentMillis;  
  }
  doPulses(currentMillis);
}

void handleAnalog() 
{
  //(-160*1024 + 512*360) / 512
  // doing some fixed num calculation
  const long a = -160;
  const long c = 360l * 512l;
  //const long factor = 512;  // 40 ms interval
  const long factor = 688;  // 30 ms interval
  
  long analogValue = analogRead(analogPin);  // 0 - 1023
  long v = (a * analogValue);
  long oldInterval = interval;
  interval = (v + c) / factor;

  if (oldInterval != interval) {
    // send the new value to Blynk
    updateInterval(interval);
  }
  
#ifdef DEBUG
    Serial.print("ana:");
    Serial.print(analogValue);
    Serial.print(" | int:");
    Serial.print(interval);

    if (clickEnabled) {
      Serial.print(" | Clk");
    } else {
      Serial.print(" | ___");
    }

    if (blynkEnabled) {
      Serial.println(" | Blynk On");
    } else {
      Serial.println(" | Blynk Off");
    }
#endif
}

void doPulses(unsigned long currentMillis) 
{
  static unsigned long previousMillis = 0;        // will store last time LED was updated

    // different timing for on and off
  // if the LED is off turn it on and vice-versa:
  if (ledState == LOW) {
    if (currentMillis - previousMillis >= interval) {
       ledState = HIGH;
       previousMillis = currentMillis;   
    }
  } else {
    if (currentMillis - previousMillis >= clickDuration) {
       ledState = LOW;
       // save the last time you blinked the LED 
       previousMillis = currentMillis;   
    }
  }
  // set the LED with the ledState of the variable:
  digitalWrite(ledPin, ledState);
  digitalWrite(switchPin, !ledState);
}

// Update virtual pins
void updateInterval(long interval) {
  // Blynk.virtualWrite(V1, interval);
}

/// BLYNK STUFF
// BLYNK_WRITE(V0)
// {
//   clickEnabled = boolean(param.asInt());
// }

// BLYNK_READ(V0)
// {
//   Blynk.virtualWrite(V1, int(clickEnabled));
// }

// BLYNK_READ(V1)
// {
//   updateInterval(interval);
// }

// BLYNK_CONNECTED()
// {
//   Serial.println(" | Blynk Connected");  
// }