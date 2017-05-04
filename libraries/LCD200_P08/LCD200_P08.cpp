#include <Arduino.h>
#include <Wire.h>
//#include <SoftI2C.h>
#include <LTC2451.h>
#include <AD5541.h>
#include <SPI.h>
#include <LCD200_P08.h>
//#include <LCD200_P08_MS.h>
#include <FiveInOne_MS.h>



LCD200::LCD200()
{}
void LCD200::SetPinMode()
{
//    pinMode(ENC2_A, OUTPUT);
//  	pinMode(ENC2_SW, INPUT);
    pinMode(PWR_OFF, OUTPUT);
  	pinMode(LD_EN, OUTPUT);
  	pinMode(ENDAC, OUTPUT);
  	pinMode(VFC1, OUTPUT);
  	pinMode(VFC2, OUTPUT);
  	pinMode(VFC3, OUTPUT);
//  	pinMode(VLD, INPUT);
//  	pinMode(V_SENS, INPUT);
//  	pinMode(LCDSW, INPUT);
}

void LCD200::DACInit()
{
	ad5541.SetPin(ENDAC);
  	ad5541.init();
//  	ad5541.ModeWrite(0);
	ad5541.NormalWrite(65535);
}
void LCD200::PWROnOff(bool en) // set en LOW to turn OFF VCC
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
  SetVCC(VCCLOW);
  ltc2451.SoftI2CInit(SOFTSDAPIN, SOFTSCLPIN, 1);
//  ad5541.NormalWrite(65535); // !!??Need to check why need this line??
  digitalWrite(LD_EN, LOW);
  g_dacoutslow = 65535;
  g_dacout = 65535;
  g_dacoutslow = 65535;
  g_initfinished = 0;
}
void LCD200::ResetFlag()
{
  g_LDOpenFlag =0;
  g_LDShortFlag =0;
  g_AnyErrFlag =0;
  g_checkflag =1;
  g_outerrorcounter =0;
}
void LCD200::readMonitor()
{
	g_vmon = ltc2451.SoftI2CRead();
//	Serial.println(g_vmon);
}
bool LCD200::OpenShortVfCheck()
{
  unsigned int vf, vth1, vth2;
 
  vth1 = (float)g_vfth1*20.46;
  vth2 = (float)g_vfth2*20.46;
  digitalWrite(LD_EN, HIGH); //LD_EN LOW : bypass LD current
  delay(200);
  ad5541.NormalWrite(CHECKCURRENT);
  delay(500);
  vf = analogRead(VLD);
  g_vf = vf;
  ad5541.NormalWrite(65535);
  digitalWrite(LD_EN, LOW);
//  if(g_vmon < OPENVTH) 
//  {
//    g_LDOpenFlag =1;
//    g_AnyErrFlag =1;
//    ad5541.NormalWrite(65535); 
//	PWROnOff(LOW);
//  }
//  else if(vf < VFSHORT)
//  {
//    g_LDShortFlag =1;
//    g_AnyErrFlag =1;
//    ad5541.NormalWrite(65535);
//    PWROnOff(LOW);
//  }
//  else 
//  { 	

//	if(vf >vth2)
//		SetVCC(VCCHIGH);
//	else if(vf > vth1)
//	    SetVCC(VCCMEDIUM);
	PWROnOff(HIGH);
	
	g_checkflag = 0;
//  }
  
  //only for temp test
//  PWROnOff(HIGH);
  //
}

void LCD200::PWRCheck()
{
  unsigned int vplus;
  vplus = analogRead(V_SENS);
  p_vplus = vplus; //I2C test
  if( ((g_dacoutslow == 65535) || (vplus < POWERGOOD)) && (abs(g_ioutset-g_ioutreal)<30) ) digitalWrite(LD_EN, LOW); 
  
//  if(g_com_lden==1 && g_dacoutslow!=65535) digitalWrite(LD_EN, 1);
  
}

