#include <SoftSPI.h>


String inputString = ""; // a String to hold incoming data

boolean stringComplete = false;  // whether the string is complete

#define SPICHIPSEL 9 // CHIPSELECT PIN FOR SPI
#define MISO 12
#define MOSI 11
#define SCK 13

SoftSPI mySPI(MOSI, MISO, SCK);

/*******number of uart command number **********/
#define COMMAND_NUM 2
#define SPICMD_GETADC 0x54
#define GETADC_DATALEN 200

#define TESTMODE false
/********glogal variable***************/

typedef struct table {
  char *cmd;
  void (*action)(char *);
} table_t;
table_t cmd_list[COMMAND_NUM];

void setup() {

/***** register command and act function here ******/
  cmd_list[0].cmd = "setSPI";
  cmd_list[0].action = setSPI;
  cmd_list[1].cmd ="readSPI";
  cmd_list[1].action = readSPI;

/****************************************************/
  pinMode(SPICHIPSEL, OUTPUT);
  digitalWrite(SPICHIPSEL,HIGH);
  Serial.begin(115200);
  mySPI.setClockDivider(CLOCK_DIV256);
  mySPI.setBitOrder(MSBFIRST);
  mySPI.setDataMode(MODE0);
  mySPI.begin();


  //SPI.setClockDivider(SPI_CLOCK_DIV128);
  //SPI.setDataMode(SPI_MODE0);
  //SPI.setBitOrder(MSBFIRST);
  //SPI.begin();

  inputString.reserve(40);
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
  //String str = string;
  char sperator = ' ';
  char cmd[20];
  int reg, data;
  
  Serial.println("setSPI");
  sscanf(string, "%s %x %x", cmd, &reg, &data);
  sendSPI(reg, data);

}

unsigned long sendSPI( unsigned int reg, unsigned int data)
{
  byte high, low;
  unsigned long out, temp1, temp2;
  digitalWrite(SPICHIPSEL,LOW);
  Serial.print("sendSPI");
  Serial.println(reg, DEC);
  Serial.println(data, DEC);
  low = reg;
  high = reg >>8;
  temp1 =mySPI.transfer(high);
  temp2 =mySPI.transfer(low);
  out = (temp1 << 24)|(temp2 << 16);
  Serial.print("first two bytes:");
  Serial.pirntln(HEX,out); 
  low = data;
  high = data >>8;
  temp1 = mySPI.transfer(high);
  temp2 = mySPI.transfer(low);
  out = (temp1 << 8)|temp2|out;
  Serial.print("total:");
  Serial.pirntln(HEX,out); 
  digitalWrite(SPICHIPSEL,HIGH);
  return out;
  
}
 
void readSPI(char *string)
{
  //String str = string;
  char sperator = ' ';
  char cmd[20];
  int reg, data;
  byte high, low;
  unsigned long out;
  Serial.println("readSPI");
  sscanf(string, "%s %x %x", cmd, &reg, &data);
  sendSPI(reg, data);
  out = sendSPI(54, 34);
  Serial.println(HEX,out);
  //Serial.println(HEX,out>>24);
  //Serial.println(HEX,(out<<8)>>16);
  //Serial.println(HEX,(out<<16)>>8);
  // Serial.println(HEX,(out<<24)>>24);
  
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
