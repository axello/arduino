/* Blink without Delay

 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.
 
 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 modified 11 Nov 2013
 by Scott Fitz                                                                                                                                                                                                                                                                                                                             gerald
 
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay

 Axel Roest
 This version uses an analog value, derived from a potmeter, to set the click speed
 
 */

typedef enum buttonStateType {
  none = -1,
  low = 0,
  high = 1
};

// constants
#define BUTTONCOUNT 2
#define DEBUG 1


const int upPin =  3;      // the number of the LED pin
const int downPin =  4;      // the number of the LED pin
const int upLedPin = 8;
const int downLedPin = 9;
const int ledPin =  13;      // the number of the LED pin
const int analogPin = 0;

// Variables will change :
int ledState = LOW;             // ledState used to set the LED
long interval = 200;           // interval at which to blink (milliseconds)

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
const unsigned long int clickDuration = 200;

void setup() {
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(upLedPin, OUTPUT);
  pinMode(downLedPin, OUTPUT);
//  lastButtonState[0] = HIGH;
//  lastButtonState[1] = HIGH;
//  lastDebounceTime[0] = 0;
//  lastDebounceTime[1] = 0;

#ifdef DEBUG
   Serial.begin(57600);
#endif
}

void loop()
{
   static unsigned long previousMillis = 0;        // will store last time LED was updated
   static const unsigned long buttonCheckInterval = 50;    // the amount of milliseconds after which we process keys, the key-repeat rate in ms

  unsigned long currentMillis = millis();
  static unsigned long lastCheck = 0;

  if (currentMillis - lastCheck > buttonCheckInterval) {
//    handleButtons();
    handleAnalog();
    lastCheck = currentMillis;  
  }

  // different timing for on and off
  // if the LED is off turn it on and vice-versa:
  if (ledState == LOW) {
    if (currentMillis - previousMillis >= clickDuration) {
       ledState = HIGH;
       previousMillis = currentMillis;   
    }
  } else {
    if (currentMillis - previousMillis >= interval) {
       ledState = LOW;
       // save the last time you blinked the LED 
       previousMillis = currentMillis;   
    }
  }
  // set the LED with the ledState of the variable:
  digitalWrite(ledPin, ledState);
}

void handleAnalog() {
  //(-160*1024 + 512*360) / 512
  // doing some fixed num calculation
  const long a = -160;
  const long c = 360 * 512;
  const long factor = 512;
  
  int analogValue = analogRead(analogPin);  // 0 - 1023
  interval = (a * analogValue + c) / factor;
}

void handleButtons() {
  static const unsigned long defaultCrement = 50;    // the decrement or increment in ms per keypress
  static unsigned long crement = 50;    // the decrement of increment per keypress

  int reading = handleDebounce(upPin);
 
  switch (reading) {
    case LOW:
 #ifdef DEBUG
   Serial.print("L ");
 #endif
     interval += crement;
     crement += defaultCrement;     // accellerate moderately
     digitalWrite(upLedPin, HIGH);
    break;
    case HIGH:
    crement = defaultCrement;
#ifdef DEBUG
    Serial.print("H ");
#endif
      digitalWrite(upLedPin, LOW);
    break;
  }
  reading = handleDebounce(downPin);
  switch (reading) {
    case LOW:
      interval = (interval < 100) ? 100 : interval - crement;
      digitalWrite(downLedPin, HIGH);
     crement += defaultCrement;     // accellerate moderately
    break;
    case HIGH:
     crement = defaultCrement;
     digitalWrite(downLedPin, LOW);
    break;
  }

}

/* 
 *  returns : -1 : undefined (debouncing)
              0 : low
              1 : high
*/              
int handleDebounce(int pin) {
  const unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
  static int buttonState[BUTTONCOUNT] = {};             // the current reading from the input pin
  static int lastButtonState[BUTTONCOUNT] = {};   // the previous reading from the input pin
  static unsigned long lastDebounceTime[BUTTONCOUNT] = {};  // the last time the output pin was toggled

    // read the state of the switch into a local variable:
  int reading = digitalRead(pin);
#ifdef DEBUG
     Serial.print(pin);
     Serial.print(reading);
     Serial.print(" ");
#endif

  int pinIndex = pin - upPin;
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[pinIndex]) {
    // reset the debouncing timer
    lastDebounceTime[pinIndex] = millis();
  }

  lastButtonState[pinIndex] = reading;
  
  if ((millis() - lastDebounceTime[pinIndex]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // Toggle: if the button state has changed:
//    if (reading != buttonState[pinIndex]) {
//      buttonState[pinIndex] = reading;
//      return reading;
//    }
    // just return the buttonState:
    buttonState[pinIndex] = reading;
    return reading;
  }
  return -1;
}
