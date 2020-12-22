// for ATMEGA328P 3.3V clock = 10MHz //

#include <SoftSPI.h>


String inputString = ""; // a String to hold incoming data

boolean stringComplete = false;  // whether the string is complete
// sherry++ 2020.10.14
boolean readSPI_flag = false;
unsigned int g_reg, g_data;
unsigned long g_test = 0;
#define READ_DATA_TIME 12500 // for clock = 10MHz

#define SPICHIPSEL 10 // CHIPSELECT PIN FOR SPI
#define MISO 12
#define MOSI 11
#define SCK 13
#define TESTMODE 1

SoftSPI mySPI(MOSI, MISO, SCK);

/*******number of uart command number **********/
#define COMMAND_NUM 3
#define SPICMD_GETADC 0x54
#define GETADC_DATALEN 200

/******ADC channel**********/
#define TEMPCH1 A5
#define TEMPCH2 A4
#define TEMPCH3 A3
#define TEMPCH4 A2


/********glogal variable***************/

typedef struct table {
  char *cmd;
  void (*action)(char *);
} table_t;
table_t cmd_list[COMMAND_NUM];

//long cnt=0, t1, t2;

void setup() {

/***** register command and act function here ******/
  cmd_list[0].cmd = "setSPI";
  cmd_list[0].action = setSPI;
  cmd_list[1].cmd ="readSPI";
  cmd_list[1].action = readSPI;
  cmd_list[2].cmd ="readTemp";
  cmd_list[2].action = readTemp;
/****************************************************/
  pinMode(SPICHIPSEL, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
  digitalWrite(SPICHIPSEL,HIGH);
//  Serial.begin(184320);
  
  #ifdef TESTMODE
    Serial.begin(115200);
    //Serial.print("test");
  #else
    Serial.begin(92160);  // real clock rate = 92160 / 0.8 = 115,200 Hz
  #endif
  mySPI.setClockDivider(CLOCK_DIV256);
  mySPI.setBitOrder(MSBFIRST);
  mySPI.setDataMode(MODE0);
  mySPI.begin();

  g_test = 0;

  inputString.reserve(40);
//  t1 = millis();
}


void loop() {

  if (stringComplete)
  {
    char *c_inputString = (char*)inputString.c_str();
    match_cmd(c_inputString, cmd_list);    
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


void match_cmd(char *input_string, table_t cmd[])
{

  for(int i=0; i<COMMAND_NUM; i++)
  {
    if (strstr(input_string, cmd[i].cmd) != NULL) cmd[i].action(input_string);   
  }
}


void setSPI(char *string)
{
  char sperator = ' ';
  char cmd[20];
  unsigned int reg, data;
  byte address;
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

  low = data;
  high = data >>8;
  temp1 = mySPI.transfer(high);
  temp2 = mySPI.transfer(low);
  out = (temp1 << 8)|temp2|out;

  digitalWrite(SPICHIPSEL,HIGH);
  return out;
}
 
void readSPI(char *string)
{
  char sperator = ' ';
  char cmd[20];
  unsigned int reg, data;
  byte address;
  long var;
  byte high, low;
 
  sscanf(string, "%s %x %ld", cmd, &address, &var);
  reg = (int)((address<<8) | ((var>>16) & 0x00ff));
  data = var;

  // sherry++ 2020.10.14
  if (var == 1)
    readSPI_flag = true;
  else
    readSPI_flag = false;
  g_reg = reg;
  g_data = data;
  g_test = 0;

}

void readSPI_data()
{
  long out; 
  unsigned long t_begin, t_end, t_diff;

      t_begin = micros();
      sendSPI(g_reg, g_data);
      delayMicroseconds(5000);
      #ifdef TESTMODE
        if (g_test % 1000 == 0)
          out = 284562111;
         else
          out = g_test;
      #else
        out = sendSPI(0xffff, 0xffff);
      #endif
      Serial.write(out>>24);
      Serial.write(out>>16);
      Serial.write(out>>8);
      Serial.write(out);
      t_end = micros();
      t_diff = t_end - t_begin;
      delayMicroseconds(READ_DATA_TIME - t_diff);

}

void readTemp(char *string)
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

  high = temperature >> 13;
  low = temperature>>5;
  Serial.write(high);
  Serial.write(low);

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
