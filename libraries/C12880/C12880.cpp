#include "Arduino.h"

#include <C12880.h>



C12880::C12880()
{}

void C12880::SpectroInit()
{
	pinMode(PIN_CLKA, OUTPUT);
	pinMode(PIN_STA, OUTPUT);
	pinMode(PIN_CLKB, OUTPUT);
	pinMode(PIN_STB, OUTPUT);
	digitalWrite(PIN_CLKA, HIGH);
	digitalWrite(PIN_STA, LOW);
	digitalWrite(PIN_CLKB, HIGH);
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

void C12880::PulseClkA(unsigned long pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		PORTC |= CLKA_HIGH;
		PORTC &= CLKA_LOW;
	}
}

void C12880::PulseClkB(unsigned long pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		PORTC |= CLKB_HIGH;
		PORTC &= CLKB_LOW;
	}
}

void C12880::PulseClkAB(unsigned long pulse)
{
	unsigned int i;
	for (i = 0; i < pulse; i++)
	{
		PORTC |= CLKAB_HIGH;
		PORTC &= CLKAB_LOW;
	}
}

void C12880::StartIntegAB()
{
	PORTC |= STA_HIGH;
	PORTC |= STB_HIGH; 
}

void C12880::StopIntegA()
{
	PORTC &= ~STA_HIGH;
}

void C12880::StopIntegB()
{
	PORTC &= ~STB_HIGH;
}

#if 0	//only read 1
void C12880::ReadVedioA(int *buffer)
{	
	ADMUX = ADC_READA;
	unsigned int i, low, high;
	for (i=0; i < CHANNEL_NUMBER; i++)
	{
		ADCSRA |= B01000000;
		while(ADCSRA & B01000000);
		low = ADCL;
		high = ADCH;
		buffer[i] = (high << 8) | low;
		PulseClkA(1);
	}
}
#endif

#if 0	//2 for int
void C12880::ReadVedioAB(int *buffer)
{	
	unsigned int i, low, high;
	for (i=0; i < CHANNEL_NUMBER*2; i++)
	{
#if 1
		//read A
		ADMUX = ADC_READA;
		ADCSRA |= B01000000;
		while(ADCSRA & B01000000);
		low = ADCL;
		high = ADCH;
		buffer[i] = (high << 8) | low;
		//read B
		ADMUX = ADC_READB;
		ADCSRA |= B01000000;
		while(ADCSRA & B01000000);
		low = ADCL;
		high = ADCH;
		buffer[++i] = (high << 8) | low;
#else	//debug
		buffer[i] = analogRead(ADC_READA);
		buffer[++i] = analogRead(ADC_READB);
#endif
		PulseClkAB(1);
	}
}
#endif

#if 1	//2 for byte
void C12880::ReadVedioAB(byte *buffer)
{
	unsigned int i, low, high, num = 0;
	for (i=0; i < CHANNEL_NUMBER*4; i++)
	{
		// read A
		ADMUX = ADC_READA;
		ADCSRA |= B01000000;
		while(ADCSRA & B01000000);
		low = ADCL;
		high = ADCH;
		buffer[i] = low;
		buffer[++i] = high;
		num++;
		// read B
		ADMUX = ADC_READB;
		ADCSRA |= B01000000;
		while(ADCSRA & B01000000);
		low = ADCL;
		high = ADCH;
		buffer[++i] = low;
		buffer[++i] = high;
		num++;
		PulseClkAB(1);
	}
}
#endif




