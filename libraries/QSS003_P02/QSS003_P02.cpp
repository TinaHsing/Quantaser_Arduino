#include <LTC2615.h>
#include <SD.h>
#include <EEPROM.h>
#include <QSS003_P02.h>

QSS003::QSS003()
{
}

void QSS003::Initialize()
{
	pinMode(SWM, INPUT);
	pinMode(LEDM, OUTPUT);
	pinMode(MODE_SD_USB, INPUT);
	digitalWrite(LEDM, LOW);
	ltc2615.init();
	currentOut(LEDA,0);
	currentOut(LEDB,0);
	currentOut(LEDC,0);
	
}
void QSS003::checkParam()
{
	String command;
	int val;
	if(Serial.available())
	{
		command = Serial.readStringUntil('=');
		val = val = Serial.parseInt();
		
		if (strcmp(command.c_str(), COM_Tw)==0)
			gul_tw = val;
		else if (strcmp(command.c_str(),COM_Ta)==0)
			gul_ta = val;
		else if(strcmp(command.c_str(), COM_Tb)==0)
			gul_tb = val;
		else if(strcmp(command.c_str(), COM_I1)==0)
			gui_i1 = val;
		else if(strcmp(command.c_str(), COM_I2)==0)
			gui_i2 = val;
		else if(strcmp(command.c_str(), COM_I3)==0)
			gui_i3 = val;
		else if(strcmp(command.c_str(), COM_Rp)==0)
			gui_rp = val;
	}
}
void QSS003::currentOut(unsigned char ch, unsigned int cur)
{
	float volt, r_sense =0.5;

	volt = (float)cur/1000.0*r_sense;
	if (ch ==1)
		ltc2615.write(CH_G, volt);
	else if (ch ==2)
		ltc2615.write(CH_F, volt);
	else if (ch ==3)
		ltc2615.write(CH_E, volt);
}
unsigned long QSS003::us2cyc(unsigned long t_int)
{
	unsigned long cyc;
	if (t_int < 90) t_int =90;
	cyc = (unsigned long)t_int/CYCP-48;
	return cyc;
}

