
#include <DTC03IB_Master_V100.h>
#include <DTC03IB_MS.h>
#include <EEPROM.h>
//#include <Wire.h>
#include <openGLCD.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Iain5x7.h>
#include <fonts/fixed_bold10x15.h>

DTC03Master master;
void setup() {
  // put your setup code here, to run once:
  float tset, tact, itec;
  unsigned int vact_mv;
  int itec_mv;
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
  
  attachInterrupt(digitalPinToInterrupt(ENC_A),CheckEncoder,CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B),CheckEncoder,CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  
//  if(master.g_cursorstate<7) //do it when normal-mode
//  {
    master.CheckStatus();
    //delay(400);//20161103
//  }
#if ENABLE_REMOTE_MODE //sherry++ 2017.9.29
  if (!master.CheckSlaveRemote())
    master.UpdateEnable(); 
#else
  master.UpdateEnable(); 
#endif
  master.CursorState();
  master.HoldCursortate();
  master.blinkTsetCursor();
#if ENABLE_REMOTE_MODE //sherry++ 2017.9.29
  if (!master.CheckSlaveRemote())
    master.UpdateParam();
#else
  master.UpdateParam();
#endif
  master.SaveEEPROM();

}
void CheckEncoder()
{
  master.Encoder();
}


