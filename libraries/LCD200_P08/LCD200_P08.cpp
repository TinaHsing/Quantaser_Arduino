#include <Arduino.h>
#include <LTC2451.h>
#include <AD5541.h>
#include <SPI.h>
#include <LCD200_P08.h>



LCD200::LCD200()
{}

void LCD200::SetPinMode()
{
    pinMode(ENC2_A, OUTPUT);
  	pinMode(ENC2_SW, INPUT);
  	pinMode(ENC2_B, OUTPUT);
  	pinMode(PWR_OFF, OUTPUT);
  	pinMode(LD_EN, OUTPUT);
  	pinMode(PZT, OUTPUT);
  	pinMode(ENDAC, OUTPUT);
  	pinMode(VFC1, OUTPUT);
  	pinMode(LDSW, INPUT);
  	pinMode(VFC2, OUTPUT);
  	pinMode(VFC3, OUTPUT);
  	pinMode(VLD, INPUT);
  	pinMode(V_SENS, INPUT);
  	pinMode(LCDSW, INPUT);
}

void LCD200::DACInit()
{
	ad5541.init();
  ad5541.SetPin(ENDAC);
	ad5541.NormalWrite(65535);
}





