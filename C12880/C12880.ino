#include <C12880.h>

//#define DEBUG_MODE  1

C12880 spectro;
//int data[CHANNEL_NUMBER];
//int data[CHANNEL_NUMBER*2];
byte data[CHANNEL_NUMBER*4];

uint32_t I_timeOnlyA = 100;
uint32_t I_timeOnlyB = 150;
uint32_t I_timeBothAB = 0;
bool bFlagAB = true;
uint32_t P_timeBothAB = 0;

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
  spectro.ReadVedioAB(data);
  //=======
  delay(10);
  RunAll();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void RunAll()
{
  int i;

  spectro.PulseClkAB(3);

#ifdef DEBUG_MODE
  Serial.print("A time = ");
  Serial.println(I_timeOnlyA);
  Serial.print("B time = ");
  Serial.println(I_timeOnlyB);
#endif

  if (I_timeOnlyB > I_timeOnlyA)
  {
    I_timeBothAB = I_timeOnlyA;
    I_timeOnlyB -= I_timeOnlyA;
    bFlagAB = false;
  }
  else // if (I_timeOnlyA > I_timeOnlyB)
  {
    I_timeBothAB = I_timeOnlyB;
    I_timeOnlyA -= I_timeOnlyB;
    bFlagAB = true;
  }

  spectro.StartIntegAB();
#ifdef DEBUG_MODE
  Serial.println("Start Integ AB");
#endif
  spectro.PulseClkAB(I_timeBothAB);
#ifdef DEBUG_MODE
  Serial.print("Start AB clock = ");
  Serial.println(I_timeBothAB);
#endif

  if (bFlagAB == false)
  {
    spectro.StopIntegA();
#ifdef DEBUG_MODE
    Serial.println("Stop A Integ");
#endif

    if (I_timeOnlyB < 87)
    {
      spectro.PulseClkAB(I_timeOnlyB);
#ifdef DEBUG_MODE
      Serial.print("Start AB clock = ");
      Serial.println(I_timeOnlyB);
#endif

      spectro.StopIntegB();
#ifdef DEBUG_MODE
      Serial.println("Stop B Integ");
#endif

      P_timeBothAB = 87 - I_timeOnlyB;
      spectro.PulseClkAB(P_timeBothAB);
#ifdef DEBUG_MODE
      Serial.print("Start AB clock = ");
      Serial.println(P_timeBothAB);
#endif

      spectro.PulseClkB(I_timeOnlyB);
#ifdef DEBUG_MODE
      Serial.print("Start B clock = ");
      Serial.println(I_timeOnlyB);
#endif
    }
    else // if (I_timeOnlyB > 87)
    {
      spectro.PulseClkAB(87);
#ifdef DEBUG_MODE
      Serial.println("Start AB clock = 87");
#endif

      I_timeOnlyB -= 87;
      spectro.PulseClkB(I_timeOnlyB);
#ifdef DEBUG_MODE
      Serial.print("Start B clock = ");
      Serial.println(I_timeOnlyB);
#endif

      spectro.StopIntegB();
#ifdef DEBUG_MODE
      Serial.println("Stop B Integ");
#endif

      spectro.PulseClkB(87);
#ifdef DEBUG_MODE
      Serial.println("Start B clock = 87");
#endif
    }
  }
  else // if (bFlagAB == true)
  {
    spectro.StopIntegB();
#ifdef DEBUG_MODE
    Serial.println("Stop B Integ");
#endif

    if (I_timeOnlyA < 87)
    {
      spectro.PulseClkAB(I_timeOnlyA);
#ifdef DEBUG_MODE
      Serial.print("Start AB clock = ");
      Serial.println(I_timeOnlyA);
#endif

      spectro.StopIntegA();
#ifdef DEBUG_MODE
      Serial.println("Stop A Integ");
#endif

      P_timeBothAB = 87 - I_timeOnlyA;
      spectro.PulseClkAB(P_timeBothAB);
#ifdef DEBUG_MODE
      Serial.print("Start AB clock = ");
      Serial.println(P_timeBothAB);
#endif

      spectro.PulseClkA(I_timeOnlyA);
#ifdef DEBUG_MODE
      Serial.print("Start A clock = ");
      Serial.println(I_timeOnlyA);
#endif
    }
    else // if (I_timeOnlyA > 87)
    {
      spectro.PulseClkAB(87);
#ifdef DEBUG_MODE
      Serial.println("Start AB clock = 87");
#endif

      I_timeOnlyA -= 87;
      spectro.PulseClkA(I_timeOnlyA);
#ifdef DEBUG_MODE
      Serial.print("Start A clock = ");
      Serial.println(I_timeOnlyA);
#endif

      spectro.StopIntegA();
#ifdef DEBUG_MODE
      Serial.println("Stop A Integ");
#endif

      spectro.PulseClkA(87);
#ifdef DEBUG_MODE
      Serial.println("Start A clock = 87");
#endif
    }
  }
  
  spectro.ReadVedioAB(data);

#ifndef DEBUG_MODE
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
#endif

}
  
