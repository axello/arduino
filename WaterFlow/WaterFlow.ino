/* Pulse counter for water flow meters

    two water flow meters
    pulse or liters/minute count is sent to serial port, for later distribution by a Onion Omega or ESP

*/

/*
 * 7 segment stuff
 */
#define SEGMENT7  0

#if SEGMENT7
const int clock = 5; //SCK
const int latch = 6; //RCK
const int data = 7;  //DIO

byte value[] = { B11000000, // 0
                 B11111001, // 1
                 B10100100, // 2
                 B10110000, // 3
                 B10011001, // 4
                 B10010010, // 5
                 B10000010, // 6
                 B11111000, // 7
                 B10000000, // 8
                 B10010000, // 9
                 B11111111
               };// display nothing

byte segments[8];
int decimalPoint = 8;
#endif

/* 
 *  waterflow meter stuff
 */
const int meter1 = 2;
const int meter2 = 3;
const int displaySwitch = 8;

volatile unsigned long int pulse1;
volatile unsigned long int pulse2;

const int pulses_per_litre = 450;
unsigned long currentTime;
unsigned long cloopTime;
unsigned long totalLiters = 0;

const int reportMillis = 10000;     // every 10 seconds / minute is good enough!

void setup()
{
  Serial.begin(57600);

#if SEGMENT7
  setUpDisplay();
#endif

  pinMode(meter1, INPUT_PULLUP);
  pinMode(meter2, INPUT_PULLUP);
  pinMode(displaySwitch, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(meter1), count_pulse1, RISING);
  attachInterrupt(digitalPinToInterrupt(meter2), count_pulse2, RISING);

  currentTime = millis();
  cloopTime = currentTime;
  Serial.println("{\"startup\" : [\"metera\", \"meterb\"]}");
}

void loop()
{
  pulse1 = 0;
  pulse2 = 0;
  unsigned int l_hour = 0;
  
  interrupts();
  delay(reportMillis);
  noInterrupts();

  // meter A
  if (pulse1 > 0) {
    
    // Serial.print("A pulses: ");
    // Serial.println(pulse1);
  
    l_hour = (pulse1 * 60) / 6.6;    // pulses / sec * minute
    totalLiters += l_hour;
    
    // Serial.print("A -->  liter/h: ");
    // sprintf("\"metera\": {\"ppm\": %ul, \"lph\": %u}", pulse1, l_hour);
    String text = "{\"metera\": {\"ppm\": " + String(pulse1) + ", \"lph\": " + String(l_hour) + "}}";
    Serial.println(text);
  }
  // meter B
  if (pulse2 > 0) {
    //    Serial.print("B pulses per second: ");
    // Serial.println(pulse2);
  
    l_hour = (pulse2 * 60) / 6.6;    // pulses / sec * minute
    totalLiters += l_hour;
    String text = "{\"meterb\": {\"ppm\": " + String(pulse2) + ", \"lph\": " + String(l_hour) + "}}";
    // Serial.print("B -->  liter/h: ");
    // Serial.println(l_hour);
    Serial.println(text);
 }

#if SEGMENT7
  segmentsFromNumber(totalLiters);
#endif
}

void count_pulse1()                   // Interrupt function
{
  pulse1++;
}


void count_pulse2()                   // Interrupt function
{
  pulse2++;
}


/*
 * 7 segment stuff
 * 
 */

#if SEGMENT7
void setUpDisplay() {
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, OUTPUT);

  setupDisplayTimer();

  for (int i = 0 ; i < 8 ; i++) {
    segments[i] = 0;
  }  
}

void setupDisplayTimer()
{
  cli();//stop interrupts
  //set timer0 interrupt at 980Hz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  OCR0A = 255;//(must be <256) --> 16000000 / (prescaler*255) = 980 Hz
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);   //prescaler = 64
  TIMSK0 |= (1 << OCIE0A);
  sei();//allow interrupts
}

/* 7 segment display interupt service routine */
ISR(TIMER0_COMPA_vect)
{
  static byte ii = 0;
  byte vvalue;

  if (++ii == 8) ii = 0;

  // choose which digit to display, depending on counter ii
  vvalue = segments[ii];

  digitalWrite(latch, LOW);
  shiftOut(data, clock, MSBFIRST, B11111111);
  shiftOut(data, clock, MSBFIRST, B11111111);
  digitalWrite(latch, HIGH);

  digitalWrite(latch, LOW);
  shiftOut(data, clock, MSBFIRST, 0x01 << ii); // digit[ii] =  1 << ii?
  // show decimal point, or not
  byte pointValue = (ii == decimalPoint) ? value[vvalue] & 0x7F : value[vvalue];
  shiftOut(data, clock, MSBFIRST, pointValue);

  digitalWrite(latch, HIGH);
}

void segmentsFromNumber(long num) {
    long exponent = 1;
    for (int j = 0 ; j < 8 ; j++) {
      if (j > 0) {
        exponent = exponent * 10;
      }
      byte value = (num / exponent) % 10;
      segments[7 - j] = value;
    }
}
#endif
