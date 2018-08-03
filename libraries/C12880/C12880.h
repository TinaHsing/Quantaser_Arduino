#ifndef C12880_H
#define C12880_H

#define CLKPERIOD1 1 	// 1us
#define CLKPERIOD2 100 	// 100us


// =======define pin for the spectrometer ========
// Spectro A:
// CLK : pin 3
// STA : pin 4
// STB : pin 5
// so use DDRD to fast manipulate
#define PIN_VEDIOA 	A3
#define PIN_CLK 	A2
#define PIN_STA 	A1
#define PIN_STB 	4

#define CHANNEL_NUMBER 288


#define CLK_HIGH 	B00000100 // use | to implement clk high with PORTC
#define CLK_LOW 	B11111011 // use & to implement clk low with PORTC
#define STA_HIGH    B00000010 //use & to implement STA high with PORTC
#define STB_HIGH 	B00010000 //use & to implement STB high with PORTD

#define ADC_READA	B01000011 // set the reference to 5V, Set the result to the right adjust, ReadCh3
#define ADC_READB	B01000100 // set the reference to 5V, Set the result to the right adjust, ReadCh4

#define nop asm volatile ("nop\n\t") // use nop to tune the delay



class C12880
{
private:
	
	unsigned char guc_led1, guc_led2;
	unsigned char guc_opst1h, guc_opst1l, guc_opst2h, guc_opst2l; // fast port manipulation constant

	public:
	C12880();

	void SpectroInit();
	void LEDInit(unsigned char led1, unsigned char led2);
	void StartLED(unsigned int delaytime);
	void PulseClkA(unsigned long pulse);
	void StartIntegA();
	void StopIntegA();
	void ReadVedioA(int *buffer);

};

#endif