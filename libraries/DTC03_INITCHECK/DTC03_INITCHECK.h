//#include <DTC03_MS.h>
//#include <Wire.h>
#include <SPI.h>
#include <LTC1865.h>
//#include <EEPROM.h>
#include <AD5541.h>
//#include <avr/pgmspace.h>

//========================Other Library version Request =================
//Need to Use LTC1865 1.01
// ===================================================

// =====================DEBUGFLAG Table =================================
#define DEBUGFLAG01
//#define DEBUGFLAG02 3
//#define DEBUGFLAG03
#define INITIAL_VALUE_CHECK 

#ifdef DTCDEBUG01
	#define DEBUGFLAG01 // Show initial setup information in Serial monitor
#else
#endif

#ifdef DTCDEBUG02				 // Disable VCC autodetection function and set VCC for H, M, L by 3, 2, 1
	#define DEBUGFLAG02 DTCDEBU02 // Disable VCC autodetection function and set VCC for H, M, L by 3, 2, 1
#else
#endif 

#ifdef DTCDEBUG03
	#define DEBUGFLAG03
#else
#endif

//====================end of DEBUGFLAG Table==============================*/

//=====================Freqently update define ==========================
// ========User can copy below define to main function ==================
#define RMEASUREVOUT 55000 //20161031
#define RMEASUREDELAY 3000 //20161031
#define RMEASUREAVGTIME 10
#define VACTAVGTIME 64 // Note!!!! VACTAVGTIEM = 2 ^ VACTAVGPWR 
#define VACTAVGPWR 6 	// Note!!!! VACTAVGTIEM = 2 ^ VACTAVGPWR
#define ITECAVGTIME 64 // Note !!! ITECAVGTIME = 2^ ITECAVGPWR
#define ITECAVGPWR 6  // Note !!! ITECAVGTIME = 2^ ITECAVGPWR
#define IAVGTIME 16 	//average time for current sensor and Vtec read
#define BCONSTOFFSET 3500
#define VCCRTH_LM 0.6
#define VCCRTH_MH 1.0
#define VSETSLOWSTEP 1
#define ILIMDACOUTSTART 500		// define the current limit start current 500mA
#define ILIMDACSTEP 50  		// define the current limit step current 50mA
#define LIMCOUNTER 10
#define FBCCHECK_LOW 35000
#define FBCCHECK_HIGH 60000

//=================end of Frequently update define======================

//=======No EEPROM default parameter user can copy these define to main for debug======
#define NOEE_P 15
#define NOEE_KI 219
#define NOEE_LS 20
#define NOEE_KIINDEX 11 //20161103
#define NOEE_B 488
#define NOEE_SENS 0 //sensortype
#define NOEE_VSET 13524 //vset_lim =@Bconst=3988
#define NOEE_ILIM 32 // currntlimit,3A=50
//#define NOEE_VBEH1 215 // DACout max=215*
//#define NOEE_R2	19 // DACout minus step=19*
//#define NOEE_VBEC1	215
#define NOEE_R1 6 // R1, 0.6ohm
#define NOEE_R2	16 // R2, 1ohm
#define NOEE_PIDOFFSET	2 //Tpid offset 2000
#define NOEE_VBEC2	19
#define NOEE_FBC 22500 //45000
#define NOEE_DUMMY 104
#define NOEE_OFFSET 32393


// The address of EEPROM
#define EEADD_P 				0
//#define EEADD_KI 				1
//#define EEADD_LS				2
#define EEADD_currentlim 		3
#define EEADD_Vset_upper 		4
#define EEADD_Vset_lower 		5
#define EEADD_Sensor_type 		8
#define EEADD_B_upper 			9
#define EEADD_B_lower 			10
#define EEADD_R1		     	11
#define EEADD_R2			    12
#define EEADD_PIDOFFSET			13
#define EEADD_VBE_C2			14
#define EEADD_FBC_base_upper 	15
#define EEADD_FBC_base_lower 	16
#define EEADD_Vmodoffset_upper 	17
#define EEADD_Vmodoffset_lower 	18
#define EEADD_KIINDEX		    19 //20161101
#define EEADD_DUMMY             20
// Pin definition

