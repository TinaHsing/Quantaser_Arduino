#include "Arduino.h"
#include <SPI.h>
#include <LTC1865.h>
#include <C12880_V01.h>


//********* clka , sta, clkb, stb must all in PORTD (arduino pin 0 ~7) ****************
C12880::C12880()
{

}

bool C12880::SpectroInit(unsigned char clka, unsigned char sta, unsigned char clkb, unsigned char stb, unsigned char adcconv, unsigned char adc_cha)
{
  guc_clka = clka;
  guc_clkb = clkb;
  guc_sta = sta;
  guc_stb = stb;
  guc_clka_high = 1<< clka;
  guc_clka_low = ~guc_clka_high;
  guc_clkb_high = 1 << clkb;
  guc_clkb_low = ~guc_clkb_high;
  guc_sta_high = 1<< guc_sta;
  guc_stb_high = 1<< guc_stb;
    
  guc_stab_high = guc_sta_high | guc_stb_high;
  guc_clkab_high = guc_clka_high | guc_clkb_high;
  guc_clkab_low = ~guc_clkab_high;
  guc_adc_cha = adc_cha;
  

  

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  adc.init(adcconv, adc_cha);
  
  if (guc_sta>7 || guc_stb>7 || guc_clka > 7 || guc_clkb >7)
    return 1;
  pinMode(guc_clka, OUTPUT);
	pinMode(guc_sta, OUTPUT);
	pinMode(guc_clkb, OUTPUT);
	pinMode(guc_stb, OUTPUT);
	digitalWrite(guc_clka, LOW);
	digitalWrite(guc_sta, LOW);
	digitalWrite(guc_clkb, LOW);
	digitalWrite(guc_stb, LOW);

  return 0;
}

void C12880::PulseClkA(unsigned long pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		
    PORTD |= guc_clka_high;
    PORTD &= guc_clka_low;	
	}
}

void C12880::PulseClkB(unsigned long pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		
    PORTD |= guc_clkb_high;
    PORTD &= guc_clkb_low;
	}

}

void C12880::PulseClkAB(unsigned long pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		
    PORTD |= guc_clkab_high;
    PORTD &= guc_clkab_low;
		
	}
}

void C12880::StartIntegAB()
{
	PORTD |= guc_stab_high;
}

void C12880::StopIntegA()
{
	PORTD &= ~guc_sta_high;
}

void C12880::StopIntegB()
{
	PORTD &= ~guc_stb_high;
}

void C12880::ReadVedioAB()
{
	unsigned int i, data;
  unsigned char low, high;
	for (i=0; i < CHANNEL_NUMBER; i++)
	{
    data = adc.Read(!guc_adc_cha);
    low = (unsigned char)(data);
    high = data >>8;
    #if HEXMODE
    Serial.write(low);
    Serial.write(high);
    # endif
    # if NONHEX
    Serial.print(data);
    Serial.print(", ");
    # endif
		// read B
	  data = adc.Read(guc_adc_cha);
    low = (unsigned char) (data); 
    high = data >>8;
    #if HEXMODE
    Serial.write(low);
    Serial.write(high);
    # endif
    # if NONHEX
    Serial.println(data);
    # endif
		PulseClkAB(1);
	}
}


void C12880::RunDevice(unsigned long I_timeA, unsigned long I_timeB)
{
  #if TIMEMODE
  unsigned long t1 = 0, t2 = 0, t3=0, t4 =0, t5 =0;
  #endif
  unsigned long I_timeBothAB, P_timeBothAB;
  uint8_t ucFlagAB;
  #if TIMEMODE
  t1= micros(); 
  #endif
  if (I_timeA == I_timeB)
  {
    I_timeBothAB = I_timeA;
  	ucFlagAB = ABSameTime;
  }
  else if (I_timeB > I_timeA)
  {
    I_timeBothAB = I_timeA;
    I_timeB -= I_timeA;
    ucFlagAB = BTimeBig2A;
  }
  else // if (I_timeA > I_timeB)
  {
    I_timeBothAB = I_timeB;
    I_timeA -= I_timeB;
    ucFlagAB = ATimeBig2B;
  }

  PulseClkAB(2);
  StartIntegAB();
  #if TIMEMODE
  t2= micros(); 
  #endif
  PulseClkAB(I_timeBothAB);

  if (ucFlagAB == ABSameTime)
  {
    StopIntegA();
    StopIntegB();
    #if TIMEMODE
    t3= micros();
    t4= micros();  
    #endif
    PulseClkAB(PAUSE_NUMBER);
  }
  else if (ucFlagAB == BTimeBig2A)
  {
    StopIntegA();
    #if TIMEMODE
    t3= micros(); 
    #endif

    if (I_timeB < PAUSE_NUMBER)
    {
      PulseClkAB(I_timeB);
      StopIntegB();
      #if TIMEMODE
      t4= micros(); 
      #endif
      P_timeBothAB = PAUSE_NUMBER - I_timeB;
      PulseClkAB(P_timeBothAB);
      PulseClkB(I_timeB);
    }
    else // if (I_timeB > PAUSE_NUMBER)
    {
      PulseClkAB(PAUSE_NUMBER);
      I_timeB -= PAUSE_NUMBER;
      PulseClkB(I_timeB);
      StopIntegB();
      #if TIMEMODE
      t4= micros(); 
      #endif
      PulseClkB(PAUSE_NUMBER);
    }
  }
  else // if (ucFlagAB == ATimeBig2B)
  {
    StopIntegB();
    #if TIMEMODE
    t3= micros(); 
    #endif
    if (I_timeA < PAUSE_NUMBER)
    {
      PulseClkAB(I_timeA);
      StopIntegA();
      #if TIMEMODE
      t4= micros(); 
      #endif
      P_timeBothAB = PAUSE_NUMBER - I_timeA;
      PulseClkAB(P_timeBothAB);
      PulseClkA(I_timeA);
    }
    else // if (I_timeA > PAUSE_NUMBER)
    {
      PulseClkAB(PAUSE_NUMBER);
      I_timeA -= PAUSE_NUMBER;
      PulseClkA(I_timeA);
      StopIntegA();
      #if TIMEMODE
      t4= micros(); 
      #endif
      PulseClkA(PAUSE_NUMBER);
    }
  }

  
  ReadVedioAB();

#if TIMEMODE
  t5 = micros();
  Serial.print("start PROG:");
  Serial.println(t1);
  Serial.print("Start INIT:");
  Serial.println(t2);
  Serial.print("stop 1st:");
  Serial.println(t3);
  Serial.print("stop 2nd:");
  Serial.println(t4);
  Serial.print("finished:");
  Serial.println(t5);
#endif

}


