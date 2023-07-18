
#include <DTC03AMaster_V102.h>
#include <openGLCD.h>

DTC03Master master;
void setup() {
  // put your setup code here, to run once:
  analogReference(DEFAULT);
  master.SetPinMode();
  master.ParamInit();
  master.ReadEEPROM();
  master.WelcomeScreen();
  master.BackGroundPrint();
  master.WaitPowerOn();
  master.I2CWriteAll(); //
  master.PrintNormalAll(); //
//  master.CheckStatus(); //
//  master.UpdateEnable(); //  
  
  attachInterrupt(digitalPinToInterrupt(ENC_A), CheckEncoder, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  master.CheckStatus();
  master.CursorState();
  master.blinkTsetCursor();
  master.UpdateParam(); //
  master.SaveEEPROM();
  if(master.MemReload()) {
    master.WaitPowerOn();
    master.I2CWriteAll();
    master.PrintNormalAll();
  }
}

void CheckEncoder()
{
  master.Encoder();
}
