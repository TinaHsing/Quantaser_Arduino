#include <Wire.h>
#include <openGLCD.h>
#include <EEPROM.h>
#include <Wire.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/fixednums7x15_S.h>
#include <fonts/Arial_bold_14.h>
#include <avr/pgmspace.h>
#include <DTC03A_MS.h>

//========Frequently update paramter===========
#define DEBOUNCETIME			5			//debounceing time(ms) for ENC
#define ENC_SPEED_T				25			//rotate speed time(ms) for ENC
#define LONGPRESSTIME			1000
#define BLINKDELAY				350
#define HIGHLOWBOUNDRY			500			//
#define DEBOUNCE_WAIT			50
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
#define EEADD_BCONST_LOWER		0x00
#define EEADD_BCONST_UPPER		0x01
#define EEADD_I_Bias_IO			0x02
#define EEADD_I_Bias			0x03
#define EEADD_VSET_LOWER		0x04
#define EEADD_VSET_UPPER		0x05
#define EEADD_Ilim_LOWER		0x06
#define EEADD_Ilim_UPPER		0x07
#define EEADD_Vlim_LOWER		0x08
#define EEADD_Vlim_UPPER		0x09
#define EEADD_K_LOWER			0x0A
#define EEADD_K_UPPER			0x0B
#define EEADD_Ti_LOWER			0x0C
#define EEADD_Ti_UPPER			0x0D
#define EEADD_Td_LOWER			0x0E
#define EEADD_Td_UPPER			0x0F
#define EEADD_HiLimit_LOWER		0x10
#define EEADD_HiLimit_UPPER		0x11
#define EEADD_LoLimit_LOWER		0x12
#define EEADD_LoLimit_UPPER		0x13
#define EEADD_AutoType			0x14
#define EEADD_AutoDelta_LOWER	0x15
#define EEADD_AutoDelta_UPPER	0x16
#define EEADD_DUMMY				0x64

//----------NOEE Default value------
#define NOEE_DUMMY				104
#define NOEE_BCONST				3988
#define NOEE_I_Bias_IO			0
#define NOEE_I_Bias				Ib_Auto
#define NOEE_VSET				26214		// 25C
#define NOEE_ILIM				204			// I_LIM = 1A
#define NOEE_VLIM				589			// V_LIM = 6V
#define NOEE_K					0
#define NOEE_Ti					0			// OFF
#define NOEE_Td					0			// OFF
#define NOEE_HI_LIMIT			52428		// 32768 * (1 + (3A / 5A))
#define NOEE_LO_LIMIT			13108		// 32768 * (1 - (3A / 5A))
#define NOEE_AutoType			Autotune_PI
#define NOEE_AutoDelta			-655			// (0.2A / 5A) * 32768

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
#define ATUNE_COORD_X			COLUMNPIXEL0507 * 14
#define ATUNE_COORD_Y			ROWPIXEL0507 * 1
#define ATUNE_COORD_X2			COLUMNPIXEL0507 * 17
#define Text_AT					"AT:"
#define ATUNE_DELTA_COORD_X		COLUMNPIXEL0507 * 14
#define ATUNE_DELTA_COORD_Y		ROWPIXEL0507 * 2
#define ATUNE_DELTA_COORD_X2	COLUMNPIXEL0507 * 17
#define Text_dA					"dA:"
#define K_COORD_X				COLUMNPIXEL0507 * 14
#define K_COORD_Y				ROWPIXEL0507 * 3
#define K_COORD_X2				COLUMNPIXEL0507 * 16
#define Text_K					"K:"
#define Ti_COORD_X				COLUMNPIXEL0507 * 14
#define Ti_COORD_Y				ROWPIXEL0507 * 4
#define Ti_COORD_X2				COLUMNPIXEL0507 * 17
#define Text_Ti					"Ti:"
#define I_BIAS_COORD_X			COLUMNPIXEL0507 * 14
#define I_BIAS_COORD_Y			ROWPIXEL0507 * 5
#define I_BIAS_COORD_X2			COLUMNPIXEL0507 * 17
#define Text_Ib					"Ib:"
#define BCONST_COORD_X			COLUMNPIXEL0507 * 14
#define BCONST_COORD_Y			ROWPIXEL0507 * 6
#define BCONST_COORD_X2			COLUMNPIXEL0507 * 16
#define Text_B					"B:"
#define MODE_COORD_X			COLUMNPIXEL0507 * 14
#define MODE_COORD_Y			ROWPIXEL0507 * 7
#define MODE_COORD_X2			COLUMNPIXEL0507 * 16
#define Text_M					"M:"

