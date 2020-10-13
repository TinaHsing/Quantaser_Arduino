#include <SoftSPI.h>


String inputString = ""; // a String to hold incoming data

boolean stringComplete = false;  // whether the string is complete

#define SPICHIPSEL 10 // CHIPSELECT PIN FOR SPI
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

//long cnt=0, t1, t2;
void setup() {

/***** register command and act function here ******/
  cmd_list[0].cmd = "setSPI";
  cmd_list[0].action = setSPI;
  cmd_list[1].cmd ="readSPI";
  cmd_list[1].action = readSPI;

/****************************************************/
  pinMode(SPICHIPSEL, OUTPUT);
  digitalWrite(SPICHIPSEL,HIGH);
//  Serial.begin(184320);
  Serial.begin(115200);
  mySPI.setClockDivider(CLOCK_DIV256);
  mySPI.setBitOrder(MSBFIRST);
  mySPI.setDataMode(MODE0);
  mySPI.begin();



  inputString.reserve(40);
//  t1 = millis();
}


void loop() {
  /***test ***/
//  char *c_inputString = "readSPI 0";
//  stringComplete = true;
//  delay(100); 
/***test ***/ 
  if (stringComplete)
  {
    
    char *c_inputString = (char*)inputString.c_str();
  /***test ***/
//    Serial.print(c_inputString); 
    /***test ***/
    match_cmd(c_inputString, cmd_list);    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  /***test ***/
//  if(cnt==100) {
//    cnt=0;
//    t2 = millis();
//    Serial.print("dt: ");
//    Serial.println(t2-t1);
//    t1 = t2;
//  }
  /***test ***/
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
  
//  Serial.println("setSPI");
  sscanf(string, "%s %x %ld", cmd, &address, &var);
  reg = (int)((address<<8) | ((var>>16) & 0x00ff));
  data = var;
//  Serial.println(address, HEX);
//  Serial.println(var, HEX);
//  Serial.println(reg, HEX);
//  Serial.println(data, HEX);

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
  temp1 =mySPI.transfer(high);
  temp2 =mySPI.transfer(low);
  out = (temp1 << 24)|(temp2 << 16);

  low = data;
  high = data >>8;
  temp1 = mySPI.transfer(high);
  temp2 = mySPI.transfer(low);
  out = (temp1 << 8)|temp2|out;
//  cnt++;
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
  long out;
//  Serial.println("readSPI");
  sscanf(string, "%s %x %ld", cmd, &address, &var);
  reg = (int)((address<<8) | ((var>>16) & 0x00ff));
  data = var;
  if(var) 
  {
      while(1){
      sendSPI(reg, data);
      delay(5);
      out = sendSPI(0xffff, 0xffff);
  //  Serial.println(out);
      Serial.write(out>>24);
      Serial.write(out>>16);
      Serial.write(out>>8);
      Serial.write(out);
    }
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
  }
}
