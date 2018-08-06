#include <C12880.h>

C12880 spectro;
//int data[CHANNEL_NUMBER];
//int data[CHANNEL_NUMBER*2];
byte data[CHANNEL_NUMBER*4];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  spectro.SpectroInit();
  spectro.PulseClkAB(3);
  spectro.StartIntegAB();
  spectro.PulseClkAB(10000);
  spectro.StopIntegA();
  spectro.StopIntegB();
  spectro.PulseClkAB(87);
  Serial.println("ReadVedioAB 1");
  spectro.ReadVedioAB(data);
  //=======
  delay(10);
  RunAll();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void RunAll(){
  int i;

  spectro.PulseClkAB(3);
  spectro.StartIntegAB();
  spectro.PulseClkAB(60000);
  spectro.StopIntegA();
  spectro.StopIntegB();
  spectro.PulseClkAB(87);
  Serial.println("ReadVedioAB 2");
  spectro.ReadVedioAB(data);

#if 0  //only read 1
  for (i=0; i< CHANNEL_NUMBER; i++)
  {
    Serial.println(data[i]);
  }
#endif

#if 0  //2 for int
  for (i=0; i< CHANNEL_NUMBER*2; i++)
  {
    Serial.print(data[i]);
    Serial.print(',');
    Serial.println(data[++i]);
  }
#endif

#if 1  //2 for byte
  for (i=0; i< CHANNEL_NUMBER*4; i++)
  {
    unsigned char n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    n1 = data[i];
    n2 = data[++i];
    n3 = data[++i];
    n4 = data[++i];
    Serial.print(n1|(n2<<8));
    Serial.print(',');
    Serial.println(n3|(n4<<8));
  }
#endif
}
  