// define GLCD parameter
#define COLUMNPIXEL1015			11			//column pixels of fixed_bold10x15
#define COLUMNPIXEL0715			8			//column pixels of fixednums8x16
#define COLUMNPIXEL0507			6			//column pixels of SystemFont5x7
#define ROWPIXEL0507			8			//row pixels of SystemFont5x7

// define I2C parameter
#define I2CSENDDELAY			100			//delay100us
#define I2CREADDELAY			100			//delay100us

//define calculation parameter
#define V_ACT_LIM_H				64225
#define V_ACT_LIM_L				1311
#define T0INV					0.003354016			// 1 / 298.15
#define RTHRatio_Lo				26214
#define RTHRatio_Hi				209712
#define Bin_To_Itec				0.002434716796875	// (g_I_Tec * (3.022 * 3.3V) / (4096(12bit ADC)) - 5
#define Bin_To_Ilim				0.0048828125		// g_I_Lim * (5 / 1024(10bit DAC))
#define Ilim_To_Bin				204.8				// g_I_Print * (1024(10bit DAC) / 5)
#define Bin_To_Idelta			0.152587890625		// g_I_AutoDelta * (5000mA / 32768(half of 16bit DAC))
#define Idelta_To_Bin			6.5536				// g_I_AutoDelta * (32768(half of 16bit DAC) / 5000mA)
#define PI_Zero					32768				// DAC value that current is 0
#define PI_Freq					1000				// Timer frequency = 1KHz

typedef enum {
	Ib_Auto = 0x00U,
	Ib_200uA = 0x01U,
	Ib_1_6mA = 0x02U
} I_Bias_Mode;

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
	void WaitPowerOn();
	bool MemReload();
	void CheckStatus();
	void UpdateParam();
	void WelcomeScreen();
	void BackGroundPrint();
	void PrintTset();
	void PrintTact(float tact);
	void PrintItec(float itec);
	void PrintIlim();
	void PrintAtune();
	void PrintAtuneResult();
	void PrintAtuneDone();
	void PrintAtunDelta();
	void PrintK();
	void PrintTi();
	void PrintIb();
	void PrintB();
	void PrintM();
	void PrintNormalAll();
	void CursorState();
	void blinkTsetCursor();
	void ShowCursor(unsigned char CursorState);
	void Encoder();

	//working variable-------------------
	short g_V_Tec;
	short g_I_Tec;
	short g_Auto_Delta;
	unsigned char g_Mem_Load;
	unsigned char g_Remote;
	unsigned char g_IO_State;
	unsigned char g_PID_Mode;
	unsigned char g_I_Bias;
	unsigned char g_Auto_Type;
	unsigned char g_Auto_Result;
	unsigned short g_Dev_State;
	unsigned short g_B_Const;
	unsigned short g_V_Set;
	unsigned short g_V_Act;
	unsigned short g_V_Lim;
	unsigned short g_I_Lim;
	unsigned short g_K;
	unsigned short g_Ti;
	unsigned short g_Td;
	unsigned short g_HI_LIMIT;
	unsigned short g_LO_LIMIT;

	short g_I_AutoDelta;
	float g_T_Set;
	float g_I_Print;

	bool g_atune_status, g_runTimeflag, g_lock_flag;
	unsigned char g_cursorstate;
	//------------------------------------
	unsigned short g_pid_mode;

private:
	float ReturnTemp(unsigned int vact);
	unsigned int ReturnVset(float tset);
	unsigned char QCP0_CRC_Calculate(unsigned char *pData, unsigned char Length);
	void QCP0_Package(unsigned char RorW, unsigned short Command, unsigned short Data, unsigned char *pData);
	void QCP0_Unpackage(unsigned char *pData, unsigned char *RorW, unsigned short *Command, unsigned short *Data);
	void QCP0_REG_PROCESS(unsigned short Command, unsigned short Data);

	glcd lcd;
	bool g_EncodeDir;
	bool p_LongPress;
	unsigned int p_PressTime[2];
	unsigned int p_tBlink, p_holdCursorTimer;
	unsigned int g_encstep;
	unsigned char p_ee_update_state;
	bool g_paramupdate, p_tBlink_toggle, p_blinkTsetCursorFlag;
	bool p_atunProcess_flag;
	unsigned long g_tenc, p_loopindex;
	unsigned long g_param_t;
	float g_tsetstep;
};
