#include <C12880.h>

C12880 spectro;
int data[CHANNEL_NUMBER];

void setup() {
  // put your setup code here, to run once:
  int i;
  Serial.begin(9600);
  spectro.SpectroInit();
  spectro.PulseClkA(3);
  spectro.StartIntegA();
  spectro.PulseClkA(60000);
  spectro.StopIntegA();
  spectro.PulseClkA(87);
  spectro.ReadVedioA(data);
  //=======
  delay (10);
  spectro.SpectroInit();
  spectro.PulseClkA(3);
  spectro.StartIntegA();
  spectro.PulseClkA(60000);
  spectro.StopIntegA();
  spectro.PulseClkA(87);
  spectro.ReadVedioA(data);

  for (i=0; i< CHANNEL_NUMBER; i++)
  {
    Serial.println(data[i]);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}


  
