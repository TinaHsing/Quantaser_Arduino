#include <Wire.h>
#include <openGLCD.h>
#include <DTC03_MS.h>
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
#define ILIMSTART 0.5
#define ILIMSTEP 0.05
#define DEBOUNCETIME 2 //debounceing time(ms) for ENC
#define COUNTRESETTIME 80
#define LONGPRESSTIME 1000
#define FACTORYMODETIME 20000
#define ENGCOUNTER 12
#define BLINKDELAY 200
#define CURRENTLIMMAX 50 //maximun of g_currentlim
#define BCONSTMAX 4500//maximun of bconst
#define HIGHLOWBOUNDRY 500 //


//=================pin definition=========================
#define ENC_B 3
#define ENC_A 2
#define PUSHB A7
#define ENSW A6 

//=====================text coordinate definition=========
#define TSET_T_X 0 
#define TSET_T_Y 0
#define TACT_T_X 0
#define TACT_T_Y 24
#define ITEC_T_X 7
#define ITEC_T_Y 48
#define ILIM_T_X 7
#define ILIM_T_Y 56
#define P_T_X 90
#define P_T_Y 24
#define I_T_X 90
#define I_T_Y 32
#define D_T_X 90
#define D_T_Y 40
#define BCONST_T_X 90
#define BCONST_T_Y 48
#define VMOD_T_X 90 // 11/13 when print Vmod status, dispble Sensor type selection funtion
#define VMOD_T_Y 56
#define SENSOR_V_X 90 
#define SENSOR_V_Y 56

#define VMOD_V_X 108 // 11/13 when print Vmod status, dispble Sensor type selection funtion
#define VMOD_V_Y 56
#define TSET_V_X 16
#define TSET_V_Y 0
#define TACT_V_X 18
#define TACT_V_Y 24
#define ITEC_V_X 37
#define ITEC_V_Y 48
#define ILIM_V_X 37
#define ILIM_V_Y 56
#define P_V_X 102
#define P_V_Y 24
#define I_V_X 102
#define I_V_Y 32
#define D_V_X 102
#define D_V_Y 40
#define BCONST_V_X 102
#define BCONST_V_Y 48


#define VBEH1_X 0
#define VBEH1_X2 42//
#define VBEH1_Y 0
#define VBEH2_X 0
#define VBEH2_X2 42//
#define VBEH2_Y 8
#define VBEC1_X 0
#define VBEC1_X2 42//
#define VBEC1_Y 16
#define VBEC2_X 0
#define VBEC2_X2 42//
#define VBEC2_Y 24
#define VFBC_X 0
#define VFBC_X2 36
#define VFBC_Y 32
#define VMOD_X 0
#define VMOD_X2 36
#define VMOD_Y 40

// define GLCD parameter
#define COLUMNPIXEL1015 11 //column pixels of fixed_bold10x15
#define COLUMNPIXEL0507 6  //column pixels of SystemFont5x7
#define ROWPIXEL0507 8 //row pixels of SystemFont5x7


// define I2C parameter
#define I2CSENDDELAY 100 //delay100ms
#define I2CREADDELAY 100 //delay100us

//define calculation parameter
#define T0INV 0.003354
#define RTHRatio 25665 
//#define CURRENTRatio 9.775	// code * 5000/1023/50/0.01= code * 9.997 (mA)
#define CURRENTRatio 0.009997// code * 5/1023/50/0.01= code * 0.09997 (A)

