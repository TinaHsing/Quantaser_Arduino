#include <SPI.h>
#include <LTC1865.h>
#include <C12880_V01.h>
#define CLKA 2
#define STA 5
#define CLKB 3
#define STB 6
#define ADCCONV 7
#define ADC_CHA 0

C12880 spectro(CLKA, STA, CLKB, STB, ADCCONV, ADC_CHA);


void setup() {
 
  Serial.begin(9600);
  Serial.println("test");
//  spectro.SpectroInit();
//  spectro.RunDevice(2000, 2500);
  Serial.println("test");

}

void loop() {
  // put your main code here, to run repeatedly:

}


