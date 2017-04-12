/*
  WaterWarning
  this sketch plays a melody when a water level is detected at the waterDetectorPin
  it only tests every 5 minutes, which is enough annoyance for my application. You
  may want to change the value for interval to something shorter.

 http://www.arduino.cc/en/Tutorial/Tone

 */
#include "pitches.h"

const int speakerPin = 3;
const int testPin = 2;
const int waterDetectorPin = 5;
bool playing = false;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 300000;           // interval at which to blink (milliseconds)


// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void tricalabamba() {
    // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(3, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(3);
  }
  playing = false;
}

void setup() {
  Serial.begin(115200);
  pinMode(speakerPin, OUTPUT);
  pinMode(testPin, INPUT_PULLUP);
  pinMode(waterDetectorPin, INPUT);
}

void loop() {
   if (digitalRead(testPin) == LOW && !playing) {
      playing = true;
      tricalabamba();
   }
   if ((millis() - previousMillis) >= interval) {
     // only test level and make noise every interval ms
  
     int water = digitalRead(waterDetectorPin);
     int level = analogRead(A0);
     
     Serial.print(water);
     Serial.print(" --> ");
     Serial.println(level);
  
     if (water == LOW && !playing) {
        playing = true;
        tricalabamba();
     }
     previousMillis = millis();
   }
  delay(100);
   
}