/*matrix modified on 168us 20161103*/
//const PROGMEM unsigned char kilstable[] =
//{ 
//0,   0, //0
//16,	220, //0.05
//17,	220,//0.1
//18,	220,//0.2
//18,	147,//0.3
//19,	220,//0.4
//19,	176,//0.5
//19,	147,//0.6
//20,	251,//0.7
//20,	220,//0.8
//20,	196,//0.9
//20,	176,//1
//20,	160,//1.1
//20,	147,//1.2
//20,	135,//1.3
//21,	252,//1.4
//21,	235,//1.5
//21,	220,//1.6
//21,	207,//1.7
//21,	196,//1.8
//21,	185,//1.9
//21,	176,//2
//21,	141,//2.5
//22,	235,//3
//22,	201,//3.5
//22,	176,//4
//22,	157,//4.5
//22,	141,//5
//23,	235,//6
//23,	201,//7
//23,	176,//8
//23,	157,//9
//23,	141,//10
//24,	235,//12
//24,	201,//14
//24,	176,//16
//24,	157,//18
//24,	141,//20
//25,	225,//25
//25,	188,//30
//25,	161,//35
//25,	141,//40
//26,	251,//45
//26,	225,//50
//26,	204,//55
//26,	187,//60
//26,	173,//65
//26,	161,//70
//26,	150,//75
//26,	141,//80
//
//};
//
//
//const PROGMEM unsigned int timeconst[] =
//{
//  0,
//  5,
//  10, 
//  20,
//  30,
//  40,
//  50,
//  60,
//  70,
//  80,
//  90,
//  100,
//  110,
//  120,
//  130,
//  140,
//  150,
//  160,
//  170,
//  180,
//  190,
//  200,
//  250,
//  300,
//  350,
//  400,
//  450,
//  500,
//  600,
//  700,
//  800,
//  900,
//  1000,
//  1200,
//  1400,
//  1600,
//  1800,
//  2000,
//  2500,
//  3000,
//  3500,
//  4000,
//  4500,
//  5000,
//  5500,
//  6000,
//  6500,
//  7000,
//  7500,
//  8000,
//};




class DTC03Master {
public:
	DTC03Master();
	void SetPinMode();
	void ParamInit();
	void WelcomeScreen();
	void I2CReadData(unsigned char i);
	void I2CReadAll();
	void VarrayInit();
	void IarrayInit();
	void BackGroundPrint();
	float ReturnTemp(unsigned int vact, bool type);
	void PrintTset();
	void PrintTact(float tact);
	void PrintItec(float itec);
	void PrintIlim();
	void PrintP();
	void PrintKi();
	void PrintB();
	void PrintSensor();
	void PrintModStatus();
	void Encoder();
	void CursorState();
	void I2CWriteData(unsigned char com);
	void UpdateParam();
	unsigned int ReturnVset(float tset, bool type);
	void PrintFactaryMode();
	void CheckStatus();
	void PrintEngBG();
	void PrintVbeh1();
	void PrintVbeh2();
	void PrintVbec1();
	void PrintVbec2();
	void PrintVfbc();
	void PrintVmod();
	void ShowCursor();
    void UpdateEnable();//
	
    // unsigned int ReturnVset(float tset, bool type);
	// void ReturnKi();
	// void ReturnLs();
	unsigned int g_vact, g_vset;
	bool g_sensortype, g_en_state, g_mod_status;
	//bool g_sensortype, g_en_state,g_paramupdate; //
    unsigned long g_vactsum; 
	//long g_itecsum;
	int g_itecsum;//
    float g_tset;
    unsigned char g_cursorstate;
private:
	glcd lcd;
	int g_counter;
	//int Iarray[IAVGTIMES];//
    unsigned int g_bconst, g_fbcbase, Varray[VAVGTIMES], Iarray[IAVGTIMES], g_icount ,g_vmodoffset;
	//unsigned int g_bconst, g_fbcbase, Varray[VAVGTIMES], g_icount;
    unsigned char g_p, g_ki,g_ls,g_currentlim, g_vbec1, g_vbec2, g_vbeh1, g_vbeh2, g_kiindex;
	//unsigned char g_iarrayindex, g_varrayindex, g_lastencoded, g_cursorstate, g_engmode;
	unsigned char g_iarrayindex, g_varrayindex, g_lastencoded, g_engmode;
 	//bool g_errcode1, g_errcode2, g_flag, g_countersensor;
    bool g_errcode1, g_errcode2, g_flag, g_paramupdate, g_countersensor;
    unsigned long g_tenc;
	float g_tsetstep;


};
