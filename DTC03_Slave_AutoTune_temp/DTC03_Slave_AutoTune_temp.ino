  /*Ver.3.01*/

#include <SPI.h>
#include <Wire.h>
#include <AD5541.h>
#include <LTC1865.h>
#include <PID.h>
#include <DTC03_Slave_V300.h>
//#include <AutoTune_V100.h>
#include <EEPROM.h>
#include <DTC03_MS.h>
#define PRINTLOOP 1
#define PIDOUTPUTLIMIT 65535
#define ISENSE_GAIN 7
#define MAPPING 0.586

/////----Auto tune parameters----//


#define ATUNE_SAMPLING_TIME 50


unsigned int input_auto, output_auto=0, noise_Mid;
float kp_auto,ki_auto;

///////////////////////////
DTC03 dtc;
PID ipid, tpid;

unsigned int i=0 ;
unsigned long loop_time[5];
void setup() {
  
  Wire.begin(DTC03P05);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);
  Serial.begin(9600);
  dtc.SetSPI();
  dtc.SetPinMode();
  dtc.ParamInit();
  dtc.DynamicVcc();
  dtc.CheckSensorType();
  dtc.CheckTemp();
  
  dtc.ReadVoltage(1);
  
  
  ipid.Init(32768,32768,dtc.g_ki,dtc.g_ls,ISENSE_GAIN);
  tpid.Init(50000,32768,1,2,0 );
//  ipid.Init(32768,32768,0x7FFFFFFF,ISENSE_GAIN);
//  tpid.Init(32768,32768,0x7FFFFFFF,0);
  dtc.dacforilim.ModeWrite(0);
  dtc.dacformos.ModeWrite(0);
  
  Serial.print("Look back:");
  Serial.println(MAXLBACK);
  Serial.print("MAXPEAKS:");
  Serial.println(MAXPEAKS);
  Serial.print("Noise band:");
  Serial.println(NOISEBAND);
  Serial.print("FINDBIASARRAY:");
  Serial.println(FINDBIASARRAY);
  Serial.print("TBIAS:");
  Serial.println(TBIAS);
  Serial.print("RUNTIMELIMIT:");
  Serial.println(RUNTIMELIMIT);
//  Serial.println("time, D, A");

}

void loop() {
  
  // put your main code here, to run repeatedly:
  long isense, ierr, iset, iset2;
  long ioutput,toutput,output, terr, iteclimit;

//  Serial.print("g_atune_flag=");
//  Serial.println(dtc.g_atune_flag);
  dtc.ReadVoltage(1);
  dtc.ReadIsense();
  dtc.ReadVpcb();
  dtc.CheckSensorType();
  dtc.CheckTemp();
  if (!dtc.g_atune_flag) 
  {
    dtc.CurrentLimit();
    iteclimit=(long)dtc.g_iteclimitset<<ISENSE_GAIN;
    terr = (long)dtc.g_vact - (long)dtc.g_vset_limitt; 
    toutput=tpid.Compute(dtc.g_en_state, terr, dtc.g_p, dtc.g_ki, dtc.g_ls);  
    iset=abs(toutput*MAPPING);
    if(iset > iteclimit) iset=iteclimit;
    isense =abs( ( (long)(dtc.g_itecread)-(long)(dtc.g_isense0) )<<ISENSE_GAIN );
    ierr = isense - iset;
    ioutput=ipid.Compute(dtc.g_en_state, ierr, 20, 10, 1);//old :kp=58,ki=1,ls=2, new : 20,10,1
    output = (long)(abs(ioutput)+dtc.g_fbc_base);
    if (output>PIDOUTPUTLIMIT) output= PIDOUTPUTLIMIT;
    if(toutput<=0) dtc.SetMos(HEATING,output);
    else dtc.SetMos(COOLING,output);
  }
  if(dtc.g_atune_flag)
  {
    dtc.autotune(kp_auto, ki_auto);
  }
}
void ReceiveEvent(int howmany)
{
  dtc.I2CReceive();
}
void RequestEvent()
{ 
  dtc.I2CRequest();
}
