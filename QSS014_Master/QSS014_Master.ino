#include <Wire.h>
#include "QSS014_cmn.h"
#include <LTC2451.h>

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/********number of uart command number **********/
#define COMMAND_NUM 6
#define I2CSENDDELAY 100 //delay100us
#define TEST_MODE false

/********glogal variable***************/
int g_freq, g_phase;
int g_time[3], g_loop, g_temp[3];

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
