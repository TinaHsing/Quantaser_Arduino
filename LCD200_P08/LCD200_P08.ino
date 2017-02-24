#include <Wire.h>
#include <SoftI2C.h>
#include <LTC2451.h>
#include <SPI.h>
#include <AD5541.h>
#include <LCD200_P08_MS.h>
#include <LCD200_P08.h>


LCD200 lcd200;
bool g_first_turn_on_flag;
void setup() {
  // put your setup code here, to run once:
  lcd200.SetPinMode();
  lcd200.DACInit();
  lcd200.AnaBoardInit();
  lcd200.ResetFlag();
  Wire.begin(LCD200I2CSLAVEADD);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);
  while(~lcd200.g_initfinished) delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(lcd200.g_com_lden)
  {
  
    if(lcd200.g_checkflag)
      lcd200.OpenShortVfCheck();
    lcd200.PWRCheck();
    if(lcd200.IoutSlow())
    {}
    else
      lcd200.CheckOutputErr();
   }
  else  
    lcd200.ResetFlag();
}
void ReceiveEvent(int howmany)
{
  lcd200.OnReceiveEvent();
}
void RequestEvent()
{
  lcd200.OnRequestEvent();
}
