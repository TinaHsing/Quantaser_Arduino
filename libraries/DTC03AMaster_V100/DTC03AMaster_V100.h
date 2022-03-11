#include <Wire.h>
#include <openGLCD.h>
#include <EEPROM.h>
#include <Wire.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Iain5x7.h>
#include <fonts/fixed_bold10x15.h>
#include <avr/pgmspace.h>
#include <DTC03A_MS.h>

//========Frequently update paramter===========
#define DEBOUNCETIME			2			//debounceing time(ms) for ENC
#define LONGPRESSTIME			1000
#define BLINKDELAY				350
#define HIGHLOWBOUNDRY			500			//
#define CURSORSTATE_STAYTIME	700
#define ACCUMULATE_TH			50
#define DEBOUNCE_WAIT			ACCUMULATE_TH * 4
//=================pin definition=========================
// #define PUSH_ENABLE				0
// #define ENC_SW					2
// #define ENC_A					3
// #define ENC_B					1
#define ENC_B						3
#define ENC_A						2
#define ENC_SW						A7
#define PUSH_ENABLE					A6

//-----------EEPROM ADDRESS---------
#define EEADD_BCONST_UPPER		0x00
#define EEADD_BCONST_LOWER		0x01
#define EEADD_VSET_UPPER		0x02
#define EEADD_VSET_LOWER		0x03
#define EEADD_Ilim_UPPER		0x04
#define EEADD_Ilim_LOWER		0x05
#define EEADD_Vlim_UPPER		0x06
#define EEADD_Vlim_LOWER		0x07
#define EEADD_K_UPPER			0x08
#define EEADD_K_LOWER			0x09
#define EEADD_Ti_UPPER			0x0A
#define EEADD_Ti_LOWER			0x0B
#define EEADD_Td_UPPER			0x0C
#define EEADD_Td_LOWER			0x0D
#define EEADD_HiLimit_UPPER		0x0E
#define EEADD_HiLimit_LOWER		0x0F
#define EEADD_LoLimit_UPPER		0x10
#define EEADD_LoLimit_LOWER		0x11
#define EEADD_AutoType			0x12
#define EEADD_AutoDelya_UPPER	0x13
#define EEADD_AutoDelya_LOWER	0x14
#define EEADD_DUMMY				0x64

//----------NOEE Default value------
#define NOEE_DUMMY				104
#define NOEE_BCONST				3988
#define NOEE_VSET				26214		// 25C
#define NOEE_ILIM				204			// I_LIM = 1A
#define NOEE_VLIM				589			// V_LIM = 6V
#define NOEE_K					10
#define NOEE_Ti					0			// OFF
#define NOEE_Td					0			// OFF
#define NOEE_HiLimit			840			// PWM Full
#define NOEE_LoLimit			0			// OFF
#define NOEE_AutoType			Autotune_P
#define NOEE_AutoDelta			42			// PWM Full * 0.5

//=====================BG print coordinate definition=========
#define TSET_COORD_X			0
#define TSET_COORD_Y			0
#define TSET_COORD_X2			18//16
#define Text_SET				"SET"
#define TACT_COORD_X			0
#define TACT_COORD_Y			ROWPIXEL0507 * 3
#define TACT_COORD_X2			18
#define Text_ACT				"ACT"
#define ITEC_COORD_X			COLUMNPIXEL0507
#define ITEC_COORD_Y			ROWPIXEL0507 * 6
#define ITEC_COORD_X2			COLUMNPIXEL0507 * 6
#define Text_ITEC				"ITEC:"
#define ILIM_COORD_X			COLUMNPIXEL0507
#define ILIM_COORD_Y			ROWPIXEL0507 * 7
#define ILIM_COORD_X2			COLUMNPIXEL0507 * 7
#define Text_ILIM				"ILIM:"
#define P_COORD_X				COLUMNPIXEL0507 * 14
#define P_COORD_Y				ROWPIXEL0507 * 3
#define P_COORD_X2				COLUMNPIXEL0507 * 16
#define Text_P					"P:"
#define I_COORD_X				COLUMNPIXEL0507 * 14
#define I_COORD_Y				ROWPIXEL0507 * 4
#define I_COORD_X2				COLUMNPIXEL0507 * 16
#define Text_I					"I:"
#define BCONST_COORD_X			COLUMNPIXEL0507 * 14
#define BCONST_COORD_Y			ROWPIXEL0507 * 5
#define BCONST_COORD_X2			COLUMNPIXEL0507 * 16
#define Text_B					"B:"
#define ATUNE_DELTA_COORD_X		COLUMNPIXEL0507 * 14
#define ATUNE_DELTA_COORD_Y		ROWPIXEL0507 * 6
#define ATUNE_DELTA_COORD_X2	COLUMNPIXEL0507 * 18
#define Text_ATUNE_DELTA		"AdT:"
#define ATUNE_COORD_X			COLUMNPIXEL0507 * 14
#define ATUNE_COORD_Y			ROWPIXEL0507 * 7
#define ATUNE_COORD_X2			COLUMNPIXEL0507 * 17
#define Text_AT					"AT:"

