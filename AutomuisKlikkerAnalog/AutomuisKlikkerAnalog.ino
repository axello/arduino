/* Blink without Delay

 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

  Version meant for the digispark
  Version meant for the nano

 Axel Roest
 This version uses an analog value, derived from a potmeter, to set the click speed
 
 */
 
// constants

// #define DEBUG 1

const int ledPin =  3;      // the number of the LED pin
const int switchPin =  2;      // the number of the Switch pin
const int analogPin = A7;

// Variables will change :
int ledState = LOW;             // ledState used to set the LED
long interval = 200;           // interval at which to blink (milliseconds)

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
const unsigned long int clickDuration = 40;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, OUTPUT);

#ifdef DEBUG
   Serial.begin(57600);
   Serial.println("Auto MuisKlikker Analog");
#endif
}

void loop()
{
   static unsigned long previousMillis = 0;        // will store last time LED was updated
   static const unsigned long buttonCheckInterval = 50;    // the amount of milliseconds after which we process keys, the key-repeat rate in ms

  unsigned long currentMillis = millis();
  static unsigned long lastCheck = 0;

  if (currentMillis - lastCheck > buttonCheckInterval) {
    handleAnalog();
    lastCheck = currentMillis;  
  }

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
  digitalWrite(switchPin, ledState);
}

void handleAnalog() {
  //(-160*1024 + 512*360) / 512
  // doing some fixed num calculation
  const long a = -160;
  const long c = 360l * 512l;
  const long factor = 512;
  
  long analogValue = analogRead(analogPin);  // 0 - 1023
  long v = (a * analogValue);
  interval = (v + c) / factor;
#ifdef DEBUG
    Serial.print("ana: ");
    Serial.print(analogValue);
    Serial.print(" | int: ");
    Serial.println(interval);
#endif
}
