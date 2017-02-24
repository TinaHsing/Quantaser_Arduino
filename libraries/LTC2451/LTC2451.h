#define LTC2451ADD 0x14
#define I2CREAD 1
#define I2CWRITE 0

class SoftI2C;
class LTC2451
{
	public:
		LTC2451();
		void Init();
		bool SoftI2CInit(unsigned char, unsigned char);
		unsigned int Read();
		unsigned int SoftI2CRead();
	private:
		unsigned char g_sdapin, g_sclpin;
		SoftI2C softi2c;
};