/*
//Demo for 
//by Open-Smart Team and Catalex Team
//catalex_inc@163.com
//Store: http://www.aliexpress.com/store/1199788
//	   http://dx.com
//Demo Function: Read Water Flow Meter and output reading in litres/hour

*/

volatile int  flow_frequency;  // Measures flow meter pulses
unsigned int  l_hour;          // Calculated litres/hour                      
unsigned char flowmeter = 2;  // yellow pin of Flow Meter to arduino Pin number
unsigned long currentTime;
unsigned long cloopTime;

void flow ()                  // Interruot function
{ 
   flow_frequency++;
} 

void setup()
{ 
   pinMode(flowmeter, INPUT);
   Serial.begin(9600); 
   attachInterrupt(0, flow, RISING); // Setup Interrupt 
                                     // see http://arduino.cc/en/Reference/attachInterrupt
   sei();                            // Enable interrupts  
   currentTime = millis();
   cloopTime = currentTime;
} 

void loop ()    
{
   currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {     
      cloopTime = currentTime;              // Updates cloopTime
      // Pulse frequency (Hz) = 6.6*Q, Q is flow rate in L/min. (Results in +/- 3% range)
      l_hour = (flow_frequency * 60 / 6.6); // (Pulse frequency x 60 min) / 6.6Q = flow rate in L/hour 
      flow_frequency = 0;                   // Reset Counter
      Serial.print(l_hour, DEC);            // Print litres/hour
      Serial.println(" L/hour");
   }
}
