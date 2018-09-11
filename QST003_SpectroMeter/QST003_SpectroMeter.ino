#include <LTC2615.h>
#include <SPI.h>
#include <LTC1865.h>
#include <C12880_V01.h>

///// DAC paras /////
#define LEDA 1
#define LEDB 2
#define LEDC 3
///// C12880 paras /////
#define CLKA 2
#define STA 5
#define CLKB 3
#define STB 6
#define ADCCONV 7
#define ADC_CHA 0
#define CYCP 1.8 //cycle period 1.8 us

LTC2615 ltc2615;
C12880 spectro;

void setup() {
  Serial.begin(115200);
  ltc2615.init();
  spectro.SpectroInit(CLKA, STA, CLKB, STB, ADCCONV, ADC_CHA);
}

void loop() {
  long I1, I2, I3, ta, tb, t_wait, rp;
  
  scanVar("Input LED_A current in mA", I1); 
  scanVar("Input LED_B current in mA", I2); 
  scanVar("Input LED_C current in mA", I3); 
  scanVar("Input integration time of spectrometer A(us, min 90)", ta); 
  scanVar("Input integration time of spectrometer B(us, min 90)", tb); 
  scanVar("Input wait time before date acquisition(us)", t_wait); 
  scanVar("Input repeat times", rp); 
  currentOut(LEDA, I1);
  currentOut(LEDB, I2);
  currentOut(LEDC, I3);
  delayMicroseconds(t_wait);
  for(int i=0; i<rp; i++) 
  {
    spectro.RunDevice(us2cyc(ta), us2cyc(tb));
  }
  
}

void scanVar(String s, long &var)
{
  char r;
  s.concat(": ");
  Serial.print(s);
  while(Serial.available() == 0);
  var = Serial.parseInt();
  Serial.println(var);
  r=Serial.read(); //read "\n"
}

void currentOut(byte ch, int cur)
{
  float volt, r_sense = 0.5;
  
  volt = (float)cur/1000.0*r_sense;
  
  if(ch == 1) {
    ltc2615.write(CH_G, volt);
    Serial.print("CH_G: ");
    Serial.println(volt);
  }
  else if(ch == 2) {
    ltc2615.write(CH_F, volt);
    Serial.print("CH_F: ");
    Serial.println(volt);
  }
  else if(ch == 3) {
    ltc2615.write(CH_E, volt);
    Serial.print("CH_E: ");
    Serial.println(volt);
  } 
}

unsigned long us2cyc(long t_int)
{
  unsigned long cyc;
  if(t_int < 90) t_int = 90;
  cyc = (unsigned long)t_int/CYCP-48;
//  Serial.println(cyc);
  return cyc;
}

