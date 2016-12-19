#include <DTC03S_Master.h>
#include <openGLCD.h>
#include <EEPROM.h>
#include <DTC03_MS.h>
#include <Wire.h>
//test

DTC03SMaster master;
unsigned char vstep;
float tstep, frate;
void setup() {
  // put your setup code here, to run once:
  float tact;
  master.SetPinMode();
  master.ParamInit();
  master.WelcomeScreen();
  master.ReadEEPROM();
  master.I2CWriteData(I2C_COM_CTR);// Seting Pgain and current limit
  master.I2CWriteData(I2C_COM_VBEH);/// Change to R1 R2 setting
  master.I2CWriteData(I2C_COM_VBEC); // Setting TPIDOFF (tpidoffset)
  master.I2CWriteData(I2C_COM_FBC); // set gate threshould voltage for NMOS
  master.I2CWriteData(I2C_COM_KI); // set KI and LS
  master.I2CWriteData(I2C_COM_VSET); // Set the Vset
  master.PrintBG();
  master.PrintTstart();
  master.PrintTend();
  master.PrintRate();
  master.CheckVact(); // Check Vact from slave and print
  master.PrintScan();
  master.PrintEnable();
  attachInterrupt(digitalPinToInterrupt(ENC_B), CheckEncoder, RISING);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  master.g_tloopstart = millis();
 master.CheckVact();
 master.UpdateEnable();
 master.CheckScan();
 master.checkTnowStatus();
// master.PrintTnow();
 master.CheckStatus(); //Check which cursor state it is
 master.ShowCursor(); 
 master.UpdateParam();
 master.CalculateRate();
 master.g_tloopend = millis();
 master.Printloopt(master.g_tloopstart, master.g_tloopend);

}
void CheckEncoder()
{  
  master.Encoder();
}
