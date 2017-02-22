#define LTC2451ADD 0x14

class LTC2451
{
	public:
		LTC2451();
		void Init();
		unsigned int Read();
};