// define GLCD parameter
#define COLUMNPIXEL1015			11			//column pixels of fixed_bold10x15
#define COLUMNPIXEL0507			6			//column pixels of SystemFont5x7
#define ROWPIXEL0507			8			//row pixels of SystemFont5x7

// define I2C parameter
#define I2CSENDDELAY			100			//delay100us
#define I2CREADDELAY			100			//delay100us

//define calculation parameter
#define T0INV					0.003354
#define RTHRatio				26214
#define Bin_To_Itec				0.002434716796875	// (g_I_Tec * (3.022 * 3.3V)/(4096(12bit ADC)) - 5
#define Bin_To_Ilim				0.0048828125		// g_I_Lim * (5/1024(10bit DAC))
#define Ilim_To_Bin				204.8				// g_I_Print * (1024(10bit DAC)/5)
#define PI_Freq					1000				// Timer frequency = 1KHz

class DTC03Master
{
public:
	DTC03Master();
	void SetPinMode();
	void ParamInit();
	void ReadEEPROM();
	void SaveEEPROM();
	void I2CWriteAll();
	void I2CWriteData(unsigned short Command , unsigned short Data);
	void I2CReadData(unsigned short Command);
	void UpdateEnable();
	void CheckStatus();
	void UpdateParam();
	void WelcomeScreen();
	void BackGroundPrint();
	void PrintTset();
	void PrintTact(double tact);
	void PrintItec(double itec);
	void PrintIlim();
	void PrintK();
	void PrintTi();
	void PrintB();
	void PrintAtune();
	void PrintAtuneDone();
	void PrintNormalAll();
	void CursorState();
	void HoldCursortate();
	void blinkTsetCursor();
	void ShowCursor(unsigned char);
	void Encoder();

	//working variable-------------------
	bool g_Temp_Sensor_Mode;
	short g_V_Tec;
	short g_I_Tec;
	unsigned char g_Remote;
	unsigned char g_IO_State;
	unsigned char g_PID_Mode;
	unsigned char g_Auto_Type;
	unsigned char g_Auto_Delta;
	unsigned short g_B_Const;
	unsigned short g_V_Set;
	unsigned short g_V_Act;
	unsigned short g_V_Lim;
	unsigned short g_I_Lim;
	unsigned short g_K;
	unsigned short g_Ti;
	unsigned short g_Td;
	unsigned short g_HiLimit;
	unsigned short g_LoLimit;

	double g_T_Set;
	double g_I_Print;

	bool g_atune_status, g_runTimeflag, g_lock_flag;
	unsigned char g_cursorstate;
	//------------------------------------
	unsigned short g_pid_mode;

private:
	double ReturnTemp(unsigned int vact);
	unsigned int ReturnVset(double tset);
	unsigned char QCP0_CRC_Calculate(unsigned char *pData, unsigned char Length);
	void QCP0_Package(unsigned char RorW, unsigned short Command, unsigned short Data, unsigned char *pData);
	void QCP0_Unpackage(unsigned char *pData, unsigned char *RorW, unsigned short *Command, unsigned short *Data);
	void QCP0_REG_PROCESS(unsigned short Command, unsigned short Data);

	glcd lcd;
	bool g_EncodeDir;
	unsigned int p_cursorStateCounter[3], p_cursorStayTime;
	unsigned int p_tBlink, p_tcursorStateBounce, p_holdCursorTimer;
	unsigned char p_ee_change_state;
	bool g_paramupdate, p_tBlink_toggle, p_blinkTsetCursorFlag;
	bool p_ee_changed, p_HoldCursortateFlag, p_timerResetFlag, p_atunProcess_flag;
	unsigned long g_tenc, p_loopindex;
	double g_tsetstep;
};
