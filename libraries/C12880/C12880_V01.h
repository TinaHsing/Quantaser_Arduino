#ifndef C12880_H
#define C12880_H


#define DEBUG		0
#define TIMEMODE	0
#define HEXMODE     1
#define NONHEX      0

// =======define pin for the spectrometer ========
// Spectro A:
// CLK : pin 3
// STA : pin 4
// so use DDRD to fast manipulate


#define CHANNEL_NUMBER	288
#define PAUSE_NUMBER	87

#define nop asm volatile ("nop\n\t") // use nop to tune the delay



enum{
	ABSameTime = 0,
	ATimeBig2B,
	BTimeBig2A,
};

class LTC1865;

class C12880
{



public:
	LTC1865 adc;
	C12880();

	bool SpectroInit(unsigned char clka, unsigned char sta, unsigned char clkb, unsigned char stb, unsigned char adcconv, unsigned char adc_cha);

	void RunDevice(unsigned long I_timeA, unsigned long I_timeB);

private:
	unsigned char guc_sta, guc_stb, guc_clka, guc_clkb;
	unsigned char guc_clka_high, guc_clkb_high, guc_clkab_high, guc_clka_low, guc_clkb_low, guc_clkab_low;
	unsigned char guc_sta_high, guc_stb_high, guc_stab_high;
	unsigned char guc_adc_cha;
	void PulseClkA(unsigned long pulse);
	void PulseClkB(unsigned long pulse);
	void PulseClkAB(unsigned long pulse);
	void StartIntegAB();
	void StopIntegA();
	void StopIntegB();
	void ReadVedioAB();

};

#endif