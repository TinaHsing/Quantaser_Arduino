#include <I2cMaster.h>
#include <Wire.h>
#include <LTC2451.h>
#define MODE30HZ 1
#define MODE60HZ 0

#define SDA_PIN 4
#define SCL_PIN 3
LTC2451 adc;
void setup() {

  Serial.begin(9600);
  Serial.println(adc.SoftI2CInit(SDA_PIN, SCL_PIN, MODE30HZ));
  delay(35); // 30Hz mode, next request should be later then 33ms later!!
 }
void loop() {
  unsigned int x; 
  x =adc.SoftI2CRead();
  Serial.print("adc:");
  Serial.println(x);
  delay(35);
}


