#define LTC2451ADD 0x14
#define LTC2451ADD_SWRITE 0x28 // For softi2c communication 0x14 << 1
#define LTC2451ADD_SREAD 0x29
#define I2CREAD 1
#define I2CWRITE 0

#include <I2CMaster.h>
#include <Wire.h>
class SoftI2cMaster;
class LTC2451
{
	public:
		LTC2451();
		void Init(unsigned char);
		bool SoftI2CInit(unsigned char, unsigned char, unsigned char);
		unsigned int Read();
		unsigned int SoftI2CRead();
	private:
		unsigned char g_sdapin, g_sclpin;
		SoftI2cMaster softi2c;
};
