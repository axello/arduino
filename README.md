# arduino
Arduino sketches

### AutomuisKlikker

Whenever you need to click 1000 times for a stupid online game.
Has up and down buttons to change the speed of ‘clicking’.
### BlinkWithDeepSleep
Test with ESP.deepSleep for ESP8266

### LightMyFire
Arduino sketch do demo interactivity with Google Firebase and an ESP32.
### mqtt_serial_esp8266
Upload stuff to domoticz with the mqtt protocol and the PubSubClient from an ESP8266.

### Scan-I2C-WiFi
This is a sample sketch do demonstrate how to use ESP8266 with OLED, I2C Scanner, OTA and OLED Display.
### TrackBox
A sketch for the MicroView OLED+Arduino gadget. It measures light pulses on a phototransistor and shows them on the display. You can set a preset using up/down and a decimal point button.
Also a test for the MicroView.

### WaterFlow
Sketch to sample two water flowmeters and display the 
pulses / liter and accumulating values on the serial port. A separate Onion processor was then used to put the values on a server.
### WaterWarning
This sketch plays a melody when a water level is detected at the waterDetectorPin. It only tests every 5 minutes, which is enough annoyance for my application. You may want to change the value for interval to something shorter.


## Wifi
For the sketches that make use of wifi or other 'secrets', make sure you put your wifi ssid and key in a separate file 'secrets.h', which is gitignored.

```
/*
Contains ssid and other secret info
which should not appear in github
*/

const char* ssid = ";
const char* password = "";
const char* mqtt_server = "";

```