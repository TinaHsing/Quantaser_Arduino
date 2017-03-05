#include <Wire.h>
#include <openGLCD.h>
#include <DTC03_MS.h>
#include <EEPROM.h>
#include <Wire.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Iain5x7.h>
#include <fonts/fixed_bold10x15.h>
#include <avr/pgmspace.h>

//========Frequently update paramter===========
#define BCONSTOFFSET 3500
#define KILENGTH 16
#define VAVGTIMES 64	// Note!!!! VAVGTIMEs = 2 ^ VAVGPWR 
#define VAVGPWR 6		// Note!!!! VAVGTIMEs = 2 ^ VAVGPWR
#define IAVGTIMES 8
#define IAVGPWR 3
#define ILIMSTART 0.45
#define ILIMSTEP 0.05
#define DEBOUNCETIME 2 //debounceing time(ms) for ENC
#define COUNTRESETTIME 80
#define LONGPRESSTIME 1000
#define FACTORYMODETIME 20000
#define ENGCOUNTER 10
#define BLINKDELAY 350
#define CURRENTLIMMAX 50 //maximun of g_currentlim
#define BCONSTMAX 4500//maximun of bconst
#define HIGHLOWBOUNDRY 500 //
#define CURSORSTATE_STAYTIME 700
#define ACCUMULATE_TH 50
#define DEBOUNCE_WAIT ACCUMULATE_TH*4
#define MV_STATUS 1
#define MVTIME 16
#define MVTIME_POWER 4
//=================pin definition=========================
#define ENC_B 3
#define ENC_A 2
#define PUSHB A7
#define ENSW A6 

//-----------EEPROM ADDRESS---------
//DTC03
#define EEADD_VSET_UPPER	0
#define EEADD_VSET_LOWER	1
#define EEADD_MODSTATUS		4
#define EEADD_currentlim 	5
#define EEADD_FBC_UPPER		6
#define EEADD_FBC_LOWER		7
#define EEADD_P             8
#define EEADD_KIINDEX	    9
#define EEADD_TOTP_UPPER    10
#define EEADD_TOTP_LOWER    11
#define EEADD_R1            12
#define EEADD_R2            13
#define EEADD_TPIDOFF		14
#define EEADD_MODOFF_UPPER	15
#define EEADD_MODOFF_LOWER	16
#define EEADD_RMEAS_UPPER	17
#define EEADD_RMEAS_LOWER	18
#define EEADD_BCONST_UPPER	19
#define EEADD_BCONST_LOWER	20
//DTC03_2
#define EEADD_VSET_UPPER_2	25
#define EEADD_VSET_LOWER_2	26
#define EEADD_MODSTATUS_2	27
#define EEADD_currentlim_2 	28
#define EEADD_FBC_UPPER_2	29
#define EEADD_FBC_LOWER_2	30
#define EEADD_P_2           31
#define EEADD_KIINDEX_2	    32
#define EEADD_TOTP_UPPER_2  33
#define EEADD_TOTP_LOWER_2  34  
#define EEADD_R1_2          35
#define EEADD_R2_2          36 
#define EEADD_TPIDOFF_2		37
#define EEADD_MODOFF_UPPER_2 38
#define EEADD_MODOFF_LOWER_2 39	
#define EEADD_RMEAS_UPPER_2	 40
#define EEADD_RMEAS_LOWER_2	 41
#define EEADD_BCONST_UPPER_2 42
#define EEADD_BCONST_LOWER_2 43
//LCD200
#define EEADD_VTH1			21
#define EEADD_VTH2			22
#define EEADD_IOUT_UPPER    23
#define EEADD_IOUT_LOWER    24


#define EEADD_DUMMY			100
//----------NOEE Default value------
//DTC03
#define NOEE_DUMMY 		104
#define NOEE_VSET		26214//25C
#define NOEE_ILIM		11 // currntlimit=0.45+0.05*11=1A
#define NOEE_P			10
#define NOEE_kiindex    1 //OFF
#define NOEE_BCONST		3988
#define NOEE_MODSTATUS  0
#define NOEE_R1			20
#define NOEE_R2			30
#define NOEE_TPIDOFF    2
#define NOEE_FBC       	21000
#define NOEE_FBC2       21000
#define NOEE_MODOFF     32494
#define NOEE_MODOFF2    32494
#define NOEE_RMEAS      28400
#define NOEE_RMEAS2     28400
#define NOEE_TOTP		561 //120C

//LCD200
#define NOEE_VTH1		10
#define NOEE_VTH2		20
#define NOEE_IOUT		65535

