#include <Led4X7_Disp.h> ;
#include <SawtoothGen.h>
#define AI_PIN A2

led4X7_disp led(2,3,4,5,8,9,10,11,13); // (A,B,C,D,0,1,2,3,h), A~D: BCD pins, 0~3: position pins, h : point
SawtoothGen fn_gen(1, 6);// comparator AI pin, charge control pin
void setup() {
  led.init(2, 10, 31.1); // AI pin; adc bits, option : 10, 12, 16; gain 
}

void loop() {
  fn_gen.Sawtooth_out();
  led.print();
}

