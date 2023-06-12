/* Blink without Delay

 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

 Used as a test for ESP8266 deep sleep functionality, as the hardware is buggy.
 
 The circuit:
 * LED attached from pin 2 to ground (std ESP12 wifi pin).
 
 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 modified 11 Nov 2013
 by Scott Fitzgerald
 
 
 This example code is in the public domain.

 based on
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

// constants won't change. Used here to set a pin number :
const int ledPin =  2;      // the number of the LED pin
const int blinks = 10;
int   counter;

// Variables will change :
int ledState = LOW;             // ledState used to set the LED
int i;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
long interval = 200;           // interval at which to blink (milliseconds)

// Time to sleep (in seconds):
const int sleepTimeS = 15;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(0, OUTPUT);  // weird GPIO behaviour
  digitalWrite(0,HIGH);
  
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Waking up");
  counter = blinks;
}

void loop()
{
  unsigned long currentMillis = millis();

//  if (digitalRead(ledPin) == 1) {
//    interval = 50;
//  } else {
//    interval = 400;
//  }
  
  if(currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    ledState = (ledState == LOW) ? ledState = HIGH : ledState = LOW;
    digitalWrite(ledPin, ledState);
    counter--;
  }
  if (0 == counter) {    
    Serial.println("Entering Deepsleep");
    delay(100);
//    ESP.deepSleep(sleepTimeS * 1000000);
    ESP.deepSleep(sleepTimeS * 1000000, WAKE_RF_DISABLED);

  }
}