//=====================BG print coordinate definition=========
//DTC03
#define TSET_COORD_X	0 
#define TSET_COORD_Y	0
#define TSET_COORD_X2	16
#define Text_SET		"SET"
#define TACT_COORD_X	0
#define TACT_COORD_Y	ROWPIXEL0507*3
#define TACT_COORD_X2	18
#define Text_ACT		"ACT"
#define ITEC_COORD_X	COLUMNPIXEL0507
#define ITEC_COORD_Y	ROWPIXEL0507*6
#define ITEC_COORD_X2	COLUMNPIXEL0507*6
#define Text_ITEC		"ITEC:"
#define ILIM_COORD_X	COLUMNPIXEL0507
#define ILIM_COORD_Y	ROWPIXEL0507*7
#define ILIM_COORD_X2	COLUMNPIXEL0507*7
#define Text_ILIM		"ILIM:"
#define P_COORD_X		COLUMNPIXEL0507*14
#define P_COORD_Y		ROWPIXEL0507*3
#define P_COORD_X2		COLUMNPIXEL0507*17
#define Text_P			"P:"
#define I_COORD_X		COLUMNPIXEL0507*14
#define I_COORD_Y		ROWPIXEL0507*4
#define I_COORD_X2		COLUMNPIXEL0507*16
#define Text_I			"I:"
#define BCONST_COORD_X 	COLUMNPIXEL0507*14
#define BCONST_COORD_Y 	ROWPIXEL0507*6
#define BCONST_COORD_X2	COLUMNPIXEL0507*16
#define Text_B			"B:"
#define VMOD_COORD_X 	COLUMNPIXEL0507*14
#define VMOD_COORD_Y 	ROWPIXEL0507*7
#define VMOD_COORD_X2 	COLUMNPIXEL0507*17
#define Text_MS			"MS:"


//==================ENG BG print coordinate definition=========
#define R1_COORD_X       COLUMNPIXEL0507
#define R1_COORD_X2      COLUMNPIXEL0507*7  
#define R1_COORD_Y       0
#define Text_R1 		 "R1   :"
#define R2_COORD_X 		 COLUMNPIXEL0507
#define R2_COORD_X2 	 COLUMNPIXEL0507*7
#define R2_COORD_Y 		 ROWPIXEL0507
#define Text_R2 		 "R2   :"
#define TPIDOFF_COORD_X  COLUMNPIXEL0507
#define TPIDOFF_COORD_X2 COLUMNPIXEL0507*7
#define TPIDOFF_COORD_Y  ROWPIXEL0507*2
#define Text_pidOS 		 "pidOS:"
#define VFBC_COORD_X 	 COLUMNPIXEL0507
#define VFBC_COORD_X2 	 COLUMNPIXEL0507*7
#define VFBC_COORD_Y 	 ROWPIXEL0507*3
#define Text_Vfbc 		 "Vfbc :"
#define VMOD_COOED_X 	 COLUMNPIXEL0507
#define VMOD_COOED_X2 	 COLUMNPIXEL0507*7
#define VMOD_COOED_Y 	 ROWPIXEL0507*4
#define Text_Vmod 		 "Vmod  :"
#define RMEAS_COORD_X 	 COLUMNPIXEL0507
#define RMEAS_COORD_X2 	 COLUMNPIXEL0507*7
#define RMEAS_COORD_Y 	 ROWPIXEL0507*5
#define Text_Rmeas 		 "Rmeas:"
#define TOTP_COORD_X 	 COLUMNPIXEL0507
#define TOTP_COORD_X2 	 COLUMNPIXEL0507*9
#define TOTP_COORD_Y 	 ROWPIXEL0507*6
#define Text_Totp 		 "Totp :"
#define TPCB_COORD_X 	 COLUMNPIXEL0507
#define TPCB_COORD_X2 	 COLUMNPIXEL0507*9
#define TPCB_COORD_Y 	 ROWPIXEL0507*7
#define Text_Tpcb 		 "Tpcb :"

#define Test1_COORD_X    0
#define Test1_COORD_Y    ROWPIXEL0507*2
#define Test2_COORD_X    0
#define Test2_COORD_Y    ROWPIXEL0507*3
#define Test3_COORD_X    0
#define Test3_COORD_Y    ROWPIXEL0507*4

//Five in One circuit
#define T1_S_X			0 
#define T1_S_Y			0
#define T1_S_X2			COLUMNPIXEL0507*5
#define Text_T1_S		"T1_S:"
#define T1_A_X			0 
#define T1_A_Y			ROWPIXEL0507
#define T1_A_X2			COLUMNPIXEL0507*5
#define Text_T1_A		"T1_A:"
#define A1_X			COLUMNPIXEL0507*12
#define A1_Y			0
#define A1_X2			COLUMNPIXEL0507*15
#define Text_A1			"A1:"
#define P1_X			COLUMNPIXEL0507*12
#define P1_Y			ROWPIXEL0507
#define P1_X2			COLUMNPIXEL0507*15
#define Text_P1			"P1:"
#define I1_X			COLUMNPIXEL0507*12
#define I1_Y			ROWPIXEL0507*2
#define I1_X2			COLUMNPIXEL0507*15
#define Text_I1			"I1:"

