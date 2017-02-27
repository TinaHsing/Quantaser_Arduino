#include <LTC2451.h>

LTC2451::LTC2451()
{}
void LTC2451::Init()
{
	Wire.begin();
	Wire.beginTransmission(LTC2451ADD);
	Wire.write(1);
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
bool LTC2451::SoftI2CInit(unsigned char sdapin, unsigned char sclpin)
{
//	Wire.begin();
	g_sdapin = sdapin;
	g_sclpin = sclpin;
	softi2c.init(g_sdapin,g_sclpin);
	if(softi2c.Start(LTC2451ADD,I2CWRITE))
	{	
		softi2c.Write(1);
		softi2c.Stop();
		return 0;
	}
	else return 1;
}
unsigned int LTC2451::SoftI2CRead()
{
	unsigned char i, data[2];
	unsigned int vout;
	 Serial.println("1");
	if(softi2c.Start(LTC2451ADD,I2CREAD))
	{ Serial.println("2");
		for(i =0; i< 2; i++)
			{
				data[i] = softi2c.Read(1);
			}
		softi2c.Stop();
		vout = data[0] << 8 | data[1];
		Serial.print(data[0]);
		Serial.print(", ");
		Serial.println(data[1]);
	}

	return vout;
}

