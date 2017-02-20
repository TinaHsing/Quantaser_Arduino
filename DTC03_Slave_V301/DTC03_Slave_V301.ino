  /*Ver.3.01*/

#include <SPI.h>
#include <Wire.h>
#include <AD5541.h>
#include <LTC1865.h>
#include <DTC03_Slave_V300.h>
#include <PID.h>
#include <EEPROM.h>
#include <DTC03_MS.h>
#define PRINTLOOP 1
#define PIDOUTPUTLIMIT 65535
#define ISENSE_GAIN 0

DTC03 dtc;
PID ipid, tpid;

unsigned int i=0;
unsigned long loop_time[5];
void setup() {
  Wire.begin(DTC03P05);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);
  dtc.SetSPI();
  dtc.SetPinMode();
  dtc.ParamInit();
  dtc.DynamicVcc();
  dtc.CheckSensorType();
  dtc.CheckTemp();
  ipid.Init(32768,32768,0x7FFFFFFF);
  tpid.Init(32768,32768,0x7FFFFFFF);
  dtc.dacforilim.ModeWrite(0);
  dtc.dacformos.ModeWrite(0);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  long isense, ierr, iset;
  long ioutput,toutput,output, terr, iteclimit;
//  if (i==5) {
//    i=0;
//    for (int j=0;j<5;j++) Serial.println(loop_time[j]);
//    Serial.println(); 
//  }
//  loop_time[i] = micros();
  i++;

  dtc.ReadVoltage(1);
  dtc.ReadIsense();
  dtc.ReadVpcb();
  dtc.CheckSensorType();
  dtc.CheckTemp();
  dtc.CurrentLimit();
  iteclimit=dtc.g_iteclimitset<<ISENSE_GAIN;
  
  terr = (long)dtc.g_vact - (long)dtc.g_vset_limitt;
  toutput=tpid.Compute(dtc.g_en_state, terr, dtc.g_p, dtc.g_ki, dtc.g_ls); 
  
//  iset=abs(map( toutput,-65535,65535,-300<<ISENSE_GAIN,300<<ISENSE_GAIN));
  iset=abs(toutput*307>>16<<ISENSE_GAIN);
  if(iset > iteclimit) iset=iteclimit;
  
  isense =abs( ( (long)(dtc.g_itecread)-(long)(dtc.g_isense0) )<<ISENSE_GAIN );
  ierr = isense - iset;
  ioutput=ipid.Compute(dtc.g_en_state, ierr, 5, 1, 2);//kp=58,ki=1,ls=2
  
  output = (long)(abs(ioutput)+dtc.g_fbc_base);
  if (output>PIDOUTPUTLIMIT) output= PIDOUTPUTLIMIT;
  
  if(toutput<=0) dtc.SetMos(HEATING,output);
  else dtc.SetMos(COOLING,output);
//  if (i%500==0) {
//      tpid.showParameter();
//      ipid.showParameter();
//      Serial.print(dtc.g_itecavgsum >> AVGPWR);
//      Serial.print(", ");
//      Serial.print(dtc.g_itecread);
//      Serial.print(", ");
//      Serial.print(dtc.g_isense0);
//      Serial.print(toutput);
//      Serial.print(", ");
//      Serial.println(iset);
//  }
  
}


void ReceiveEvent(int howmany)
{
  dtc.I2CReceive();
}
void RequestEvent()
{ 
  dtc.I2CRequest();
}
