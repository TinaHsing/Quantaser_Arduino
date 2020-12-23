#include <LTC2451.h>
#include <Wire.h>
#define COMMAND_NUM 2



typedef struct table {
  char *cmd;
  void (*action)(char *);
} table_t;

String inputString = "";
bool stringComplete = false;
table_t cmd_list[COMMAND_NUM];
LTC2451 adc;
unsigned int counter = 0;
void setup() {

  // put your setup code here, to run once:
  setISR();
  Serial.begin(115200);
  adc.Init(0);
  //adc.Init(MODE60Hz);
  Wire.begin();


}


void loop() {
  // put your main code here, to run repeatedly:
  if (stringComplete)
  { 
     char *c_inputString = (char *)inputString.c_str();
     if(strstr(c_inputString, "READ_ADC") !=NULL)
      ACT_readAdc(c_inputString);
    else if(strstr(c_inputString, "MOD_FREQ") !=NULL)
      ACT_setModFreq(c_inputString);
    else if(strstr(c_inputString, "MOD_PHASE") !=NULL)
      ACT_setModPhase(c_inputString);
     inputString ="";
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
void setISR()
{
  cli(); // disable all interrupt
  TCCR1A =0; // reset the timer1 register
  TCCR1B =0; // reset the timer1 register
  //TCCR1B |= (1 << CS12)|(1<< CS10); // CS12 & CS10 = 1 will set prescaler = 1024 
  TCCR1B |= (1 << CS12); // set prescaler = 256 
  TCCR1B |= (1<< WGM12); //set to TCT mode counter will reset when reach OCR1A
  TCNT1 = 0; // set the counter to 0
  TIMSK1 = (1<< OCIE1A)|(1<< OCIE1B); // enable OCIE1A and OCIE1B interrupt
  OCR1A = 7812;
  OCR1B = 3500;
  sei(); // enable all interterrupt
}


ISR(TIMER1_COMPA_vect)
{
  digitalWrite(11, !digitalRead(11));
}

ISR(TIMER1_COMPB_vect)
{
   digitalWrite(12, !digitalRead(12));
}





void ACT_readAdc(char *string)
{
   unsigned int value;
   counter++;
   value = adc.Read();
   Serial.print(counter);
   Serial.print(":");
   Serial.println(value);
}

void ACT_setModFreq( char *string)
{
  String str = string;
  char *time_str = string + str.indexOf(' ') + 1;
  unsigned int value = atoi(time_str);
  
  if (value)
  {
    cli();
    TCNT1 = 0;
    OCR1A = value;
    sei();
  }
}
void ACT_setModPhase(char *string)
{
  String str = string;
  char *time_str = string + str.indexOf(' ') + 1;;
  unsigned int value = atoi(time_str);

  if (value)
  {
    cli();
    TCNT1 = 0;
    OCR1B = value;
    sei();
  }
}
