/* TrackBox
 *  display is 64 x 48 pixels
 */
#include <MicroView.h>


// globals
bool setupMode = false;
int decimal = 6;

// constants
const int XOFFSET = 2;

const int decimalPin = 3;                   // the number of the decimal pushbutton pin
const int upPin =  2;                       // the number of the up pushbutton pin
// digital read
// const int pulsePin = 5;                     // the pin where the phototransistor is connected to
// analog pulse read
long lastTime;
const long repeatDelay = 5000;

int nameOffset = 64;                     // the pin where the phototransistor is connected to
const int NAMEOFFSETY = 24;
const char *naam = "Ger Baron  •  Ger Baron  •  ";
const char *voornaam = "Ger";
const char *achternaam = "Baron";

// necessary for microchip library
void spin(int16_t lowVal, int16_t highVal, int16_t stepSize,
          unsigned long stepDelay, void (*drawFunction)(int16_t val));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
//  pinMode(decimalPin, INPUT);
//  pinMode(upPin, INPUT);
//  pinMode(pulsePin, INPUT);
  
  uView.begin();
  setupMainDisplay();
//  setNameLabelAt(56, NAMEOFFSETY, voornaam);
//  setNameLabelAt(-8, NAMEOFFSETY + 12, achternaam);
  lastTime = millis();
}

void loop() {
  if (nameOffset > 0) {
    setNameLabelAt(nameOffset, NAMEOFFSETY, voornaam);
    setNamePartLabelAt(nameOffset, NAMEOFFSETY + 12, achternaam);

//    setNameLabelAt( 8 - nameOffset, NAMEOFFSETY + 12, achternaam);
    nameOffset--;
    if (nameOffset == 0) {
      lastTime = millis() + repeatDelay;
    }
    uView.display();
  }

  if (nameOffset == 0 && millis() > lastTime) {
    nameOffset = 64;
    setupMainDisplay();
  } else {
    delay(30);
  }
}

void setupMainDisplay(void)
{
    uView.clear(PAGE);
    uView.setFontType(0);
    uView.setCursor(0, 4);
    uView.print("Trackbox");
    uView.setCursor(0, 14);
    uView.print("654 for:");
//    uView.setCursor(3, 20);
//    uView.setFontType(1);
}

void setNameLabelAt(int offsetx, int offsety, char *name)
{
  uView.setFontType(1);
  uView.setCursor(offsetx, offsety);
  uView.print(name);
}

#define CHARWIDTH 8
void setNamePartLabelAt(int offsetx, int offsety, char *name)
{
    int offset = offsetx / CHARWIDTH;
    int len = (int) strlen(name);
    int charpos = len - offset;
    if (charpos > 0) {  // show chars
        int newoffset = - (offsetx % CHARWIDTH);
        char *newName ;
        if (charpos <= len) {
          newName = name + len - charpos;
        } else {
          newName = name;
        }
        uView.setFontType(1);
        uView.setCursor(newoffset, offsety);
        uView.print(newName);
    }
}

void debugUp(int up) {
  if (up == 0) {
     uView.setFontType(0);
     uView.setCursor(56, 2);
     uView.print("u");
//     Serial.print("u ");
  } else {
     uView.setFontType(0);
     uView.setCursor(56, 2);
     uView.print(" ");
  }  
}

void debugDecimal(int dec)
{
    if (dec == 0) {
     uView.setFontType(0);
     uView.setCursor(48, 2);
     uView.print("d");
//      Serial.print("d ");
    } else {
     uView.setFontType(0);
     uView.setCursor(48, 2);
     uView.print(" ");
  }
}

void debugPulse(bool up) {
  uView.setFontType(0);
  uView.setCursor(56, 10);
  if (up) {
     uView.print("P");
  } else {
     uView.print(" ");
  }  
}

void showDecimal(int decimal)
// puts a line underneath the decimal we're editing
{
  int y = 23;
  int x = XOFFSET + (6 - decimal) * 6;
  // erase old line
  uView.lineH(1,y,46,BLACK,NORM);
  uView.lineH(x,y,6,WHITE,NORM);

  uView.setCursor(54, 15);
  uView.print(decimal);

}

void setTextLabel2(int32_t val)
{
  const uint8_t offsetY = 30;
  const uint8_t offsetX = XOFFSET;
  uView.setCursor(offsetX, offsetY);
  printLabel(val);
}

void setTextLabel1(int32_t val)
{
  const uint8_t offsetY = 15;
  const uint8_t offsetX = XOFFSET;
  
  uView.setCursor(offsetX, offsetY);
  printLabel(val);
}

// prints the long value at the position already set by setCursor
void printLabel(int32_t val)
{
  char txt[10];
  
  int chars = sprintf(txt,"%07ld",val);
  uView.print(txt);
}

void demoNumber(int num) {
  uView.clear(PAGE);
  uView.setCursor(0, 0);
  uView.print(num);
  uView.print(":");
}

void fontDemo(void)
{
  for (int n=0 ; n < 6 ; n++) {
    uView.clear(PAGE);
    uView.setFontType(0);
     uView.setCursor(0, 0);
    uView.print("type:");
     uView.setCursor(40, 0);
    uView.print(n);
    uView.setFontType(n);
    uView.setCursor(0, 30);
  setTextLabel2(123456);
  uView.display();
  delay(5000);
  }
}


