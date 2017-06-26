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
#define ISENSE_GAIN 7
#define MAPPING 0.586
/////Auto tune parameters//
//#define INPUTGNDLEVEL 1352
//#define OUTPUTGNDLEVEL 1814
#define PNOISEBAND 110 //440
#define NNOISEBAND 100 //430
#define MAXLBACK 20
#define MAXPEAKS 10

int input_auto,kp_auto,lookback = 5; // 3 <= loopback <= MAXLBACK
unsigned int output_auto=0;
byte ki_auto;
///////////////////////////
DTC03 dtc;
PID ipid, tpid;

unsigned int i=0 ;
int Vact_offset;
unsigned long loop_time[5], t_off;
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
  
  dtc.ReadVoltage(1);
  Vact_offset = dtc.g_vact;
  
  ipid.Init(32768,32768,dtc.g_ki,dtc.g_ls,ISENSE_GAIN);
  tpid.Init(50000,32768,1,2,0 );
//  ipid.Init(32768,32768,0x7FFFFFFF,ISENSE_GAIN);
//  tpid.Init(32768,32768,0x7FFFFFFF,0);
  dtc.dacforilim.ModeWrite(0);
  dtc.dacformos.ModeWrite(0);
  Serial.begin(9600);
  t_off = millis();
}

void loop() {
  
  // put your main code here, to run repeatedly:
  long isense, ierr, iset, iset2;
  long ioutput,toutput,output, terr, iteclimit;
//  if (i==5) {
//    i=0;
//    for (int j=0;j<5;j++) Serial.println(loop_time[j]);
//    Serial.println(); 
//  }
//  loop_time[i] = micros();
//  i++;

  dtc.ReadVoltage(1);
  dtc.ReadIsense();
  dtc.ReadVpcb();
  dtc.CheckSensorType();
  dtc.CheckTemp();
  dtc.CurrentLimit();
  iteclimit=(long)dtc.g_iteclimitset<<ISENSE_GAIN;
  
  if(dtc.g_en_state) autotune(&input_auto, &output_auto , &kp_auto, &ki_auto, 5000); //Change the output Amp in the fifth parameter (30 right now)
  
  terr = (long)dtc.g_vact - (long)dtc.g_vset_limitt;
//  Serial.print(dtc.g_vset_limitt);
//  Serial.print(", ");
//  Serial.println( dtc.ReturnTemp(dtc.g_vset_limitt,0),3); 
  toutput=tpid.Compute(dtc.g_en_state, terr, dtc.g_p, dtc.g_ki, dtc.g_ls); 
  
  iset=abs(toutput*MAPPING);
  if(iset > iteclimit) iset=iteclimit;
  
  isense =abs( ( (long)(dtc.g_itecread)-(long)(dtc.g_isense0) )<<ISENSE_GAIN );
  ierr = isense - iset;
  
  ioutput=ipid.Compute(dtc.g_en_state, ierr, 20, 10, 1);//old :kp=58,ki=1,ls=2, new : 20,10,1

//  if(dtc.g_en_state) output = (long)(10000+dtc.g_fbc_base); //22445 ~= 28.5
//  else output = (long) dtc.g_fbc_base;
//  dtc.SetMos(HEATING,output);

//  if (i%500==0) {
//      Serial.print(millis()-t_off);
//      Serial.print(", ");
//      Serial.print(output);
//      Serial.print(", ");
//      Serial.println(dtc.g_vact);
//  }
//  if(i==2000) i=0;
//  i++;
  
}
void autotune(int *in, unsigned int *out, int *kp, byte *ki, unsigned int Outstep)
{
  
  int lastinput[MAXLBACK],peaks[MAXPEAKS];
  int peakcount, peaktype, peakstemp,A;
  bool justchanged,ismax,ismin;
  unsigned long peaktime[MAXPEAKS],now,t1,Pu;
  float Ku;
  unsigned int step_out;
    
  //Variable for simulation
  int i,j=0; 
    
  //Variable initialization

  peakcount = -1;
  peaktype = 0;
  A=0;
  Pu=0;  
    for (i=0;i<MAXLBACK;i++) lastinput[i]=0; // initialize the lastinput array 

  while (peakcount < MAXPEAKS) // 迴圈執行至蒐集滿設定的peak數目為止 
  {
      input_bias(in); // 讀取目前ADC值
      
  
      delay(500);   
      now = micros();
      justchanged = false;
//      if ( (*in-Vact_offset) >PNOISEBAND) *out = 0;  
//      if ( (*in-Vact_offset) <NNOISEBAND) *out = Outstep;
      if ( abs(*in-Vact_offset) >PNOISEBAND) step_out = 0;  
      if ( abs(*in-Vact_offset) <NNOISEBAND) step_out = Outstep;
      Serial.print((float)(millis()-t_off)/1000,1);
      Serial.print(", ");
      Serial.print(abs(*in-Vact_offset));
      Serial.print(", ");
      Serial.print(*in);
      Serial.print(", ");
      output_bias(step_out); //DAC output 
//      Serial.print("in: ");
//      Serial.print(*in);
//      Serial.print(", ");
//      Serial.print(Vact_offset);
//      Serial.print(", ");
      
      
      Serial.print(NNOISEBAND);
      Serial.print(", ");
      Serial.println(PNOISEBAND);
   
      ismax=true;
      ismin=true;
    
//      lookbackloop(*in, lastinput, lookback, &ismax, &ismin);
//      lastinput[0]=*in;
//      peakrecord(*in, &ismax, &ismin, &peaktype, &peakcount, peaks, &peakstemp, peaktime, now, &t1, &justchanged);  
//      if(peakcount >=2 && justchanged ) parameter(&peakcount, peaks, peaktime, &A, &Pu);
//      Ku = 4*Outstep/(A*3.14159);
//      *kp = 0.4*Ku;
//      *ki = 0.48*Ku/Pu;   
  } 
}
void lookbackloop (int input,int *lastinput, int lookback, boolean *ismax, boolean *ismin)
{
  int i;
  for (i=lookback-2;i >=0;i--) 
  {
     if(*ismax) *ismax= input>lastinput[i];
     if(*ismin) *ismin= input <lastinput[i];
     lastinput[i+1]=lastinput[i];   
  }
}

