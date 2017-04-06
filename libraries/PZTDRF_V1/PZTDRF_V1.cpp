#include <PZTDRF_V1.h>

PZTDRF::PZTDRF(uint8_t v_status, uint8_t dcbias_sw, uint8_t drv2700_en)
{
	p_voltStatusPin = v_status; //AI
	p_dcbiasSw = dcbias_sw; // dcbias analog switch control pin
	p_drv2700En = drv2700_en;//drv2700 EN pin
	pinMode(p_dcbiasSw, OUTPUT);
	pinMode(p_drv2700En, OUTPUT);	
	
}

void PZTDRF::init()
{
	digitalWrite(p_dcbiasSw, LOW);
	digitalWrite(p_drv2700En, LOW);
	p_powerOffFlag = 1;
}

void PZTDRF::PowerOn(float vth_on)
{	
	int vth;
	vth = (int) vth_on/3.0*1024/5;	
	while( !(analogRead(p_voltStatusPin)>vth) ) {};
	digitalWrite(p_drv2700En, HIGH);
	delay(1000);
	digitalWrite(p_dcbiasSw, HIGH);
}

void PZTDRF::PowerOff(float vth_off)
{
	int vth;
	if(p_powerOffFlag)	
	{
		vth = (int) vth_off/3.0*1024/5;	
		if( analogRead(p_voltStatusPin) <= vth )
		{
			digitalWrite(p_drv2700En, LOW);
			delay(5);
			digitalWrite(p_dcbiasSw, LOW);
			p_powerOffFlag = 0;
		}
		
	}
	
}

