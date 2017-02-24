#include <Arduino.h>
#include <Wire.h>
#include <SoftI2C.h>
#include <LTC2451.h>
#include <AD5541.h>
#include <SPI.h>
#include <LCD200_P08.h>
#include <LCD200_P08_MS.h>



LCD200::LCD200()
{}
void LCD200::SetPinMode()
{
    pinMode(ENC2_A, OUTPUT);
  	pinMode(ENC2_SW, INPUT);
    pinMode(PWR_OFF, OUTPUT);
  	pinMode(LD_EN, OUTPUT);
  	pinMode(ENDAC, OUTPUT);
  	pinMode(VFC1, OUTPUT);
  	pinMode(VFC2, OUTPUT);
  	pinMode(VFC3, OUTPUT);
  	pinMode(VLD, INPUT);
  	pinMode(V_SENS, INPUT);
  	pinMode(LCDSW, INPUT);
}

void LCD200::DACInit()
{
	ad5541.SetPin(ENDAC);
  ad5541.init();
	ad5541.NormalWrite(65535);
}
void LCD200::PWROnOff(bool en)
{
  if (en)
    digitalWrite(PWR_OFF, LOW);
  else
    digitalWrite(PWR_OFF, HIGH);
}
void LCD200::SetVCC(unsigned char vcc) 
{
  if(vcc == VCCLOW)
  {
    digitalWrite(VFC1, LOW);
    digitalWrite(VFC2, LOW);
    digitalWrite(VFC3, HIGH); 
  }
  if(vcc == VCCMEDIUM)
  {
    digitalWrite(VFC1, LOW);
    digitalWrite(VFC2, HIGH);
    digitalWrite(VFC3, LOW); 
  }
  if(vcc == VCCHIGH)
  {
    digitalWrite(VFC1, HIGH);
    digitalWrite(VFC2, LOW);
    digitalWrite(VFC3, LOW); 
  }
}
void LCD200::AnaBoardInit()
{
  PWROnOff(LOW);
  SetVCC(LOW);
  ltc2451.SoftI2CInit(SOFTSDAPIN, SOFTSCLPIN);
  ad5541.NormalWrite(65535); // !!??Need to check why need this line??
  digitalWrite(LD_EN, LOW);


}
void LCD200::ResetFlag()
{
  g_LDOpenFlag =0;
  g_LDShortFlag =0;
  g_initfinished =0;
  g_AnyErrFlag =0;
  g_checkflag =1;
  g_dacoutslow = 65535;
  g_dacout = 65535;
  g_outerrorcounter =0;
}
bool LCD200::OpenShortVfCheck()
{
  unsigned int vf;
 
  digitalWrite(LD_EN, HIGH);
  delay(200);
  ad5541.NormalWrite(CHECKCURRENT);
  delay(500);
  g_vmon = ltc2451.SoftI2CRead(); // !!??Check if read twice is necessary!!
  vf = analogRead(VLD);
  if(g_vmon < OPENVTH) 
  {
    g_LDOpenFlag =1;
    g_AnyErrFlag =1;
    ad5541.NormalWrite(65535); 

  }
  if(vf < VFSHORT)
  {
    g_LDShortFlag =1;
    g_AnyErrFlag =1;
    ad5541.NormalWrite(65535);
  }
  PWROnOff(HIGH);
  if(vf >g_vfth2)
    SetVCC(VCCHIGH);
  else if(vf > g_vfth1)
    SetVCC(VCCMEDIUM);
  g_checkflag = 0;
}

void LCD200::PWRCheck()
{
  unsigned int vplus;
  vplus = analogRead(V_SENS);
  if((g_dacoutslow == 65535) || (vplus < POWERGOOD))
    digitalWrite(LD_EN, LOW);
  
}

bool LCD200::IoutSlow()
{
  int deltaiout;
  unsigned int absdeltaiout;
  deltaiout = g_dacout- g_dacoutslow;
  absdeltaiout = abs(deltaiout);

  //Change the dacout slowly
  if(deltaiout > IOUTSTEP)
    g_dacoutslow += IOUTSTEP;
  else if(absdeltaiout < IOUTSTEP)
    g_dacoutslow += absdeltaiout;
  else
    g_dacoutslow -= IOUTSTEP;

  ad5541.NormalWrite(g_dacoutslow);
  // Check if program is in slow state.......
  if(absdeltaiout == 0)
    return 0;
  else return 1;
}

void LCD200::CheckOutputErr() // Need to be use while IoutSlow = False
{
  unsigned int vf, ioutreal, ioutset;
  int deltaiout;
  vf = analogRead(VLD);
  ioutreal = ltc2451.SoftI2CRead(); 
  ioutset = 65535- g_dacout;
  deltaiout = ioutset - ioutreal;
  if(abs(deltaiout)> IOUTSTEP)
    g_outerrorcounter ++;
  
  if(g_outerrorcounter > IOUTCOUNTERMAX)
  {
    g_OutErrFlag =1;
    g_AnyErrFlag =1;
  } 
}
void LCD200::OnReceiveEvent()
{
  unsigned char com,temp[2];
  unsigned long t1, t2;
  unsigned int deltat;
  while(Wire.available()==3)
  {
    t1= micros();
    com = Wire.read();
    temp[0] = Wire.read();
    temp[1] = Wire.read();
    t2 = micros();
    deltat = t2-t1;
  }

  if(deltat < I2CREADTIMEMAX)
  {
    switch(com)
    {
      case LCD200_COM_IOUT:
        if((g_AnyErrFlag == 0)) // if there is no error status, update the g_dacout
          g_dacout = temp[0] << 8+ temp[1];
      break;

      case LCD200_COM_VFTH1:
        g_vfth1 = temp[0] <<8 + temp[1];
      break;

      case LCD200_COM_VFTH2:
        g_vfth2 = temp[0] <<8 + temp[1];
        g_initfinished = 1;
      break;

      case LCD200_COM_LDEN:
        g_com_lden = temp[1];
      break;
    }
  }
}
void LCD200::OnRequestEvent()
{
  unsigned char com,temp[2];

  while(Wire.available() == 1)
    com = Wire.read();
  switch(com)
  {
    case LCD200_COM_IOUT:
      temp[1] = g_vmon >> 8;
      temp[0] = g_vmon;
    break;

    case LCD200_COM_ERR:
      if(g_LDShortFlag) temp[0] |= LCD200_ERRMASK_LDOPEN;
      else temp[0] &= (~LCD200_ERRMASK_LDOPEN);
      
      if(g_LDOpenFlag) temp[0] |= LCD200_ERRMASK_LDSHORT;
      else temp[0] &= (!LCD200_ERRMASK_LDSHORT);
      temp[1] = 0;

      if(g_OutErrFlag) temp[0] |= LCD200_ERRMASK_OUTERR;
      else temp[0] &= (~LCD200_ERRMASK_OUTERR);
    break;
  }
  Wire.write(temp,2);
}








