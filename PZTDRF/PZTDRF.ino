#include <Led4X7_Disp.h> ;
#include <SawtoothGen.h>
#define AI_PIN A2
led4X7_disp led(2,3,4,5,8,9,10,11,12); // (A,B,C,D,0,1,2,3,h), A~D: BCD pins, 0~3: position pins, h : point
SawtoothGen fn_gen(0,1,6,0.005,4.995);//(refsense pin, output sense pin, charge control pin, Vmax, Vmin) 
void setup() {
  led.init(10, 30); // adc bits, option : 10, 12, 16; gain 
}

void loop() {
  int AI_read;
  fn_gen.Sawtooth_out();
  AI_read = analogRead(AI_PIN);
  led.print(AI_read);
}
