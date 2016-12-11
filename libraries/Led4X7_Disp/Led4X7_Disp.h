#ifndef LED4X7_DISP_H
#define LED4X7_DISP_H
#include <Arduino.h>

class led4X7_disp //use PORTD pins for p_pinA~D; PORTB for pin0~3 and pinh
{
	private:
	
		uint8_t p_gain, p_BCD_mask, p_PORTB_mask;		
		uint8_t p_pos_pins[4], p_BCD_pins[4], p_pinh;
		unsigned int p_adcbase;		
		void get_mask();
		void SetDisplay(uint8_t, uint8_t);
		static const bool p_data[10][4];							
		
	public:	
	
		led4X7_disp(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t); 		
		void print(unsigned int);
		void init(uint8_t, uint8_t); //uint8_t : ADC bit; uint8_t : gain
		
		
};


#endif
