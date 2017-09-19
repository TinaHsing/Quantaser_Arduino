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
#define PIDOUTPUTLIMIT 48000 //set MAX duty here, Itec must < 5A 

DTC03 dtc;
PID ipid, tpid;

unsigned int i=0;
unsigned long loop_time[5];
void setup() {
  Serial.begin(9600);
  Wire.begin(DTC03P05);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);

  dtc.SetSPI();
  dtc.SetPinMode();
  dtc.ParamInit();
  dtc.DynamicVcc();
  if(dtc.g_sensortype) digitalWrite(SENSOR_TYPE, HIGH);
  else digitalWrite(SENSOR_TYPE, LOW);
  dtc.CheckSensorType();
  dtc.CheckTemp();
  tpid.Init(32768, 32768, dtc.g_ki, dtc.g_ls, 0 );
  dtc.dacforilim.ModeWrite(0);
  dtc.dacformos.ModeWrite(0);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  int isense; //
  long ioutput,toutput,output;
  int ierr;
  long terr;
  
  unsigned int pidoffset = dtc.g_tpidoffset*1000;
  
//  if (i==5) {
//    i=0;
//    for (int j=0;j<5;j++) Serial.println(loop_time[j]);
//    Serial.println(); 
//  }
//  loop_time[i] = micros();
 
//  if(dtc.g_sensortype) digitalWrite(SENSOR_TYPE, HIGH);
//  else digitalWrite(SENSOR_TYPE,LOW);
  
  dtc.ReadVoltage(1);
  dtc.ReadIsense();
  dtc.ReadVpcb();

  dtc.CheckTemp();
  terr = (long)dtc.g_vact - (long)dtc.g_vset_limitt;
//  if (i%2000==0) {
//    Serial.print(dtc.ReturnTemp(dtc.g_vact,0));
//    Serial.print(", ");
//    Serial.println(dtc.ReturnTemp(dtc.g_vset_limitt,0));
//    Serial.println(dtc.g_overshoot);
//  }
  if (dtc.g_overshoot == 1)
  {
    dtc.g_overshoot = 0;
//    tpid.g_errorsum >>= 1;
  }
  
  toutput=tpid.Compute(dtc.g_en_state, terr, dtc.g_p, dtc.g_ki, dtc.g_ls); 
  output = (long)(abs(toutput)+dtc.g_fbc_base)>>1;
  if(output>PIDOUTPUTLIMIT) output=PIDOUTPUTLIMIT;//
  if (toutput<=0) dtc.SetMos(HEATING,output);
  else if (toutput>0) dtc.SetMos(HEATING,0);  
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
