#include <Wire.h>
#include "QSS015_cmn.h"

//for integrator usage 
#define S1 6
#define S2 5
#define S3 4
#define S4 3
#define INJECTION_CHARGE_TIME 5
#define NEGATIVE_SAMPLING 1

#define DEGUG 0

unsigned long g_int_time = 100*1000;

void setup() {
	//for integrator usage 
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(S4, OUTPUT);
	digitalWrite(S3, LOW);
	digitalWrite(S4, LOW);
	reset(30);
  Wire.begin(SLAVE_MCU_I2C_ADDR);
  Wire.onReceive(I2CReceive);
}

void loop() {

	while(1)
	{
		/////// integrate process /////
		reset(30);//不得<30, int hold end
		hold(10);
		integrate(g_int_time);
    hold(10);
	}
  
}

void reset(int wait) //10
{
  PORTD = ((PORTD & B10011111) | (1<<S1)); 
  delayMicroseconds(wait);
}

void hold(int wait) //11
{
  PORTD = ((PORTD & B10011111) | (1<<S1) | (1<<S2)); 
  delayMicroseconds(wait);
}

#if 0
void hold_sample(int wait) //11
{
//  PORTD = ((PORTD & B10001111) | (1<<S1) | (1<<S2) | (1<<S3)); //start sampling ad620 positive input
  PORTD = ((PORTD & B10000111) | (1<<S1) | (1<<S2) | (1<<S4)); //start sampling ad620 negative input
  delayMicroseconds(wait);
  PORTD = (PORTD & B11100111) ; // //stop sampling ad620 negative input
}
#endif

void integrate(unsigned long wait) //01
{
  unsigned int bg;
  PORTD = ((PORTD & B10011111) | (1<<S2)); //int start
//  delayMicroseconds(INJECTION_CHARGE_TIME); 
//  PORTD = ((PORTD & B11100111) | (1<<S3) ); //start sampling ad620 positive input
//  delayMicroseconds(NEGATIVE_SAMPLING); 
//  PORTD = (PORTD & B11100111); //stop sampling ad620 positive input
//  delay(wait);
  delayMicroseconds(wait);
}

void I2CReceive()
{
  unsigned char temp[4], com;
//  unsigned char fbc_lower, fbc_upper, vmodoffset_upper, vmodoffset_lower;
  unsigned long t1,t2,t_delta;

  for (int i = 0; i < 4; i++)
  {
    temp[i]=0;
  }

  while(Wire.available() == 1)
  {
    t1=micros();
    com=Wire.read();
    temp[0]=Wire.read();
    temp[1]=Wire.read();
    temp[2]=Wire.read();
    temp[3]=Wire.read();
    t2=micros();
    t_delta=t2-t1;//
  }

 if(t_delta<500) 
 { 
  switch(com)
  {
    case I2C_SEND_INT:
      g_int_time = temp[0]<<24 | temp[1]<<16 | temp[2]<<8 | temp[3];
#if DEBUG
      I2CWriteData(I2C_GET_INT);
#endif
    break;
  }
 }
}

void I2CWriteData(unsigned char com)
{
  unsigned char temp[2];
  switch (com)
  {
    case I2C_GET_INT:
      temp[0] = g_int_time >> 24;
      temp[1] = g_int_time >> 16;
      temp[2] = g_int_time >> 8;
      temp[3] = g_int_time;
      break;
  }

  Wire.write(com);//
  Wire.write(temp, 2);//
  Wire.endTransmission();//
  delayMicroseconds(I2CSENDDELAY);//
}
