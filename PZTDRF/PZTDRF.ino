#include <Led4X7_Disp.h> ;
#define AI_PIN A0
led4X7_disp led(4,5,6,7,8,9,10,11,12); // (A,B,C,D,0,1,2,3,h), A~D: BCD pins, 0~3: position pins, h : point
void setup() {
  led.init(10, 30); // adc bits, option : 10, 12, 16; gain 
}

void loop() {
  int AI_read;
  AI_read = analogRead(AI_PIN);
  led.print(AI_read);
}
