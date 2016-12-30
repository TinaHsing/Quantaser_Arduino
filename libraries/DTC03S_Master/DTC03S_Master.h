//------------------I2C Command and Address-----------
//						b15 b14 b13 b12 b11 b10 b09 b08 b07 b06 b05 b04 b03 b02 b01 b00
// Address: VSET 0x01 	-----------------------------VSET------------------------------
// Address: VACT 0x02   -----------------------------VACT------------------------------
// Address: 
#include <openGLCD.h>
#include <EEPROM.h>
#include <DTC03_MS.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <fonts/SystemFont5x7.h>
// #include <fonts/Iain5x7.h>
#include <fonts/fixed_bold10x15.h>


//------constantly change parameter---------
#define BVALUE 3988
#define ILIMINDEX 50 // currentl limit = 0.5+ 50*0.05 = 3
#define PGAIN	15 
#define DEFAULTR1 15	// 1.5 ohm
#define DEFAULTR2 30	// 3.0 ohm
#define DEFAULTTPIDOFF 2 // tpidoffset 2000
#define KI 176
#define LS 20

//-------constant definition -----
#define T0INV 			0.003354
#define RTHRatio 		25665 
#define ANAREADVIL 		240
#define ANAREADVIH 		500
#define LONGPRESSTIME 	5000
#define PERIOD			100
#define MAXRATEINDEX	20 
#define DEBOUNCETIME 	2
#define COUNTERINCRE	50
#define COUNTERSPEEDUP	100
#define FINETUNEAMP 3
#define SCANSAMPLERATE 40

//------pin definition ----------------
#define ENC_A 2
#define ENC_B 3
#define SCANB 11 // MOSI
#define PUSHB A7
#define ENSW A6

//-----------EEPROM ADDRESS---------
#define EEADD_VSTART_UPPER	0
#define EEADD_VSTART_LOWER	1
#define EEADD_VEND_UPPER	2
#define EEADD_VEND_LOWER	3
#define EEADD_RATE_INDEX	4
#define EEADD_DUMMY			5
#define EEADD_FBC_UPPER		6
#define EEADD_FBC_LOWER		7
#define EEADD_P             8
#define EEADD_KIINDEX	    9


//----------NOEE Default value------
#define NOEE_VSTART		53244
#define NOEE_VEND		9313
#define NOEE_RATE		1
#define NOEE_DUMMY		104
#define NOEE_FBC		22000
#define NOEE_KI			0
#define NOEE_LS			0
#define NOEE_P          10
#define NOEE_kiindex    0

#define TEMP_RISING 	1
#define TEMP_FALLING 	0


//-----------Font Parameters---------
#define ROWPIXEL0507 8 //row pixel of SystemFont5x7
#define COLUMEPIXEL0507 6  

//----------Print Coordinate ---------
#define TSTART_COORD_X 	6
#define TSTART_COORD_Y 	0
#define TSTART_COORD_X2	TSTART_COORD_X+42

#define TEND_COORD_X	6
#define TEND_COORD_Y	12
#define TEND_COORD_X2	TEND_COORD_X+42

#define RATE_COORD_X	6
#define RATE_COORD_Y	24
#define RATE_COORD_X2	RATE_COORD_X+42

#define TFINE_COORD_X	72
#define TFINE_COORD_Y	36

#define TACT_COORD_X	6
#define TACT_COORD_Y	36

#define EN_COORD_X		6
#define EN_COORD_Y		56
#define EN_COORD_X2 	42

#define ENG_X		6
#define ENG_Y		0

#define P_X		6
#define P_Y		12
#define P_X2 	P_X+12

#define I_X		6
#define I_Y		24
#define I_X2 	I_X+12



#define SCAN_COORD_X	70
#define SCAN_COORD_Y	56

#define LOOPT_X	84
#define LOOPT_Y	0


const PROGMEM unsigned char RateTable[]
{
	0,		// dummy index
	1,		//index =1,	0.01k/s
	2,		//index =2, 
	3,		//index =3, 
	4,		//index =4, 
	5,		//index =5, 
	6, 		//index =6, 
	7,		//index =7, 
	8,		//index =8, 
	9, 		//index =9, 
	10,		//       10, 0.1 
	20,		//       11, 0.2
	30,		//       12, 0.3
	40,		//       13, 0.4
	50,		//       14, 0.5
	60,		//       15, 0.6
	70,		//       16, 0.7
	80,		//       17, 0.8
	90,		//       18, 0.9
	100,	//       19, 1.0
	200,	//       20, 2.0
};

// Rate definition in the unit of degree/sec // in the unit of m degree/ 100ms 
// 0 	0 		0
// 1 	0.01 	1
// 2 	0.02 	2
// 3 	0.05 	5
// 4	0.1 	10
// 5 	0.2 	20
// 6	0.5 	50
// 7 	1 		100
// 8	2 		200



class DTC03SMaster{
public:
	DTC03SMaster();
	void SetPinMode();
	void ParamInit();
	void WelcomeScreen();
	void ReadEEPROM();
	void I2CReadVact();
	void I2CWriteData(unsigned char com);
	float ReturnTemp(unsigned int vact, bool type);
	unsigned int ReturnVset(float tset, bool type);
	void PrintBG();
	void PrintTstart();
	void PrintTend();
	void PrintRate();
	void PrintScan();
	void PrintTnow();
	void PrintEnable();
	void PrintTact(float tact);
	void PrintFbcbase();
	void PrintKi();
	void PrintP();
	void PrintEngBG();
	void CheckVact();
	void CalculateRate();
	void CheckStatus();
	void checkTnowStatus();
	void CheckScan();
	void UpdateEnable();
	void ShowCursor();
	void UpdateParam();
	void Encoder();
	void Printloopt(unsigned long);
	void SaveEEPROM();
	void RuntestI2C();
	bool g_en_state;
	unsigned int g_vact;
	unsigned long g_tloop;



private:
	glcd lcd;
	bool g_scan, g_heater, g_paramterupdate, p_en[2], p_scan[2], p_tnow_flag[2], p_curstatus0flag, p_rateflag, p_EngFlag;
	bool p_ee_changed;
	char g_counter, g_counter2;
	unsigned char g_rateindex, g_trate, g_cursorstate,g_oldcursorstate, g_lastencoded, g_kiindex, g_p, p_ee_change_state;
	unsigned int  g_fbcbase, g_vstart, g_vset, g_vend, p_loopcount, p_trate;
	unsigned long g_timer, g_tenc[3], g_tscan, p_tlp[5];
	float g_tstart, g_tend, g_tnow, g_tfine, p_rate;

};
