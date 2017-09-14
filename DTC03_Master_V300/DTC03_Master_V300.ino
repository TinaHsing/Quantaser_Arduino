
#include <DTC03Master_V300.h>
#include <DTC03_MS.h>
#include <EEPROM.h>
//#include <Wire.h>
#include <openGLCD.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Iain5x7.h>
#include <fonts/fixed_bold10x15.h>
<<<<<<< HEAD
unsigned int j=0;
=======

>>>>>>> master
DTC03Master master;
void setup() {
  // put your setup code here, to run once:
  float tset, tact, itec;
  unsigned int vact_mv;
  int itec_mv;
<<<<<<< HEAD
  
=======
>>>>>>> master
  analogReference(DEFAULT);
  master.SetPinMode();
  master.ParamInit();
  master.ReadEEPROM();
  master.I2CWriteAll();
  master.WelcomeScreen();
  master.BackGroundPrint();
  master.PrintNormalAll();
  master.CheckStatus();
  master.UpdateEnable();
<<<<<<< HEAD
=======
  
>>>>>>> master
  attachInterrupt(digitalPinToInterrupt(ENC_A),CheckEncoder,CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B),CheckEncoder,CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
<<<<<<< HEAD
  master.CheckStatus();
=======
  
//  if(master.g_cursorstate<7) //do it when normal-mode
//  {
    master.CheckStatus();
    //delay(400);//20161103
//  }
>>>>>>> master
  master.UpdateEnable(); 
  master.CursorState();
  master.HoldCursortate();
  master.blinkTsetCursor();
  master.UpdateParam();
  master.SaveEEPROM();
<<<<<<< HEAD
  
=======

>>>>>>> master
}
void CheckEncoder()
{
  master.Encoder();
}


