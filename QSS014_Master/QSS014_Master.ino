#include <Wire.h>
#include "QSS014_cmn.h"
#include <LTC2451.h>
#include <DTC03_MS.h>

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/********number of uart command number **********/
#define COMMAND_NUM 6
#define I2CSENDDELAY 100 //delay100us
#define I2CREADDELAY 100 //delay100us
#define TEST_MODE false

#define fbcbase1    23400
#define vmodoffset1 32768
#define Rmeas1      34500

#define fbcbase2    23400
#define vmodoffset2 32768
#define Rmeas2      34500

/********glogal variable***************/
int g_freq, g_phase;
int g_time[3], g_loop, g_temp[3];
unsigned int g_vact, g_vset, g_otp, g_Rmeas, g_bconst, g_fbcbase, g_vmodoffset;
unsigned char g_p, g_ki, g_currentlim, g_tpidoff, g_r1, g_r2, g_kiindex, g_stableCode_atune;


typedef struct table {
  char *cmd;
  void (*action)(char *);
} table_t;

table_t cmd_list[COMMAND_NUM];
LTC2451 adc;

void setup() {
/***** register command and act function here ******/
  cmd_list[0].cmd = "MOD_FREQ";
  cmd_list[0].action = ACT_setModFreq;

  cmd_list[1].cmd = "MOD_PHASE";
  cmd_list[1].action = ACT_setModPhase;

  cmd_list[2].cmd = "READ_ADC";
  cmd_list[2].action = ACT_readAdc;

  cmd_list[3].cmd = "SET_TIME";
  cmd_list[3].action = ACT_setTime;

  cmd_list[4].cmd = "SET_LOOP";
  cmd_list[4].action = ACT_setLoop;

  cmd_list[5].cmd = "SET_TEMP";
  cmd_list[5].action = ACT_setTemp;

/****************************************************/

  adc.Init(MODE60HZ);
  
  Serial.begin(115200);
  Wire.begin();
}