#define T2_S_X			0 
#define T2_S_Y			ROWPIXEL0507*3
#define T2_S_X2			COLUMNPIXEL0507*5
#define Text_T2_S		"T2_S:"
#define T2_A_X			0 
#define T2_A_Y			ROWPIXEL0507*4
#define T2_A_X2			COLUMNPIXEL0507*5
#define Text_T2_A		"T2_A:"
#define A2_X			COLUMNPIXEL0507*12
#define A2_Y			ROWPIXEL0507*3
#define A2_X2			COLUMNPIXEL0507*15
#define Text_A2			"A2:"
#define P2_X			COLUMNPIXEL0507*12
#define P2_Y			ROWPIXEL0507*4
#define P2_X2			COLUMNPIXEL0507*15
#define Text_P2			"P2:"
#define I2_X			COLUMNPIXEL0507*12
#define I2_Y			ROWPIXEL0507*5
#define I2_X2			COLUMNPIXEL0507*15
#define Text_I2			"I2:"

#define I_LD_X			COLUMNPIXEL0507
#define I_LD_Y			ROWPIXEL0507*6
#define I_LD_X2			COLUMNPIXEL0507*6
#define I_LD_ACT_X2		COLUMNPIXEL0507*13
#define Text_I_LD	 	"I_LD:"

#define V_PZT_X			0
#define V_PZT_Y			ROWPIXEL0507*7
#define V_PZT_X2		COLUMNPIXEL0507*6
#define Text_V_PZT	 	"V_PZT:"

// define GLCD parameter
#define COLUMNPIXEL1015 11 //column pixels of fixed_bold10x15
#define COLUMNPIXEL0507 6  //column pixels of SystemFont5x7
#define ROWPIXEL0507 8 //row pixels of SystemFont5x7


// define I2C parameter
#define I2CSENDDELAY 100 //delay100us
#define I2CREADDELAY 100 //delay100us

//define calculation parameter
#define T0INV 0.003354
#define RTHRatio 25665 
#define CURRENTRatio 0.00977	// 3A/307code=0.00977A/code 


class DTC03Master {
public:
	DTC03Master();
	void SetPinMode();
	void ParamInit();
	void WelcomeScreen();
	void I2CReadData(unsigned char, unsigned char);
	void I2CWriteData(unsigned char, unsigned char);
	void I2CReadAll();
	void VarrayInit();
	void IarrayInit();
	void BackGroundPrint();
	float ReturnTemp(unsigned int vact, bool type);
	void PrintTset();
	void PrintTset2();
	void PrintTact(float tact);
	void PrintTact2(float tact);
	void PrintItec(float itec);
	void PrintItec2(float itec);
	void PrintIlim();
	void PrintP();
	void PrintP2();
	void PrintKi();
	void PrintKi2();
	void PrintB();
	void PrintModStatus();
	void Encoder();
	void CursorState();	
	void UpdateParam();
	unsigned int ReturnVset(float tset, bool type);
	void PrintFactaryMode();
	void CheckStatus();
	void PrintEngBG();
	void PrintNormalAll();
	void PrintEngAll();
	void PrintR1();
	void PrintR2();
	void PrintTpidoff();
	void PrintVfbc();
	void PrintVmod();
	void PrintRmeas();
	void PrintTotp();
	void PrintTpcb(float);
	void PrintEnable();
	void PrintLDcurrentAct(float);
	void PrintLDcurrentSet();
	void PrintPZTvolt(float);
	unsigned int ReturnCurrentDacout(float);
	float ReturnCurrent(unsigned int);
	
	void ShowCursor(unsigned char);
    void UpdateEnable();
    void blinkTsetCursor();
    void SaveEEPROM();
    void ReadEEPROM();
    void I2CWriteAll();
    void HoldCursortate();
    void vact_MV();
	
	//working variable-------------------
	//DTC03
	unsigned int g_vact,g_vact_MV, g_vset, g_vset2, g_tpcb, g_otp, g_Rmeas, g_Rmeas2, g_bconst, g_fbcbase, g_fbcbase2, g_vmodoffset, g_vmodoffset2;
	unsigned char g_p, g_p2, g_ki, g_ki2, g_ls,g_currentlim, g_tpidoff, g_r1, g_r2, g_kiindex, g_kiindex2, g_cursorstate;
	int g_itec;
	bool g_mod_status;	
    float g_tset, g_tset2;
    //LCD200
    unsigned int g_dacout, g_Ild;
    unsigned char g_vfth1, g_vfth2;
    float g_LDcurrent;
    //PZTDRF
    unsigned int g_vpzt;
    //------------------------------------
    bool g_sensortype, g_en_state;
    int en_temp;
    unsigned char g_test;
    
private:
	glcd lcd;
	int g_counter;
    unsigned int  g_icount , p_cursorStateCounter[3], p_temp, p_cursorStayTime;
    unsigned int p_tBlink, p_tcursorStateBounce, p_holdCursorTimer, p_vact_array[16]; 
	unsigned long  p_vact_MV_sum, p_mvindex;
	unsigned char g_iarrayindex, g_varrayindex, g_lastencoded, p_engmodeCounter, p_ee_change_state;
    bool g_errcode1, g_errcode1_2, g_errcode2, g_errcode2_2, g_flag, g_paramupdate, g_countersensor, g_testgo, p_tBlink_toggle, p_engModeFlag, p_blinkTsetCursorFlag, g_wakeup;
    bool p_ee_changed, p_HoldCursortateFlag, p_timerResetFlag;
    unsigned long g_tenc, p_loopindex;
	float g_tsetstep;


};
