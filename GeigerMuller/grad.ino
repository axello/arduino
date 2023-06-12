/******************************************************************************/
/*  GRAD - GGLABS GRAD Ardunio Firmware                                       */
/*  Simple arduino sketch for the GGLABS dual tube Geiger-Muller counter      */
/*  Copyright 2021 Gabriele Gorla                                             */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GRAD is distributed in the hope that it will be useful,                   */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GRAD.  If not, see <http://www.gnu.org/licenses/>.             */
/*                                                                            */
/******************************************************************************/

// tube CPM to uS/h conversion factor
#define CPM2USV 220
//#define CPM2USV 928
// number of tubes installed
#define TUBES 2
// moving average window
#define WSIZE 10

#define LED1_PIN     8
#define LED2_PIN     9
#define SPKR_PIN     4
#define METER_PIN    5
#define METER_FS     185

#define LED_BLINK_MS 20


#define MAIN_DELAY_MS  2
#define LOG_PERIOD     60

int seconds = 0;
int logtime = 0;
volatile unsigned char flag = 0;
volatile int count1 = 0;
volatile int count2 = 0;
volatile char led1 = 0;
volatile char led2 = 0;

const float conv = (1./TUBES)/CPM2USV;

int count_array[WSIZE];
int idx=0;


void setup()
{
  int i;
  
  Serial.begin(9600);
  Serial.println("Seq , cnt1 , cnt2 , avg10 , uS/h");

  pinMode(LED1_PIN,OUTPUT); 
  pinMode(LED2_PIN,OUTPUT); 
  pinMode(SPKR_PIN,OUTPUT); 
  pinMode(METER_PIN,OUTPUT); 
  analogWrite(METER_PIN,182);

  cli();            //stop interrupts

  attachInterrupt(0, pin2irq, RISING);
  attachInterrupt(1, pin3irq, RISING);

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

//compare match register = [ 16MHz / (prescaler * desired interrupt frequency) ] - 1
  OCR1A = 62499;  // = (16*10^6) / (64*4) - 1 (must be <65536)

  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS12) ;  // prescaler 256
  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
  
}//end setup

ISR(TIMER1_COMPA_vect)
{
 //timer1 interrupt 1Hz
  seconds++;
  if(seconds>=LOG_PERIOD)
    flag=1;
}

void pin2irq()
{
  count1++;
  led1+=LED_BLINK_MS/MAIN_DELAY_MS;
  digitalWrite(LED1_PIN,1);
  digitalWrite(SPKR_PIN,!digitalRead(SPKR_PIN));
}

void pin3irq()
{
  count2++;
  led2+=LED_BLINK_MS/MAIN_DELAY_MS;
  digitalWrite(LED2_PIN,1);
  digitalWrite(SPKR_PIN,!digitalRead(SPKR_PIN));
}

void loop()
{
  int i;
  int t1,t2;
  int sum10;
  
  if(flag) {
    cli();  // stop interrupts
    flag=0;
    seconds=0;
    t1=count1;
    t2=count2;
    count1=0;
    count2=0;
    sei(); // allow interrupts

    count_array[idx]=t1+t2;
 
    if(logtime==0) {
      // initialize the moving average array(s)
      for(i=0;i<WSIZE;i++)
         count_array[i]=t1+t2;
    } 

    logtime++;
    idx++;
    if(idx==WSIZE) idx=0;
    
    sum10=0;
    for(i=0;i<WSIZE;i++)
      sum10+=count_array[i];

    analogWrite(METER_PIN,sum10*(METER_FS/1000.));

    Serial.print(logtime);
    Serial.print(" , ");
    Serial.print(t1);
    Serial.print(" , ");
    Serial.print(t2);
    Serial.print(" , ");
    Serial.print(sum10/(float)WSIZE,1);
    Serial.print(" , ");
    Serial.print(conv*sum10/(float)WSIZE,3);
    Serial.println();
  }
  
  if(led1) {
    cli();
    led1--;
    if(led1==0) digitalWrite(LED1_PIN,0);
    sei();
  }

  if(led2) {
    cli();
    led2--;
    if(led2==0) digitalWrite(LED2_PIN,0);
    sei();
  }
    
  if (led1==0 && led2==0) digitalWrite(SPKR_PIN,0);

  delay(MAIN_DELAY_MS);
}
