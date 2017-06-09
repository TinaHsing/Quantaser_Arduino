#include <DTC03S_Master.h>
#include <openGLCD.h>
#include <EEPROM.h>
#include <DTC03_MS.h>
#include <Wire.h>
#include "Arduino.h"
DTC03SMaster master;
unsigned char vstep;
float tstep, frate;


void setup() {
  float tact;
  master.SetPinMode();
  master.ParamInit();
  master.ReadEEPROM();
  master.I2CWriteAll();
  master.WelcomeScreen();
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
//  Timer1Init();
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
// master.RuntestI2C();
}

void CheckEncoder()
{  
  master.Encoder();
}
//void Timer1Init()
//{
//  noInterrupts();
//  TCCR1A = 0;
//  TCCR1B = 0;
//  TCNT1 = 0;
//
//  OCR1A = 6250; //10Hz=100ms update time
//  TCCR1B |= (1 << WGM12);
//  TCCR1B |= (1<<CS12);
//  TIMSK1 |= (1<<OCIE1A);
//  interrupts();
//}
//ISR(TIMER1_COMPA_vect)
//{
//  master.Timer();
//}


