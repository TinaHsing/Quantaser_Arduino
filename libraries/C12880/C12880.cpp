#include "Arduino.h"

#include <C12880.h>
#include <LTC1865.h>


C12880::C12880()
{}

void C12880::SpectroInit()
{
	pinMode (PIN_CLK, OUTPUT);
	pinMode (PIN_STA, OUTPUT);
	pinMode(PIN_STB, OUTPUT);
	digitalWrite(PIN_CLK, HIGH);
	digitalWrite(PIN_STA, LOW);
	digitalWrite(PIN_STB, LOW);
}
void C12880::LEDInit(unsigned char led1, unsigned char led2)
{
	guc_led1 = led1;
	guc_led2 = led2;
	pinMode (guc_led1, OUTPUT);
	pinMode (guc_led2, OUTPUT);
	digitalWrite(guc_led1, LOW);
	digitalWrite(guc_led2, LOW);
}

void C12880::StartLED(unsigned int delaytime)
{
	digitalWrite(guc_led1,HIGH);
	digitalWrite(guc_led2,HIGH);
	delayMicroseconds(delaytime);
}
void C12880::PulseClkA(unsigned int pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		PORTC &= CLK_LOW;
		PORTC |= CLK_HIGH;
	}	
}
void C12880::StartIntegA()
{
	PORTC |= STA_HIGH; 
}
void C12880::StopIntegA()
{
	PORTC &=  ~STA_HIGH;
}

void C12880::ReadVedioA(int *buffer)
{	
	ADMUX = ADC_READA;
	unsigned int i, low, high;
	for (i=0; i < CHANNEL_NUMBER; i++)
	{
		ADCSRA |= B01000000;
		low = ADCL;
		high = ADCH;
		buffer[i] = (high << 8) | low;
		PulseClkA(1);
	}
}





