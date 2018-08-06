#include <C12880.h>

C12880 spectro;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  spectro.SpectroInit();
#if 0   //sample code
  spectro.PulseClkAB(3);
  spectro.StartIntegAB();
  spectro.PulseClkAB(10000);
  spectro.StopIntegA();
  spectro.StopIntegB();
  spectro.PulseClkAB(87);
  spectro.ReadVedioAB(data);
#else
  spectro.RunDevice(10000,10000);
#endif
  //=======
  delay(10);
  spectro.RunDevice(20000,30000);
#if !DEBUG_MODE
  spectro.PrintData();
#endif
}

void loop() {
  // put your main code here, to run repeatedly:

}


