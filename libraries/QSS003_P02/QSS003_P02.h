#ifndef QSS003_P02_H
#define QSS003_P02_H 


// QSS003 parameters
#define LEDA 1
#define LEDB 2
#define LEDC 3
#define CYCP 1.8
#define MAXFILE 800
#define MASK 0b1111111100000000

//---- Pin Definition for QSS003_P02 ----///
#define MODE_SD_USB A0 		// USB and SD mode switch
#define LEDM 9				// measurement LED indicator
#define SWM 8				// mwasurement control switch
#define CLKA 2				// C12880A clock 
#define STA 5				// C12880A measure
#define CLKB 3				// C12880B clock
#define STB 6				// C12880B measure
#define ADCCONV 7			// ADC chip select pin
#define ADC_CHA 0			// ADC Channel number
#define SD_CS 4 			// SD card chip select pin

// command 
#define COM_I1 "I1"
#define COM_I2 "I2"
#define COM_I3 "I3"
#define COM_Ta "Ta"
#define COM_Tb "Tb"
#define COM_Tw "Tw"
#define COM_Rp "Rp"

//----------NOEE Default value------
#define NOEE_DUMMY	104
#define NOEE_I1		0		
#define NOEE_I2		0
#define NOEE_I3		0
#define NOEE_RP		1
#define NOEE_TA		1000
#define NOEE_TB		1000
#define NOEE_TW		1000

//-----------EEPROM ADDRESS---------
//two byte
#define EEADD_I1_UPPER	0
#define EEADD_I1_LOWER	1
#define EEADD_I2_UPPER	2
#define EEADD_I2_LOWER	3
#define EEADD_I3_UPPER	4
#define EEADD_I3_LOWER	5
#define EEADD_RP_UPPER	6
#define EEADD_RP_LOWER	7
//three byte
#define EEADD_TA_UPPER	8
#define EEADD_TA_MIDDLE	9
#define EEADD_TA_LOWER	10
#define EEADD_TB_UPPER	11
#define EEADD_TB_MIDDLE	12
#define EEADD_TB_LOWER	13
#define EEADD_TW_UPPER	14
#define EEADD_TW_MIDDLE	15
#define EEADD_TW_LOWER	16
//noee
#define EEADD_DUMMY		100


class LTC2615;

class QSS003 
{
public:
	QSS003();
	void Initialize(void);
	void checkParam();
	void currentOut(unsigned char ch, unsigned int cur);
	void SaveEEPROM();
    void ReadEEPROM();
	void printVar();
	
	unsigned long us2cyc(unsigned long t_int);
	unsigned long gul_ta, gul_tb, gul_tw;
	unsigned int gui_i1, gui_i2, gui_i3, gui_rp;
private:
	LTC2615 ltc2615;

	bool p_ee_changed;
	unsigned char p_ee_change_state;

};
#endif