void peakrecord (int input, bool *ismax, bool *ismin, int *peaktype, int *peakcount, int *peaks, int *peakstemp, unsigned long *peaktime, unsigned long now, unsigned long *t1, bool *justchanged)
{

  if(*ismax)
    {
      if(*peaktype == 0) 
      {
        *peaktype =1;
      }
      if(*peaktype ==-1) 
      {
        *peaktype = 1;
        *peakcount+=1;
        peaktime[*peakcount] = *t1;
        peaks[*peakcount]=*peakstemp;
        *justchanged = true;
        
      }
      *t1 = now;
      *peakstemp = input;
  
    }
    if(*ismin)
    {
      if(*peaktype ==0) 
      {
        *peaktype =-1;
      }
      if(*peaktype == 1) 
      {
        *peaktype = -1;
        *peakcount +=1;
        peaktime[*peakcount] = *t1;
        peaks[*peakcount]=*peakstemp;
        *justchanged = true;
      }
      *t1 = now;
      *peakstemp = input;     
    }   
}
void parameter(int *peakcount, int *peaks, unsigned long *peaktime, int *A, unsigned long *Pu) //peakcount should >=2
{
  int Atemp;
  unsigned long Putemp;
  Atemp = abs(peaks[*peakcount]-peaks[*peakcount-1]);
  Putemp = peaktime[*peakcount]-peaktime[*peakcount-2];
  
  if (*A ==0) *A = (abs(peaks[*peakcount]-peaks[*peakcount-1])+abs(peaks[*peakcount-1]-peaks[*peakcount-2])) >>1;
  else *A = (*A+Atemp) >>1;
  
  if (*Pu==0) *Pu = Putemp;
  else *Pu = (*Pu+Putemp)>>1; 
  
}

void input_bias(int *in) //read input from ADC and cancel the bias
{
//  while((ADC->ADC_ISR&0x80) == 0); // wait for AD conversion is complete 
//  *in = ADC->ADC_CDR[7]-INPUTGNDLEVEL; //Read ADC value to *in
  dtc.ReadVoltage(1);
  *in = (int)dtc.g_vact; //Read ADC value to *in
  
}
void output_bias(unsigned int Out)// adjust the output bias and write it to DAC
{
  
  Out = Out +dtc.g_fbc_base;
  dtc.SetMos(COOLING,Out);
  Serial.print(Out);
  Serial.print(", ");
  //Serial.println(*Out);
  //delay(500);
//  DACC->DACC_CDR = (1<<12) | *Out; // Analog Write function with faster speed 
  
}
void ReceiveEvent(int howmany)
{
  dtc.I2CReceive();
}
void RequestEvent()
{ 
  dtc.I2CRequest();
}
