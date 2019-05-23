/* Pulse counter for water flow meters

    two water flow meters
    pulse or liters/minute count is sent to serial port, for later distribution by a Onion Omega or ESP

*/

int meter1 = 2;
int meter2 = 3;
volatile unsigned int pulse1;
volatile unsigned int pulse2;

const int pulses_per_litre = 450;
unsigned long currentTime;
unsigned long cloopTime;

const int reportMillis = 1000;

void setup()
{
  Serial.begin(57600);

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
  Serial.print(" -->  liter/h: ");
  Serial.println(l_hour);
}

void count_pulse()                   // Interrupt function
{
  pulse1++;
}