void loop() {

  if (stringComplete)
  {
    char *c_inputString = (char*)inputString.c_str();
#if TEST_MODE
    Serial.print("loop: ");
    Serial.println(inputString);
#endif
    match_cmd(c_inputString, cmd_list);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}


void match_cmd(char *input_string, table_t cmd[])
{
  for(int i=0; i<COMMAND_NUM; i++)
  {
    if (strstr(input_string, cmd[i].cmd) != NULL) cmd[i].action(input_string);   
  }
}

void ACT_setModFreq(char *string)
{
   String str = string;

   //value 起始位址 = command 起始位址 + 空格處index + 1
   g_freq = atoi(string + str.indexOf(' ') + 1);
   I2CWriteData(I2C_MOD_FREQ);
   Serial.println(g_freq);
}

void ACT_setModPhase(char *string)
{
   String str = string;

   g_phase = atoi(string + str.indexOf(' ') + 1);
   I2CWriteData(I2C_MOD_PHASE);
   Serial.println(g_phase);
}

void ACT_readAdc(char *string)
{
   unsigned int value;

   value = adc.Read();
   Serial.println(value);
}

void ACT_setTime(char *string)
{
   String str = string;
   char *time_str = string + str.indexOf(' ') + 1;
   char *value_str = string + str.indexOf(' ') + 3;
   int index = atoi(time_str);
   int value = atoi(value_str);

#if TEST_MODE
   Serial.println(time_str);
   Serial.println(value_str);
#endif

   if ( (index >= 0) && (index <= 2) )
   {
     g_time[index] = value;
#if TEST_MODE
     Serial.print("set Time");
     Serial.print(index);
     Serial.print(" = ");
     Serial.println(value);
#endif
     Serial.println(g_time[index]);
   }
#if TEST_MODE
   else
   {
      Serial.println("out of range");
      ;
   }
#endif
}

void ACT_setLoop(char *string)
{
   String str = string;
   g_loop = atoi(string + str.indexOf(' ') + 1);
   Serial.println(g_loop);

}

void ACT_setTemp(char *string)
{
   String str = string;
   char *temp_str = string + str.indexOf(' ') + 1;
   char *value_str = string + str.indexOf(' ') + 3;
   int index = atoi(temp_str);
   int value = atoi(value_str);

#if TEST_MODE
   Serial.println(temp_str);
   Serial.println(value_str);
#endif

   if ( (index >= 0) && (index <= 2) )
   {
     g_temp[index] = value;
#if TEST_MODE
     Serial.print("set Temp");
     Serial.print(index);
     Serial.print(" = ");
     Serial.println(value);
#endif
     Serial.println(g_temp[index]);
   }
#if TEST_MODE
   else
   {
      Serial.println("out of range");
      ;
   }
#endif
}

void I2CWriteData(unsigned char com)
{
  unsigned char temp[2];
  switch (com)
  {
    case I2C_MOD_FREQ:
      temp[0] = g_freq >> 8;
      temp[1] = g_freq;
      break;

    case I2C_MOD_PHASE:
      temp[0] = g_phase >> 8;
      temp[1] = g_phase;
      break;
  }
  Wire.beginTransmission(SLAVE_MCU_I2C_ADDR);//
  Wire.write(com);//
  Wire.write(temp, 2);//
  Wire.endTransmission();//
  delayMicroseconds(I2CSENDDELAY);//
}

void DTC03_I2CWriteData(unsigned char addr, unsigned char com)
{
  unsigned char temp[2];
  switch(com)
  {
    case I2C_COM_INIT:
        temp[0] = g_bconst;
        temp[1] = g_bconst >> 8;
        break;

    case I2C_COM_CTR:
        temp[0] = g_currentlim;
        temp[1] = g_p;
        break;

    case I2C_COM_VSET:
        temp[0] = g_vset;
        temp[1] = g_vset>>8;
        break;

    case I2C_COM_R1R2:
        temp[0] = g_r1;
        temp[1] = g_r2;
        break;

    case I2C_COM_TPIDOFF:
        temp[0] = g_tpidoff;
        temp[1] = 0;
        break;

    case I2C_COM_FBC:
        temp[0] = g_fbcbase;
        temp[1] = g_fbcbase>>8;
        break;

    case I2C_COM_VMOD:
        temp[0] = g_vmodoffset;
        temp[1] = g_vmodoffset >>8;
        break;
    
    case I2C_COM_KI:
        temp[0] = pgm_read_word_near(kilstable230+g_kiindex*2);
        temp[1] = pgm_read_word_near(kilstable230+g_kiindex*2+1);
        break;
    
    case I2C_COM_RMEAS:
        temp[0] = g_Rmeas;
        temp[1] = g_Rmeas>>8;
        break;
      
    case I2C_COM_OTP:
        temp[0] = g_otp;
        temp[1] = g_otp>>8;
      break;
      
    case I2C_COM_WAKEUP:
        temp[0] = 1;
        temp[1] = 0; // overshoot cancelation, set 0 in DTC03
        break;
      
    case I2C_COM_ATSTABLE:
        temp[0] = g_stableCode_atune;
        break;  
      
  }
  Wire.beginTransmission(addr);//
  Wire.write(com);//
  Wire.write(temp, 2);//
  Wire.endTransmission();//
  delayMicroseconds(I2CSENDDELAY);//
}

void DTC03_I2CReadData(unsigned char addr, unsigned char com)
{
  unsigned char temp[2], b_upper, b_lower;
  unsigned int itectemp;
  bool itecsign;

  Wire.beginTransmission(addr);
  Wire.write(com);
  Wire.endTransmission();
  delayMicroseconds(I2CREADDELAY);
  Wire.requestFrom(DTC03P05,2);
  while(Wire.available()==2)
  {
    temp[0] = Wire.read();
    temp[1] = Wire.read();
  }
  switch(com)
  {
    case I2C_COM_VACT:
        g_vact =(temp[1] <<8) | temp[0];
        break;
  }
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
//    Serial.print("event: ");
//    Serial.println(inputString);
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
