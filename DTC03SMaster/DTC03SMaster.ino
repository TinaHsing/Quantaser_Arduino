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
  float tact;
  master.SetPinMode();
  master.ParamInit();
  master.WelcomeScreen();
  master.ReadEEPROM();
  master.I2CWriteAll();
//  master.I2CWriteData(I2C_COM_CTR);// Seting Pgain and current limit
//  master.I2CWriteData(I2C_COM_VBEH);/// Change to R1 R2 setting
//  master.I2CWriteData(I2C_COM_VBEC); // Setting TPIDOFF (tpidoffset)
//  master.I2CWriteData(I2C_COM_FBC); // set gate threshould voltage for NMOS
//  master.I2CWriteData(I2C_COM_KI); // set KI and LS
//  master.I2CWriteData(I2C_COM_VSET); // Set the Vset
  master.PrintBG();
  master.PrintTstart();
  master.PrintTend();
  master.PrintRate();
  master.CheckStatus();
  master.UpdateEnable();
  master.PrintEnable();
  master.CheckScan();
  master.PrintScan();
  attachInterrupt(digitalPinToInterrupt(ENC_B), CheckEncoder, RISING);
 
}

void loop() {
//  master.g_tloop = millis();
//  master.Printloopt(master.g_tloop);
//
 master.CheckStatus();
 master.UpdateEnable();
 master.CheckScan();
 master.checkTnowStatus();
 master.CursorState(); //Check which cursor state it is
 master.ShowCursor(); 
 master.UpdateParam();
 master.CalculateRate();
 master.SaveEEPROM();
 //
 master.RuntestI2C();
 

}
void CheckEncoder()
{  
  master.Encoder();
}
