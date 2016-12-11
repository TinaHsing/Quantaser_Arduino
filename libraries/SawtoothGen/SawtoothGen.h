#ifndef SAWTOOTHGEN_H
#define SAWTOOTHGEN_H
#include <Arduino.h>
class SawtoothGen 
{
	private :
		uint8_t p_refsense, p_outputsense, p_chargecontrol;
		unsigned int p_minvolt, p_maxvolt;
		bool p_compflag;		
		
	public :
		SawtoothGen(uint8_t, uint8_t, uint8_t, float, float) ;
		void Sawtooth_out();
	
};



#endif 
