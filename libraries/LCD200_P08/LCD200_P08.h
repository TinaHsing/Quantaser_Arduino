

// ---------Pin definition of LCD200_P06----------//

#define ENC2_A 0 //rotary encoder 2A
#define ENC2_SW 1 //rotary encoeder switch for changing cursor
#define ENC2_B 2 //INT0 for rotory encoder 2B
#define PWR_OFF 3 // Vcc switch
#define LD_EN 4 // on/off the LD current
#define PZT 5 // PD5 for buzzle driver (excnage with LCDSW pin)
#define ENDAC 6  //PD6 for control AD5541
#define VFC1 7 // VFC1 swich
#define LDSW 8 //PB0 for Vcc on/off
#define VFC2 9 // VFC3 switch
#define VFC3 10 // VFC3 switch 
#define VLD A1 // PC1 (ADC1) for LD voltage read
#define V_SENS A2 // V+ voltage detection
#define LCDSW A7 // page switch

class LTC2451;
class AD5541;

class LCD200
{
public:
	LCD200();
	void SetPinMode();
	void DACInit();
	
	AD5541 ad5541;

};
