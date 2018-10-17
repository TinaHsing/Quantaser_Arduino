#ifndef QSS003_P02_H
#define QSS003_P02_H 


// QSS003 parameters
#define LEDA 1
#define LEDB 2
#define LEDC 3
#define CYCP 1.8
#define MAXFILE 800

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

class LTC2615;

class QSS003 
{
public:
	QSS003();
	void Initialize(void);
	void checkParam();
	void currentOut(unsigned char ch, unsigned int cur);
	unsigned long us2cyc(unsigned long t_int);
	unsigned long gul_ta, gul_tb, gul_tw;
	unsigned int gui_i1, gui_i2, gui_i3, gui_rp;
private:
	LTC2615 ltc2615;



};
#endif






