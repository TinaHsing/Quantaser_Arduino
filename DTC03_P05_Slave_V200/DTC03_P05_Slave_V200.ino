/*Ver.3.00*/

#include <SPI.h>
#include <Wire.h>
#include <AD5541.h>
#include <LTC1865.h>
#include <DTC03_P05v2.h>
#include <PID.h>
#include <EEPROM.h>
#include <DTC03_MS.h>
#define PRINTLOOP 1
#define PIDOUTPUTLIMIT 65535

DTC03 dtc;
PID ipid, tpid;

int iset;
unsigned int i=0;
void setup() {
  // put your setup code here, to run once:
  dtc.SetSPI();
  dtc.SetPinMode();
  dtc.ParamInit();
  dtc.ReadEEPROMnew();
  dtc.DynamicVcc();
  dtc.InitVactArray();
  
  if(dtc.g_sensortype) digitalWrite(SENSOR_TYPE, HIGH);
  else digitalWrite(SENSOR_TYPE, LOW);
  dtc.CheckSensorType();
  dtc.CheckTemp();
//  ipid.Init(65535,32768,0x40000000);
  ipid.Init(32768,32768,0x7FFFFFFF);
  tpid.Init(32768,32768,0x7FFFFFFF);
  dtc.dacforilim.ModeWrite(0);
   
  dtc.g_vset=dtc.g_vact;
  dtc.dacformos.ModeWrite(0);
  
  Wire.begin(DTC03P05);//
  Wire.onReceive(ReceiveEvent);//
  Wire.onRequest(RequestEvent);//
}

void loop() {
  // put your main code here, to run repeatedly:
  int isense; //
  long ioutput,toutput,output;
  int ierr;
  long terr;
  unsigned int pidoffset = dtc.g_tpidoffset*1000;
//  if (i%500==0) {
//  Serial.print("r1 r2 offset: ");
//  Serial.print(dtc.g_r1);
//  Serial.print(", ");
//  Serial.print(dtc.g_r2);
//  Serial.print(", ");
//  Serial.print(dtc.g_tpidoffset);
//  Serial.print(", ");
//  Serial.print(pidoffset);
//  Serial.print(",");
//  Serial.print(dtc.g_mod_status);
//  Serial.print(",");
//  Serial.println(dtc.g_vmodoffset);
    Serial.println(dtc.g_vset_limitt);
//  }
 
  if(dtc.g_sensortype) digitalWrite(SENSOR_TYPE, HIGH);
  else digitalWrite(SENSOR_TYPE,LOW);
  dtc.CheckSensorType();
  dtc.CheckTemp();

  dtc.CurrentLimit();
  isense =abs((int)(dtc.g_itecread)-(int)(dtc.g_isense0));
  ierr = isense - dtc.g_iteclimitset;

  dtc.ReadVoltage();
  dtc.VsetSlow();

  terr = (long)dtc.g_vact - (long)dtc.g_vset_limitt;
//  if (i%500==0) {
//  Serial.println(isense);
//  }
  if(ierr > -20) 
  {
    ioutput=ipid.Compute(dtc.g_en_state, ierr, 58, 1, 2);//kp=58,ki=1,ls=2, 20161116
//     ioutput=ipid.Compute(1, ierr, 58, 1, 2); // use for check EN wrong trigger
    
    while(abs(ioutput)<(abs(toutput)+pidoffset)) //current limit
    {

     output = (long)(abs(ioutput)+dtc.g_fbc_base);//

     if (output>PIDOUTPUTLIMIT) output= PIDOUTPUTLIMIT;//
     if(toutput<=0) dtc.SetMos(HEATING,output);
     else dtc.SetMos(COOLING,output);

     ioutput=ipid.Compute(dtc.g_en_state, ierr, 58, 1, 2); //20161116
//     ioutput=ipid.Compute(1, ierr, 58, 1, 2); // use for check EN wrong trigger

     tpid.g_errorsum=0; // 1112@Adam
     toutput=tpid.Compute(dtc.g_en_state, terr, dtc.g_p, 0, 0); // 1112@Adam, only compare to Pterm
//     toutput=tpid.Compute(1, terr, dtc.g_p, 0, 0); // use for check EN wrong trigger
     
     dtc.CurrentLimit();
     isense =abs((int)(dtc.g_itecread)-(int)(dtc.g_isense0));
     ierr = isense - dtc.g_iteclimitset;

     dtc.ReadVoltage();
     dtc.VsetSlow();

     terr = (long)dtc.g_vact - (long)dtc.g_vset_limitt;      
    }
    
  }
  toutput=tpid.Compute(dtc.g_en_state, terr, dtc.g_p, dtc.g_ki, dtc.g_ls); //20161116
//  toutput=tpid.Compute(1, terr, dtc.g_p, dtc.g_ki, dtc.g_ls); // use for check EN wrong trigger

  output = (long)(abs(toutput)+dtc.g_fbc_base);
  if(output>PIDOUTPUTLIMIT) output=PIDOUTPUTLIMIT;//
  if (toutput<=0) dtc.SetMos(HEATING,output);
  else if (toutput>0) dtc.SetMos(COOLING,output);
  
  if(dtc.g_ee_changed) dtc.SaveEEPROM();
//  Serial.print(dtc.g_vmodoffset);
//  Serial.print(",");
//  Serial.print(dtc.g_vmod);
//  Serial.print(",");
//  Serial.print(dtc.g_vset_limit);
//  Serial.print(",");
//  Serial.println(dtc.g_vset_limitt);
  i++;
}


void ReceiveEvent(int howmany)
{
  dtc.I2CReceive();
}
void RequestEvent()
{ 
  dtc.I2CRequest();
}
