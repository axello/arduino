/* Pulse counter for water flow meters

    two water flow meters
    pulse or liters/minute count is sent to serial port, for later distribution by a Onion Omega or ESP

*/

/*
 * 7 segment stuff
 */

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

/* 
 *  waterflow meter stuff
 */
int meter1 = 2;
int meter2 = 3;
volatile unsigned int pulse1;
volatile unsigned int pulse2;

const int pulses_per_litre = 450;
unsigned long currentTime;
unsigned long cloopTime;
unsigned long totalLiters = 0;

const int reportMillis = 1000;

void setup()
{
  Serial.begin(57600);
  setUpDisplay();

  pinMode(meter1, INPUT);
  pinMode(meter2, INPUT);
  attachInterrupt(digitalPinToInterrupt(meter1), count_pulse, RISING);

  currentTime = millis();
  cloopTime = currentTime;
}

void loop()
{
  pulse1 = 0;
  
  interrupts();
  delay(1000);
  noInterrupts();

  Serial.print("Pulses per second: ");
  Serial.print(pulse1);

  unsigned int l_hour = (pulse1 * 60) / 6.6;    // pulses / sec * minute
  totalLiters += l_hour;
  Serial.print(" -->  liter/h: ");
  Serial.println(l_hour);
  segmentsFromNumber(totalLiters);

}

void count_pulse()                   // Interrupt function
{
  pulse1++;
}



/*
 * 7 segment stuff
 * 
 */

 
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
