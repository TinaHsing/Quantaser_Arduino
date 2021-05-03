// for ATMEGA328P 3.3V clock = 10MHz //

// compatible PCB QSS011_PWR_P02
// P02 reversion reason:
// 1. change update freq due to xtal using 10MHz
// 2. Add temperature sensing function
// 3. update temperature in read_data() with ch paramter 
// for every continue data req, user must add temperature channel as variable input


#include <SoftSPI.h>


String inputString = ""; // a String to hold incoming data

boolean stringComplete = false;  // whether the string is complete
// sherry++ 2020.10.14
boolean readSPI_flag = false;
unsigned int g_reg, g_data;

unsigned long g_test = 0;
// tina ++ 2020.12.28
unsigned char g_ch;
#define READ_DATA_TIME 12500 // for clock = 10MHz

#define SPICHIPSEL 10 // CHIPSELECT PIN FOR SPI
#define MISO 12
#define MOSI 11
#define SCK 13

// if use single arduino to test QSS011 , MUST set TESTMODE 1
// if use QSS011 PCB , set TESTMODE 0
#define TESTMODE 0

#define VERSION "QSS011_GyroA_Main_P02"

SoftSPI mySPI(MOSI, MISO, SCK);

/******ADC channel**********/
#define TEMPCH1 A5
#define TEMPCH2 A4
#define TEMPCH3 A3
#define TEMPCH4 A2

//long cnt=0, t1, t2;

void setup() {


  pinMode(SPICHIPSEL, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
  digitalWrite(SPICHIPSEL,HIGH);
//  Serial.begin(184320);

  #if TESTMODE
    Serial.begin(115200);
  #else
    Serial.begin(92160);  // real clock rate = 92160 / 0.8 = 115,200 Hz
  #endif
  mySPI.setClockDivider(CLOCK_DIV256);
  mySPI.setBitOrder(MSBFIRST);
  mySPI.setDataMode(MODE0);
  mySPI.begin();
  analogReference(EXTERNAL);
  g_test = 0;

  inputString.reserve(40);
//  t1 = millis();
}


void loop() {

  if (stringComplete)
  {
    char *c_inputString = (char*)inputString.c_str();
    match_cmd(c_inputString);    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  // sherry++ 2020.10.14
  if (readSPI_flag)
  {
    readSPI_data();
    g_test++;
  }
}


void match_cmd(char *input_string)
{
 
    if (strstr(input_string, "setSPI") != NULL) 
      setSPI(input_string);
    else if (strstr(input_string, "readSPI") != NULL)
      readSPI(input_string);
    else if (strstr(input_string, "readTemp") != NULL)
      readTemp(input_string);
    else if (strstr(input_string, "getVersion") != NULL)
      getVersion();
}
void getVersion()
{
  Serial.println(VERSION);
}


void setSPI(char *string)
{
  char cmd[20];
  unsigned int reg, data;
  unsigned int address;
  long var;
  
  sscanf(string, "%s %x %ld", cmd, &address, &var);
  reg = (int)((address<<8) | ((var>>16) & 0x00ff));
  data = var;

  sendSPI(reg, data);
}

unsigned long sendSPI( unsigned int reg, unsigned int data)
{
  byte high, low;
  unsigned long temp1, temp2;
  long out;
  digitalWrite(SPICHIPSEL,LOW);
  low = reg;
  high = reg >>8;
  temp1 = mySPI.transfer(high);
  temp2 = mySPI.transfer(low);
  out = (temp1 << 24)|(temp2 << 16);
  Serial.println(high,HEX);
  Serial.println(low, HEX);

  low = data;
  high = data >>8;
  temp1 = mySPI.transfer(high);
  temp2 = mySPI.transfer(low);
  Serial.println(high, HEX);
  Serial.println(low, HEX);
  out = (temp1 << 8)|temp2|out;

  digitalWrite(SPICHIPSEL,HIGH);
  return out;
}
 
void readSPI(char *string)
{
  char sperator = ' ';
  char cmd[20];
  unsigned int reg, data;
  unsigned int address;
  long var;
  byte high, low;
 
  sscanf(string, "%s %x %ld", cmd, &address, &var);
  reg = (int)((address<<8) | ((var>>16) & 0x00ff));
  data = var;

  // sherry++ 2020.10.14
  if (var == 0)
    readSPI_flag = false;
  else
    readSPI_flag = true;
  g_reg = reg;
  g_data = data;
  g_ch =var;
  g_test = 0;

}

void readSPI_data()
{
  unsigned long temperature;
  long out; 
  long t_begin, t_end, t_diff;

      t_begin = micros();
      sendSPI(g_reg, g_data);
     
      switch(g_ch)
      {
        case 1:
          temperature = readTemp("temp 1 1");
        break;
        case 2:
          temperature = readTemp("temp 2 1");
        break;
        case 3:
          temperature = readTemp("temp 3 1");
        break;
        case 4:
          temperature = readTemp("temp 4 1");
        break;
        default:
          temperature = readTemp("temp 1 1");
        break;

      }     
      #if TESTMODE
          out = g_test;
      #else
        out = sendSPI(0xffff, 0xffff);
      #endif
      Serial.write(out>>24);
      Serial.write(out>>16);
      Serial.write(out>>8);
      Serial.write(out);
      Serial.write(temperature >> 13);
      Serial.write(temperature >> 5);
      t_end = micros();
      t_diff = t_end - t_begin;
      if (t_diff < READ_DATA_TIME)
        delayMicroseconds(READ_DATA_TIME - t_diff);

}


unsigned long readTemp(char *string)
{
  char sperator = ' ';
  byte high, low;
  char cmd[20];
  unsigned int ch;
  unsigned long temperature = 0;
  sscanf(string, "%s %d %ld", cmd, &ch);
  for(int i = 0; i< 32; i++)
  {
    
    switch (ch)
    {
      case 1:
        temperature  = temperature + analogRead(TEMPCH1);
      break;
      case 2:
        temperature  = temperature + analogRead(TEMPCH2);
      break;
      case 3:
        temperature  = temperature + analogRead(TEMPCH3);
      break;
      case 4:
        temperature  = temperature + analogRead(TEMPCH4);
      break;
      default:
        temperature  = temperature + analogRead(TEMPCH1);
      break;
    }
  }
 
  if (!readSPI_flag)
   {
    high = temperature >> 13;
    low = temperature>>5;
    Serial.write(high);
    Serial.write(low);
    }
  
  return temperature;
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
  }
}
