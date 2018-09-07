#ifndef LTC2615_H
#define LTC2615_H
#include <Arduino.h>
#include <Wire.h>

#define ADD 0x10
#define CC 0b0011
#define ref 3.3
#define max 16383

#define CH_A 0b0000
#define CH_B 0b0001
#define CH_C 0b0010
#define CH_D 0b0011
#define CH_E 0b0100
#define CH_F 0b0101
#define CH_G 0b0110
#define CH_H 0b0111

class LTC2615 
{
	public:	
		LTC2615(); 
		void init();
		void write(uint8_t, float);//(CH_#, 0~3.3V)
		
	private:
		
		
		
};


#endif
