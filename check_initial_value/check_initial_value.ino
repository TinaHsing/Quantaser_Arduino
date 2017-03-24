#include <DTC03_INITCHECK.h>
DTC03 dtc;
void setup() {
  // put your setup code here, to run once:
  dtc.SetSPI();
  dtc.SetPinMode();
  dtc.ParamInit();
  dtc.InitVactArray();
  dtc.CheckInitValue(0,0,0);
  
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     dtc.ReadEEPROM();

}

void loop() {
  // put your main code here, to run repeatedly:

}
