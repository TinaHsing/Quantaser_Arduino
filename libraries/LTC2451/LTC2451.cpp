#include <LTC2451.h>

LTC2451::LTC2451()
{}
void LTC2451::Init(unsigned char mode)
{
	Wire.begin();
	Wire.beginTransmission(LTC2451ADD);
	Wire.write(mode);
	Wire.endTransmission();
}
unsigned int LTC2451::Read()
{
	bool i=0;
	unsigned char voltage[2];
	unsigned int vout;
	Wire.requestFrom(LTC2451ADD,2);
	while (Wire.available())
	{
		voltage[i]=Wire.read();
		i++;
	}
	
	vout = voltage[0] << 8 | voltage[1];
	return vout;
}
bool LTC2451::SoftI2CInit(unsigned char sdapin, unsigned char sclpin, unsigned char mode)
{

	g_sdapin = sdapin;
	g_sclpin = sclpin;
	softi2c.init(g_sdapin,g_sclpin);
	if(softi2c.start(LTC2451ADD_SWRITE))  
	{	
		softi2c.write(mode);
		softi2c.stop();

		return 1;
	}
	else return 0;
}
unsigned int LTC2451::SoftI2CRead()
{
	unsigned char i, data[2];
	unsigned int vout;

	if(softi2c.start(LTC2451ADD_SREAD)) 
	{
		for(i =0; i< 2; i++)
			{
				data[i] = softi2c.read(0);
			}
		softi2c.stop();
		vout = data[0] << 8 | data[1];

	}

	return vout;
}

