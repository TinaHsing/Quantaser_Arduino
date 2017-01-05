#ifndef LED4X7_DISP_H
#define LED4X7_DISP_H
#include <Arduino.h>

#define AVGTIMES 16
#define AVGPOWER 4 //2^AVGPOWER = AVGTIMES

class led4X7_disp //use PORTD pins for p_pinA~D; PORTB for pin0~3 and pinh
{
	private:
	
		uint8_t p_BCD_mask, p_PORTB_mask;		
		uint8_t p_pos_pins[4], p_BCD_pins[4], p_pinh, p_pinai;
		uint8_t p_avgcount; 
		float p_gain;
		unsigned int p_adcbase, p_vinarray[AVGTIMES], p_vinsum;		
		void get_mask();
		void SetDisplay(uint8_t, uint8_t);
		static const bool p_data[10][4];	
		bool p_delayflag;
		unsigned long p_delayStart;						
		
	public:	
	
		led4X7_disp(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t); 		
		void print(unsigned int);
		void print();
		void init(uint8_t, float); //uint8_t : ADC bit; float : gain
		void init(uint8_t,uint8_t, float); //arduino AI oin, ADC bit, gain
		unsigned int vout(unsigned int);
		
		
};


#endif
