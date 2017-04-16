#include <Led4X7_Disp.h>
#include <SawtoothGen.h>
#include <PZTDRF_V1.h>
#include <Wire.h>
#include <FiveInOne_MS.h>

led4X7_disp led(2,3,4,5,8,9,10,11,12); // (A,B,C,D,0,1,2,3,h), A~D: BCD pins, 0~3: position pins, h : point
SawtoothGen fn_gen(A1,6);// comparator AI pin, charge control pin
PZTDRF pzt(A2,7,A3); // +15V check pin, dcbias EN pin, drv2700 EN pin

void setup() {
  Wire.begin(PZTDRFADD);
  Wire.onReceive(ReceiveEvent);
  Wire.onRequest(RequestEvent);
  pzt.init();
  pzt.PowerOn(10.0, 100);//Vth, delay time
  led.init(A0, 10, 31.1); // AI pin; adc bits, option : 10, 12, 16; gain 
}

void loop() {
  fn_gen.Sawtooth_out();
  pzt.PowerOff(14.0, 10);//Vth, delay time
  led.print();
}
void ReceiveEvent(int howmany)
{
  led.OnReceiveEvent();
}
void RequestEvent()
{
  led.OnRequestEvent();
}
