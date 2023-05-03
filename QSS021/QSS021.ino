
#include <Si5341_Config.h>

int Status;
char key;
Si5341 Device;

void setup() {
  // put your setup code here, to run once:
//  char Data[] = {0xAA, 0x55};
//  TwoWire A(70, 71);
  Wire1.begin();
  Serial.begin(115200);
  Serial.println("press any key to start");
  while(1)
  {
    if (Serial.available()>0) {
      key = Serial.read();
      if (key == 'Q') {
        Serial.println("exit");
        break;
      }
      Serial.println("Start");
//      Wire1.beginTransmission(0x55);
//      Wire1.write("00");
//      Wire1.endTransmission();
      Status = Device.SetFreq();
      if(Status == XST_SUCCESS) {
        Serial.println("SetFreq success");
      } else {
        Serial.println("SetFreq failed");
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