#define FBSEL 3
#define NMOSC_IN 4
#define NMOSH_IN 5
#define CURRENT_LIM 6
#define LTC1865CONV 7
#define VCC1 8
#define VCC2 10
#define VCC3 1
#define DACC 9
#define ISENSE0 A0
#define TEMP_SENSOR A1
#define SENSOR_TYPE A2
#define TEC_VOLT A3

// define SLAVE ADDRESS

#define MUCSLAVE 0x07

// define VCC voltage state
#define VCCHIGH 3   // 8.3
#define VCCMEDIUM 2 // 6.4
#define VCCLOW 1 	// 4.8

// define MOS status
#define HEATING 1
#define COOLING 0

// define LTC1865 channel name
#define CHVACT 0
#define CHVMOD 1

// define port manipulation address
#define MOS_ON_OFF_STATUS_ADD (1 << NMOSC_IN)|(1<<NMOSH_IN)|(1<<FBSEL)

//define calculation ration parameter
#define CURRENTRatio 9.775 // code * 5000/1023/50/0.01= code * 9.997 (mA)
#define RTECRatio 2 // R=(vcode-vtec0)/(icode-itec0)/50/0.01 = (vcode-vtec0)/(icode-itec0)*2
#define RTHRatio 25665 //(Isen*R0)*65535/vref concert Isense*R0 to 16bit ADC code
#define T0INV 0.003354
#define V_NOAD590 30000 
#define V_OVERTEMP 481 // 90 degree
#define BVALUE 3988 

const unsigned char PS_16 = (1<<ADPS2);
const unsigned char PS_32 = (1<<ADPS2)|(1<<ADPS0);
const unsigned char PS_128 = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);


class AD5541;
class LT1865;
class PID;
class DTC03
{
public:
	DTC03();
    void SetPinMode();
	void SetSPI();
	void ParamInit();
	void DynamicVcc();
	void SetVcc(unsigned char state);
	void ReadEEPROMnew();
    void SetMos(bool heating, unsigned int fb_value);
    void SetMosOff();
	float CalculateR(unsigned int fb_value, unsigned int stabletime, int ravgtime, int vavgtime);
	unsigned int InitVactArray();
//	void ReadEEPROM();
	void CheckSensorType();
	void CheckTemp();
	void ReadVoltage();
	void VsetSlow();
//	void CurrentLimitGain(bool heating);
	void CurrentLimit();
	void I2CRequest();
	void I2CReceive();
	void SaveEEPROM();
    int ReadIsense();//
    void CheckInitValue(bool, bool, bool);
    float ReturnTemp(unsigned int);

	unsigned int g_vact, g_vset, g_fbc_base, g_isense0,Vactarray[VACTAVGTIME], Itecarray[ITECAVGTIME],g_currentabs,g_itecread;//
    unsigned char g_p, g_ki, g_ls, g_currentlim, g_ee_change_state,g_kiindex, g_limcounter,g_currentindex, g_tpidoffset, g_r1, g_r2;
	unsigned long g_vactavgsum, g_itecavgsum;
	bool g_en_state, g_heating, g_errcode1, g_errcode2, g_sensortype, g_mod_status, g_ee_changed;
    unsigned int g_b_upper, g_b_lower,g_vset_limit, g_ilimdacout,g_vset_limitt,g_vmod;
    unsigned int g_vmodoffset;//
    int g_iteclimitset;//
    AD5541 dacformos, dacforilim;

private:
	//int ReadIsense();
	int ReadVtec(int Avgtime);
	//unsigned char g_currentindex, g_vbeh1, g_vbeh2, g_vbec1, g_vbec2, g_vactindex;
    unsigned char  g_vactindex, g_vbec2;
	LTC1865 ltc1865;
	//AD5541 dacformos, dacforilim;
	//int g_vmodoffset;
	//unsigned int g_vset_limit,g_vbec, g_vbeh, g_b_upper, g_b_lower, g_ilimdacout, g_vmod;
	//unsigned int g_vbec, g_vbeh,g_vmod;
//	unsigned int g_vbec, g_vbeh;
    float g_ilimgain;
};
