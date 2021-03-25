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
	p_ee_changed = 0;
	
}
bool QSS003::checkParam()
{
	String command;
	long val;
	bool run = 0;

	if(Serial.available())
	{
		command = Serial.readStringUntil('=');
		val = Serial.parseInt();
		if (strcmp(command.c_str(), COM_St)==0)
		{
			QSS003::printVar();
		}
		else if (strcmp(command.c_str(), COM_Run)==0)
		{
			run = 1;
		}
		else if (strcmp(command.c_str(), COM_Tw)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_TW_UPPER;
			gul_tw = val;
		}
		else if (strcmp(command.c_str(),COM_Ta)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_TA_UPPER;
			gul_ta = val;
		}
		else if(strcmp(command.c_str(), COM_Tb)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_TB_UPPER;
			gul_tb = val;
		}
		else if(strcmp(command.c_str(), COM_I1)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_I1_UPPER;
			gui_i1 = val;
		}
		else if(strcmp(command.c_str(), COM_I2)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_I2_UPPER;
			gui_i2 = val;
		}
		else if(strcmp(command.c_str(), COM_I3)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_I3_UPPER;
			gui_i3 = val;
		}
		else if(strcmp(command.c_str(), COM_Rp)==0)
		{
			p_ee_changed = 1;
			p_ee_change_state = EEADD_RP_UPPER;
			gui_rp = val;
		}
	}
	return run;
}
void QSS003::currentOut(unsigned char ch, unsigned int cur)
{
	float volt, r_sense =0.5;

	volt = (float)cur/1000.0*r_sense;
	if (ch ==1){
		ltc2615.write(CH_G, volt);
	}
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
void QSS003::printVar()
{
	Serial.print(COM_I1);
	Serial.print("=");
	Serial.println(gui_i1);

	Serial.print(COM_I2);
	Serial.print("=");
	Serial.println(gui_i2);

	Serial.print(COM_I3);
	Serial.print("=");
	Serial.println(gui_i3);

	Serial.print(COM_Rp);
	Serial.print("=");
	Serial.println(gui_rp);

	Serial.print(COM_Ta);
	Serial.print("=");
	Serial.println(gul_ta);

	Serial.print(COM_Tb);
	Serial.print("=");
	Serial.println(gul_tb);

	Serial.print(COM_Tw);
	Serial.print("=");
	Serial.println(gul_tw);
}
void QSS003::ReadEEPROM()
{
	unsigned char noeedummy;
	noeedummy = EEPROM.read(EEADD_DUMMY);
	if(noeedummy == NOEE_DUMMY)
	{
		gui_i1 = (unsigned int)EEPROM.read(EEADD_I1_UPPER)<<8 | EEPROM.read(EEADD_I1_LOWER);
		gui_i2 = (unsigned int)EEPROM.read(EEADD_I2_UPPER)<<8 | EEPROM.read(EEADD_I2_LOWER);
		gui_i3 = (unsigned int)EEPROM.read(EEADD_I3_UPPER)<<8 | EEPROM.read(EEADD_I3_LOWER);
		gui_rp = (unsigned int)EEPROM.read(EEADD_RP_UPPER)<<8 | EEPROM.read(EEADD_RP_LOWER);		
		gul_ta = (unsigned long)EEPROM.read(EEADD_TA_UPPER)<<16 | ((EEPROM.read(EEADD_TA_MIDDLE)<<8)&MASK) | EEPROM.read(EEADD_TA_LOWER);
		gul_tb = (unsigned long)EEPROM.read(EEADD_TB_UPPER)<<16 | ((EEPROM.read(EEADD_TB_MIDDLE)<<8)&MASK) | EEPROM.read(EEADD_TB_LOWER);
		gul_tw = (unsigned long)EEPROM.read(EEADD_TW_UPPER)<<16 | ((EEPROM.read(EEADD_TW_MIDDLE)<<8)&MASK) | EEPROM.read(EEADD_TW_LOWER);
	}
	else
	{
		EEPROM.write(EEADD_DUMMY, NOEE_DUMMY);
		
		EEPROM.write(EEADD_I1_UPPER, NOEE_I1>>8);
		EEPROM.write(EEADD_I1_LOWER, NOEE_I1);
		EEPROM.write(EEADD_I2_UPPER, NOEE_I2>>8);
		EEPROM.write(EEADD_I2_LOWER, NOEE_I2);
		EEPROM.write(EEADD_I3_UPPER, NOEE_I3>>8);
		EEPROM.write(EEADD_I3_LOWER, NOEE_I3);
		EEPROM.write(EEADD_RP_UPPER, NOEE_RP>>8);
		EEPROM.write(EEADD_RP_LOWER, NOEE_RP);
		
		EEPROM.write(EEADD_TA_UPPER,	NOEE_TA>>16);
		EEPROM.write(EEADD_TA_MIDDLE,	NOEE_TA>>8);
		EEPROM.write(EEADD_TA_LOWER,	NOEE_TA);
		EEPROM.write(EEADD_TB_UPPER,	NOEE_TB>>16);
		EEPROM.write(EEADD_TB_MIDDLE,	NOEE_TB>>8);
		EEPROM.write(EEADD_TB_LOWER,	NOEE_TB);
		EEPROM.write(EEADD_TW_UPPER,	NOEE_TW>>16);
		EEPROM.write(EEADD_TW_MIDDLE,	NOEE_TW>>8);
		EEPROM.write(EEADD_TW_LOWER,	NOEE_TW);
		
		gui_i1 = NOEE_I1;
		gui_i2 = NOEE_I2;
		gui_i3 = NOEE_I3;
		gui_rp = NOEE_RP;
		gul_ta = NOEE_TA;
		gul_tb = NOEE_TB;
		gul_tw = NOEE_TW;
	}
}

void QSS003::SaveEEPROM()
{
	if (p_ee_changed==1) 
	{
		p_ee_changed = 0;
		switch(p_ee_change_state)
		{
            case EEADD_I1_UPPER:
				EEPROM.write(EEADD_I1_UPPER, gui_i1>>8 );
                EEPROM.write(EEADD_I1_LOWER, gui_i1);
				break;
			case EEADD_I2_UPPER:
				EEPROM.write(EEADD_I2_UPPER, gui_i2>>8 );
                EEPROM.write(EEADD_I2_LOWER, gui_i2);
				break;
			case EEADD_I3_UPPER:
				EEPROM.write(EEADD_I3_UPPER, gui_i3>>8 );
                EEPROM.write(EEADD_I3_LOWER, gui_i3);
				break;
			case EEADD_RP_UPPER:
				EEPROM.write(EEADD_RP_UPPER, gui_rp>>8 );
                EEPROM.write(EEADD_RP_LOWER, gui_rp);
				break;
			case EEADD_TA_UPPER:
				EEPROM.write(EEADD_TA_UPPER,	gul_ta>>16 );
                EEPROM.write(EEADD_TA_MIDDLE, 	gul_ta>>8);
				EEPROM.write(EEADD_TA_LOWER, 	gul_ta);
				break;
			case EEADD_TB_UPPER:
				EEPROM.write(EEADD_TB_UPPER,	gul_tb>>16 );
                EEPROM.write(EEADD_TB_MIDDLE, 	gul_tb>>8);
				EEPROM.write(EEADD_TB_LOWER, 	gul_tb);
				break;
			case EEADD_TW_UPPER:
				EEPROM.write(EEADD_TW_UPPER,	gul_tw>>16 );
                EEPROM.write(EEADD_TW_MIDDLE, 	gul_tw>>8);
				EEPROM.write(EEADD_TW_LOWER, 	gul_tw);
				break;
		}
	}
}
