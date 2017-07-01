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

/////----Auto tune parameters----//
//#define PNOISEBAND 110 //440
//#define NNOISEBAND 100 //430
#define NOISEBAND 7
#define MAXLBACK 15
#define MAXPEAKS 5
#define STEPD 100
#define T_THERESTHOLD 40
#define ATUNE_BIAS 48200
#define ATUNE_SAMPLING_TIME 0
#define MV_STATUS 0
int lookback = MAXLBACK; // 3 <= loopback <= MAXLBACK
unsigned int input_auto, output_auto=0, noise_Mid;
float kp_auto,ki_auto;

///////////////////////////
DTC03 dtc;
PID ipid, tpid;

unsigned int i=0 ;
int Vact_offset;
unsigned long loop_time[5], t_off;
boolean atune_flag=1, init_flag=1, FindTtheresholdTimeFlag=1;
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
  
  
  ipid.Init(32768,32768,dtc.g_ki,dtc.g_ls,ISENSE_GAIN);
  tpid.Init(50000,32768,1,2,0 );
//  ipid.Init(32768,32768,0x7FFFFFFF,ISENSE_GAIN);
//  tpid.Init(32768,32768,0x7FFFFFFF,0);
  dtc.dacforilim.ModeWrite(0);
  dtc.dacformos.ModeWrite(0);
//  Serial.begin(9600);
  Serial.print("Look back:");
  Serial.println(lookback);
  Serial.print("Noise band:");
  Serial.println(NOISEBAND);
  Serial.print("STEPD:");
  Serial.println(STEPD);
  Serial.print("ATUNE_BIAS:");
  Serial.println(ATUNE_BIAS);
  Serial.println("------------------------");
  Serial.println(F("Time(s), Err(code), out(code), peak type, peak#, t, Vact"));
//  Serial.println("time, D, A");
  t_off = millis();
}

void loop() {
  
  // put your main code here, to run repeatedly:
  long isense, ierr, iset, iset2;
  long ioutput,toutput,output, terr, iteclimit;
  

  dtc.ReadVoltage(1);
  dtc.ReadIsense();
  dtc.ReadVpcb();
  dtc.CheckSensorType();
  dtc.CheckTemp();
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
  
  //-----searching ATUNE_BIAS-----//
  if(toutput<=0) dtc.SetMos(HEATING,output);
  else dtc.SetMos(COOLING,output);
//  if(i==2000)
//  {
//    Serial.println(output);
//    i=0;
//  }
//  i++;
//---------for auto tune test-------------//
//  if (!dtc.g_en_state) 
//    {
//      atune_flag = 1;
//      init_flag = 1;
//      output_bias(ATUNE_BIAS,0);
//      t_off = millis();
//      Vact_offset = dtc.g_vact;
//      FindTtheresholdTimeFlag=1;
//    }
//    if(dtc.g_en_state)
//    {
//  //    if(FindTtheresholdTimeFlag) FindTtheresholdTime();
//      if(atune_flag) autotune(&input_auto, &output_auto , &kp_auto, &ki_auto, STEPD); //Change the output Amp in the fifth parameter (30 right now)
//    }
//-------------end of auto tune test----------------------//

}

