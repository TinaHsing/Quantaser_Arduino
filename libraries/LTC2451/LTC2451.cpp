#include <Wire.h>
#include <LTC2451.h>

LTC2451::LTC2451()
{}
void LTC2451::Init()
{
	Wire.beginTransmission(LTC2451ADD);
	Wire.write(1);
	Wire.endTransmission();
}
unsigned int LTC2451::Read()
{
	bool i=0;
	unsigned int voltage[2];
	unsigned int vout;
	Wire.requestFrom(LTC2451ADD,2);
	while (Wire.available())
	{
		voltage[i]=Wire.read();
		i++;
	}
	vout = voltage[0] << 8 + voltage[1];
	return vout;
}