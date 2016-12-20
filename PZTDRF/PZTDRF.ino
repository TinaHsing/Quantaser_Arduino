#include <Led4X7_Disp.h> ;
#include <SawtoothGen.h>
#define AI_PIN A2
//
#define REFSENSE A0
#define OUTPUTSENSE A1
#define CHARGECONTROL 6
#define MINOUTVOLTAGE 40 //40/1024*5000=195mV
#define MAXOUTVOLTAGE 984 //984/1024*5000=4805mV
unsigned int vref, vout;
bool controlflag;
//
led4X7_disp led(2,3,4,5,8,9,10,11,12); // (A,B,C,D,0,1,2,3,h), A~D: BCD pins, 0~3: position pins, h : point
//SawtoothGen fn_gen(0,1,6,0.2,4.8);//(refsense pin, output sense pin, charge control pin, Vmin, Vmax) 
void setup() {
  //
  SetPinMode();
  controlflag = 1;
  //
  led.init(10, 30); // adc bits, option : 10, 12, 16; gain 
}

void loop() {
  int AI_read;
  //
  vout = analogRead(OUTPUTSENSE);
  if(controlflag)
  {
   vref=analogRead(REFSENSE);
   if(vout>=vref || vout>=MAXOUTVOLTAGE)
   {
    digitalWrite(CHARGECONTROL,HIGH);
    controlflag=0;
   }
  }
  else
  {
   if(vout<MINOUTVOLTAGE)
   {
    digitalWrite(CHARGECONTROL,LOW);
    controlflag=1;
   }
  }
  
//  fn_gen.Sawtooth_out();
  AI_read = analogRead(AI_PIN);
  led.print(AI_read);
}
void SetPinMode()
{
  pinMode(REFSENSE, INPUT);
  pinMode(OUTPUTSENSE, INPUT);
  pinMode(CHARGECONTROL, OUTPUT);
}
