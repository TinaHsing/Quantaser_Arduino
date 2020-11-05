#include <LTC2615.h>
#include <LTC2451.h>
#include <Wire.h>
#include "QSS015_cmn.h"

LTC2615 ltc2615;
LTC2451 ltc2451;

#define DEBUG 1

unsigned long ul_time_begin = 0, ul_time_current = 0;
unsigned long ul_ReadCounter = 0;
bool g_lock = true;
volatile unsigned long ul_Counter = 0;
unsigned long g_int_time = 100000;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PD2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PD2), AddCounter, RISING);
  // attachInterrupt(digitalPinToInterrupt(PD3), InterLock, LOW);
  inputString.reserve(20);
  ltc2615.init();
  ltc2451.Init(0);
  ul_time_begin = millis();
  Wire.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  

  ul_time_current = millis();
  if ( (ul_time_current - ul_time_begin) > 1000 )
  {
    ul_ReadCounter = ul_Counter;
    ul_Counter = 0;
    ul_time_begin = millis();
    //    Serial.print("inside:");
    //Serial.println(ul_ReadCounter);
  }

  if (stringComplete)
  {
    char *c_inputString = (char*)inputString.c_str();
    char *set_vol_str  = strstr(c_inputString, "SetVoltage ");  //11
    char *read_cnt_str = strstr(c_inputString, "ReadCounter");  //11
    char *read_vol_str = strstr(c_inputString, "ReadVoltage");  //11
    char *set_int_time = strstr(c_inputString, "SetIntTime ");  //11
    char *check_lock_str = strstr(c_inputString, "CheckInterLock");  //14

    if (set_vol_str != NULL)
    {
      char *ch_str = c_inputString + 11;
      unsigned char ch = atoi(ch_str);
      //Serial.println(ch);
      char *vol_str = ch_str + 2;
      unsigned int vol = atoi(vol_str);
      SetVoltage(ch, vol);
    }

    if (read_cnt_str != NULL)
    {
#if DEBUG
      Serial.print("ReadCounter = ");
#endif
      Serial.println(ul_ReadCounter);
    }

    if (read_vol_str != NULL)
    {
      ReadVoltage();
    }

    if (set_int_time != NULL)
    {
      char *int_str = c_inputString + 11;
      g_int_time = atol(int_str);
      //I2CReadData(g_int_time);
#if DEBUG
      Serial.print("SetIntTime in loop() = ");
      Serial.println(g_int_time);
#endif
    }

    if (check_lock_str != NULL)
    {
#if DEBUG
      Serial.print("CheckInterLock = ");
#endif
      Serial.println(g_lock);
    }

    // clear the string:
    inputString = "";
    stringComplete = false;
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
    if (inChar == '\n') {
      stringComplete = true;
    }
#if DEBUG
    //Serial.print(inChar);
#endif
  }
}

void SetVoltage(unsigned char ch, unsigned int vol)
{
  char tempStr[30];

  if (ch == 1)
  {
    ltc2615.writeint(CH_A, vol);
#if DEBUG
    sprintf(tempStr, "ch = %d, value = %u", ch, vol);
    Serial.println(tempStr);
#endif
  }
  else if (ch == 2)
  {
    ltc2615.writeint(CH_B, vol);
#if DEBUG
    sprintf(tempStr, "ch = %d, value = %u", ch, vol);
    Serial.println(tempStr);
#endif
  }
  else if (ch == 3)
  {
    ltc2615.writeint(CH_C, vol);
#if DEBUG
    sprintf(tempStr, "ch = %d, value = %u", ch, vol);
    Serial.println(tempStr);
#endif
  }
  else if (ch == 4)
  {
    ltc2615.writeint(CH_D, vol);
#if DEBUG
    sprintf(tempStr, "ch = %d, value = %u", ch, vol);
    Serial.println(tempStr);
#endif
  }
  else if (ch == 5)
  {
    ltc2615.writeint(CH_E, vol);
#if DEBUG
    sprintf(tempStr, "ch = %d, value = %u", ch, vol);
    Serial.println(tempStr);
#endif
  }
}

void InterLock()
{
  int i;
  for (i = 1 ; i < 6; i ++)
    SetVoltage(i, 0);
  g_lock = false;
}


void AddCounter()
{
  ul_Counter++;
}


void ReadVoltage()
{
  unsigned int ui_ReadVoltage = 0;

  SetTime();
  delayMicroseconds(g_int_time+50);

  ui_ReadVoltage = ltc2451.Read();

#if DEBUG
  Serial.print("ReadVoltage = ");
#endif
  Serial.println(ui_ReadVoltage);
}


void SetTime()
{
  unsigned char temp[4];
  

  temp[0] = g_int_time >> 24;
  temp[1] = g_int_time >> 16;
  temp[2] = g_int_time >> 8;
  temp[3] = g_int_time;

#if DEBUG
  Serial.print("SetIntTime in SetTime() = ");
  Serial.println(g_int_time);
#endif

  Wire.beginTransmission(SLAVE_MCU_I2C_ADDR);//
  Wire.write(temp, 4);//
  Wire.endTransmission();//
  delayMicroseconds(I2CSENDDELAY);//

}