bool LCD200::IoutSlow()
{
  long deltaiout;
  unsigned int absdeltaiout;
  
  if(g_dacoutslow != 65535) digitalWrite(LD_EN, HIGH);
  if(g_com_lden) deltaiout = (long)g_dacout-g_dacoutslow; // deltaiout > 0 =>LD current decrease
  									  				// increase Iout->decrease g_dacout
  									  				// decrease Iout->increase g_dacout
  else  deltaiout = 65535-g_dacoutslow;
  absdeltaiout = abs(deltaiout); 
  	
   //Change the dacout slowly
   if(deltaiout > IOUTSTEP) g_dacoutslow += IOUTSTEP; 		
   else if(absdeltaiout <= IOUTSTEP) g_dacoutslow += deltaiout; 	    
   else g_dacoutslow -= IOUTSTEP;
   
   ad5541.NormalWrite(g_dacoutslow);
//   ad5541.NormalWrite(g_dacout);
   g_ioutset = 65535 - g_dacoutslow;
   g_ioutreal = g_vmon;
//  ad5541.NormalWrite(65535);
//  Serial.print(deltaiout);
//  Serial.print(",");
//  Serial.print(g_dacout);
//  Serial.print(",");
//  Serial.println(g_dacoutslow);
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
//  ioutset = 65535- g_dacout;
  ioutset = 65535 - g_dacoutslow;
  deltaiout = ioutset - ioutreal;
  if(abs(deltaiout)> 980)
    g_outerrorcounter ++;
  else g_outerrorcounter=0;
  
  if(g_outerrorcounter > IOUTCOUNTERMAX)
  {
    g_OutErrFlag =1;
//    g_AnyErrFlag =1;
//    Serial.println("c");
  } 
}
void LCD200::OnReceiveEvent()
{
  unsigned char com,temp[2];
  unsigned long t1, t2, deltat;
//  unsigned int deltat;
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
      case LCD200_COM_LDEN:
        g_com_lden = temp[0];
//        Serial.println(F("EN:"));
//        Serial.println(g_com_lden);
      break;
      
      case LCD200_COM_IOUT:
//      	Serial.println(g_AnyErrFlag);
//        if(g_AnyErrFlag) {} // if there is no error status, update the g_dacout
//        else
//        {
        	g_dacout = temp[1] << 8 | temp[0];
//          	Serial.println(F("dac:"));
//          	Serial.println(g_dacout);
//		}
          
      break;

      case LCD200_COM_VFTH1:
        g_vfth1 = temp[0];
//        Serial.println(F("VF1:"));
//        Serial.println(g_vfth1);
      break;

      case LCD200_COM_VFTH2:
        g_vfth2 = temp[0];
        g_initfinished = 1;
//        Serial.println(F("VF2, wakeup:"));
//        Serial.print(g_vfth2);
//        Serial.print(F(", "));
//        Serial.println(g_initfinished);
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
    case LCD200_COM_IIN:
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

//I2C test mode
    case I2C_COM_TEST1:
//    	temp[0]=g_initfinished;
//        temp[0]=32768;
//    	temp[1]=32768>>8;

        temp[0]=g_vfth1;
    	temp[1]=g_vfth1>>8;
//    	temp[0]=g_dacout;
//    	temp[1]=g_dacout>>8;
    break;
    
    case I2C_COM_TEST2:
    	temp[0]=g_dacout;
    	temp[1]=g_dacout>>8;
//        temp[0]=g_vfth2;
//    	temp[1]=g_vfth2>>8;
    break;
    
    case I2C_COM_TEST3:
    	temp[0]=g_com_lden;
    	temp[1]=g_com_lden>>8;
//        temp[0]=g_vf;
//    	temp[1]=g_vf>>8;
    break;
  }
  Wire.write(temp,2);
}

void LCD200::ad5541Test()
{
	ad5541.NormalWrite(65535);
	delay(1000);
	ad5541.NormalWrite(32768);
	delay(1000);
}








