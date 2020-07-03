boolean toggle0 = 0;
boolean toggle1 = 0;

volatile int freq = 2000;
int freq_temp = freq;

/********glogal variable***************/
int g_freq, g_phase;

#include <Wire.h>
#include "QSS014_cmn.h"

#define PIN 7
#define PIN2 13
ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle0){
    PORTD = B10000000;
    toggle0 = 0;
  }
  else{
    PORTD = B00000000;
    toggle0 = 1;
  }
}

ISR(TIMER2_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle1){
    PORTB = B00100000;
    toggle1 = 0;
  }
  else{
    PORTB = B00000000;
    toggle1 = 1;
  }
}


void setup(){
  Serial.begin(9600);
  Wire.begin(SLAVE_MCU_I2C_ADDR);
  Wire.onReceive(ReceiveEvent);
  //set pins as outputs
  pinMode(PIN, OUTPUT);
  pinMode(PIN2, OUTPUT);
  cli();//stop interrupts
  TCCR0A = 0;// set entire TCCR2A register to 0
	TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
//    OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
//  Serial.println(freq);
//  Serial.println(freq*64L);
  OCR0A = 16000000/(freq*64L) - 1;
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  TIMSK0 |= (1 << OCIE0A); 

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = -62;//initialize counter value to 0; change this value to change phase, -124 = 180, -62 = 90
//    OCR2A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  OCR2A = 16000000/(freq*64L) - 1;
//   Serial.println(OCR2A);
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22);   
  TIMSK2 |= (1 << OCIE2A);
  sei();//allow interrupts
}//end setup

void loop() {

}

void ReceiveEvent(int howmany)
{
  I2CReceive();
}

void I2CReceive()
{
  unsigned char temp[2], com;
//  unsigned char fbc_lower, fbc_upper, vmodoffset_upper, vmodoffset_lower;
  unsigned long t1,t2,t_delta;
  temp[0]=0;
  temp[1]=0;  
  while(Wire.available() == 3)
  {
    t1=micros();
    com=Wire.read();
    temp[0]=Wire.read();
    temp[1]=Wire.read();
    t2=micros();
    t_delta=t2-t1;//
  }
  
 if(t_delta<500) 
 { 
  switch(com)
  {
    case I2C_MOD_FREQ:
      g_freq = temp[0]<<8 | temp[1];
//      Serial.println(temp[0]);
//      Serial.println(temp[1]);
      Serial.print("g_freq: ");
      Serial.println(g_freq);
    break;  
    case I2C_MOD_PHASE:
      g_phase = temp[0]<<8 | temp[1];
      Serial.print("g_phase: ");
      Serial.println(g_phase);
    break;  
  }
 }

}