void FindTtheresholdTime()
{
  output_bias(8000,0);
//  Serial.print( (float)(millis()-t_off)/1000, 2);
//  Serial.print(", ");
//  Serial.println (abs((int)dtc.g_vact-Vact_offset) );
  if( abs((int)dtc.g_vact-Vact_offset)>T_THERESTHOLD ) 
  {
    Serial.println((float)(millis()-t_off)/1000, 3);
    FindTtheresholdTimeFlag = 0;
  }
  
}
void autotune(unsigned int *in, unsigned int *out, float *kp, float *ki, unsigned int Outstep)
{ 
  int lastinput[MAXLBACK],peaks[MAXPEAKS];
  int peakcount, peaktype, peakstemp,A;
  bool justchanged,ismax,ismin;
  unsigned long peaktime[MAXPEAKS],now,t1,Pu;
  float Ku, ki2;
  unsigned int step_out;
    
  //Variable for simulation
  int i,j=0; 
    
  //Variable initialization

  peakcount = -1;
  peaktype = 0;
  A=0;
  Pu=0;  
    for (i=0;i<MAXLBACK;i++) 
    {
      dtc.ReadVoltage(1);     
      lastinput[i]=dtc.g_vact;; // initialize the lastinput array 
    }

  while (peakcount < (MAXPEAKS-1)) // 迴圈執行至蒐集滿設定的peak數目為止 
  {
      input_bias(in); // 讀取目前ADC值  
      now = millis();
      justchanged = false;
      // relay test//////////////////////////////////
      if ( (int)(*in-noise_Mid) >NOISEBAND) 
      {
        step_out = ATUNE_BIAS - Outstep/2;  //TEC
//        step_out = ATUNE_BIAS + Outstep/2;  //Heater
        init_flag = 0;
      }
      else if ( (int)(*in-noise_Mid) <-NOISEBAND) 
      {
        step_out = ATUNE_BIAS + Outstep/2; //TEC
//        step_out = ATUNE_BIAS - Outstep/2;  
      }
      else if (init_flag)
      {
        step_out = ATUNE_BIAS + Outstep/2;
//        step_out = ATUNE_BIAS - Outstep/2;
      }
      ///////////////atune data print-1/////////////////////////////////////
      Serial.print((float)(now-t_off)/1000,3);
      Serial.print(", ");
//      Serial.print(*in);
//      Serial.print(", ");
//      Serial.print((int)noise_Mid);
//      Serial.print(", ");
      Serial.print((int)(*in-noise_Mid));     
      Serial.print(", ");

      output_bias(step_out,1); //DAC output    
      ismax=true;
      ismin=true;
      lookbackloop(*in, lastinput, lookback, &ismax, &ismin);
      lastinput[0]=*in;
      peakrecord(*in, &ismax, &ismin, &peaktype, &peakcount, peaks, &peakstemp, peaktime, now, &t1, &justchanged);  
      if(peakcount >=2 && justchanged ) 
      {
        parameter(&peakcount, peaks, peaktime, &A, &Pu);
        Ku = 4*Outstep/(A*3.14159);      
        *kp = 0.4*Ku;
        *ki = 480.0*Ku/(float)Pu; 
        ki2 = 1000.0*Ku/(float)Pu;
        if(peakcount == MAXPEAKS-1)
        {
          Serial.print("A=,");
          Serial.println(A);
          Serial.print("stepD=,");
          Serial.println(Outstep);
          Serial.print("Ku=,");
          Serial.println(Ku);
          Serial.print("Pu=,");
          Serial.println(Pu);
          Serial.print("kp=,");
          Serial.println(*kp);
          Serial.print("ki=,");
          Serial.println(*ki);
          Serial.print("ki2=,");
          Serial.println(ki2);
          Serial.print("TC=,");
          Serial.println((float)1.0/(*ki),1); 
          Serial.print("Tc2=,");
          Serial.println(1.0/ki2,1);
        }
      }       
      delay(ATUNE_SAMPLING_TIME);    
  } 
}
void lookbackloop (int input,int *lastinput, int lookback, boolean *ismax, boolean *ismin) //如果在指定的lookback 數目裡有發現possible maxima(minima)的話*ismax(*ismin)才會是T, 否則為F
{
  int i;
//  Serial.print("input,");
//  Serial.print(input);
  for (i=lookback-2;i >=0;i--) 
  {
     if(*ismax) *ismax= input>lastinput[i];
     if(*ismin) *ismin= input<lastinput[i];
     lastinput[i+1]=lastinput[i];   
//     Serial.print(",");
//     Serial.print(lastinput[i]);    
  }
  //////atune data print-3/////
  Serial.print(",");
  if(*ismax) Serial.print("1");
  else if (*ismin) Serial.print("-1");
  else Serial.print("0");
}

void peakrecord (int input, bool *ismax, bool *ismin, int *peaktype, int *peakcount, int *peaks, int *peakstemp, unsigned long *peaktime, unsigned long now, unsigned long *t1, bool *justchanged)
{
  //當*ismax or *ismin為T時，紀錄peak之時間與振幅，只有第一次的*ismax or *ismin是T時才給定*peaktype值(1 or -1)，而當*peaktype變號時則代表發現peak了。
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
        //////atune data print-4/////
        Serial.print(", ");
        Serial.print("min #");
        Serial.print(*peakcount);
        Serial.print(", ");
        Serial.print(peaktime[*peakcount]);       
        Serial.print(", ");
        Serial.print(peaks[*peakcount]);  
      }
      Serial.println();
      *t1 = now;
      *peakstemp = input;    
  }
  else if(*ismin)
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
        //////atune data print-5/////
        Serial.print(", ");
        Serial.print("max #");
        Serial.print(*peakcount);
        Serial.print(", ");
        Serial.print(peaktime[*peakcount]);       
        Serial.print(", ");
        Serial.print(peaks[*peakcount]);        
      }
      Serial.println();
      *t1 = now;
      *peakstemp = input;         
  }   
  else
  {
    Serial.println();
  }
  if (*peakcount == MAXPEAKS-1)
      {
        //////atune data print-final/////
        Serial.println("T/2,  A: ");
        for(int i=0;i<MAXPEAKS-1;i++)
        {              
          Serial.print((float)(peaktime[i+1]-peaktime[i])/1000,1);
          Serial.print(", ");
          Serial.println(abs(peaks[i+1]-peaks[i]));
        }
        atune_flag = 0;
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

void input_bias(unsigned int *in) //read input from ADC and cancel the bias
{
  dtc.ReadVoltage(1);
  if(MV_STATUS) *in = (int)dtc.g_vact_MV; //Read ADC value to *in
  else *in = (int)dtc.g_vact;
  
}
void output_bias(unsigned int Out, bool mode)// adjust the output bias and write it to DAC
{
  
//  Out = Out +dtc.g_fbc_base;

  dtc.SetMos(COOLING,Out);
  if(mode)
  {
    //////atune data print-2/////
    Serial.print(Out);
    Serial.print(", ");
  }
  else
  {
//    Serial.print(Out);
//    Serial.print(", ");    
//    Serial.println(dtc.g_vact);
    if(MV_STATUS) noise_Mid = dtc.g_vact_MV;
    else noise_Mid = dtc.g_vact;
    
//    delay(500);
  }
  
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
