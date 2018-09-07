#include <LTC1865.h>
#include <SPI.h>
#define LTC1865CONV 7 //PD7

LTC1865 ltc1865;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  ltc1865.init(LTC1865CONV,0);// ch0 is the first channel to read)
//  ltc1865.init(LTC1865CONV,1);// ch0 is the first channel to read)
}

void loop() {
  // put your main code here, to run repeatedly:
 unsigned int data0, data1;
 data0 = ltc1865.Read(1); // read the ch0 data and next channel is ch1
 data1 = ltc1865.Read(0); // read the ch1 data and next chanel is ch0
// Serial.print(" Vact:,");
// Serial.println(data0);
 Serial.print("ch0: ");
 Serial.print(data0);
 Serial.print(", ");
 Serial.println((float)data0/65535.0*5.0);
  Serial.print("ch1: ");
 Serial.print(data1);
 Serial.print(", ");
 Serial.println((float)data1/65535.0*5.0);
 delay(1000);
}
