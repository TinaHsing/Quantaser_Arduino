#include <Wire.h>
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
  Serial.begin(150000);
  ltc2615.init();
  spectro.SpectroInit(CLKA, STA, CLKB, STB, ADCCONV, ADC_CHA);
  currentOut(LEDA, 0);
  currentOut(LEDB, 0);
  currentOut(LEDC, 0);
}

void loop() {
  unsigned long I1, I2, I3, ta, tb, t_wait, rp, taa, tbb;
  unsigned long t1, t2;
  while(Serial.available()==0);
  Serial.read();
  scanVar("I1:", I1); 
  scanVar("I2:", I2); 
  scanVar("I3:", I3); 
  scanVar("Ta:", ta); 
  scanVar("Tb:", tb); 
  scanVar("Twait:", t_wait); 
  scanVar("Repeat:", rp); 
//  delay (20);
//  I1 = 150;
//  I2 = 150;
//  I3 = 150;
//  ta = 30000;
//  tb = 60000;
//  t_wait = 1000;
//  rp = 1;
  currentOut(LEDA, I1);
  currentOut(LEDB, I2);
  currentOut(LEDC, I3);
  if(rp < 1) rp = 1;
  delayMicroseconds(t_wait);
  t1=micros();
  taa = us2cyc(ta);
  tbb = us2cyc(tb);
  //Serial.println(taa);
  //Serial.println(tbb);
  for(int i=0; i<rp; i++) 
  {
    spectro.RunDevice(taa, tbb);
//    Serial.println(i);
  }
  t2=micros();
//  Serial.print("time: ");
//  Serial.println((t2-t1)/1000);
  

}

void scanVar(String s, unsigned long &var)
{
  char r;
  Serial.println(s);
  while(Serial.available() == 0);
  var = Serial.parseInt();
  //Serial.println(var);
  r=Serial.read(); //read "\n"
}

void currentOut(byte ch, int cur)
{
  float volt, r_sense = 0.5;
  
  volt = (float)cur/1000.0*r_sense;
  
  if(ch == 1) {
    ltc2615.write(CH_G, volt);
//    Serial.print("CH_G: ");
//    Serial.println(volt);
  }
  else if(ch == 2) {
    ltc2615.write(CH_F, volt);
//    Serial.print("CH_F: ");
//    Serial.println(volt);
  }
  else if(ch == 3) {
    ltc2615.write(CH_E, volt);
//    Serial.print("CH_E: ");
//    Serial.println(volt);
  } 
}

unsigned long us2cyc(unsigned long t_int)
{
  unsigned long cyc;
  if(t_int < 90) t_int = 90;
  cyc = (unsigned long)t_int/CYCP-48;
//  Serial.println(cyc);
  return cyc;
}

