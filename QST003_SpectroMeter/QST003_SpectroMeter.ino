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

File writeFile;
char FileName[12] = "data001.bin";

void setup() {
  Serial.begin(115200);
  ltc2615.init();
  spectro.SpectroInit(CLKA, STA, CLKB, STB, ADCCONV, ADC_CHA);
  currentOut(LEDA, 0);
  currentOut(LEDB, 0);
  currentOut(LEDC, 0);

  if (!SD.begin(SD_CSPIN)) {
    Serial.println("SD initialization failed!");
  }

}

void loop() {
  unsigned long I1, I2, I3, ta, tb, t_wait, rp;
  unsigned long taa, tbb, t1, t2;
  int iFileNum = 1;
  bool bFileFull = true;

    while(Serial.available()==0);
    Serial.read();
    scanVar("I1:", I1); 
    scanVar("I2:", I2); 
    scanVar("I3:", I3); 
    scanVar("Ta:", ta); 
    scanVar("Tb:", tb); 
    scanVar("Twait:", t_wait); 
    scanVar("Repeat:", rp);

#if DEBUG
  delay (20);
  I1 = 150;
  I2 = 150;
  I3 = 150;
  ta = 1000;
  tb = 1000;
  t_wait = 1000;
  rp = 1;
#endif
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

  if (!SD.exists("data001.bin"))
  {
    iFileNum = 1;
    bFileFull = false;
  }
  else
  {
    for (int f = 2; f <= 999; f++)
    {
      sprintf(FileName, "data%03d.bin", f);
      if (!SD.exists(FileName))
      {
        iFileNum = f;
        bFileFull = false;
        break;
      }
      else
      {
        Serial.print(FileName);
        Serial.println(" exists");
      }
    }
  }

  if (bFileFull)
  {
    Serial.println("Please remove data001.bin to data999.bin in SD card");
#if 0
    for(int i = 0; i < rp; i++) 
    {
      spectro.RunDevice(taa, tbb, WriteSerial, writeFile);
    }
#endif
  }
  else
  {    
    sprintf(FileName, "data%03d.bin", iFileNum);
    Serial.print(FileName);
    Serial.println(" is OK");
    writeFile = SD.open(FileName, FILE_WRITE);

#if 0
    for(int i = 0; i < rp; i++) 
    {
    if (writeFile)
      spectro.RunDevice(taa, tbb, WriteSD, writeFile);
    else
      spectro.RunDevice(taa, tbb, WriteSerial, writeFile);
    }
    // close the file:
    writeFile.close();
  }

  t2 = micros();
//  Serial.print("time: ");
//  Serial.println((t2-t1)/1000);
#endif
  
  }

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

