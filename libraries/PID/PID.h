//==============Useful define variable it is suggest to copy this define to main program and remark these
#define PIDDEBUGFLAG 1500 // User choose how many times to run compute and show the paramter in Serail once
#define DEBUGFLAG01

#ifdef PIDDEBUG
	#define DEBUGFLAG01 PIDDEBUG
#else
#endif
class PID
{
	private:
		long long  g_p_limit, g_i_limit, g_errorlimit;//, g_errorsum;
		//long long g_errorsum;
        unsigned int g_index; 
	public:
		PID();		
		void Init(long long p_limit,long long i_limit,long long error_limit);
		long Compute(bool en, long errin, unsigned char kp, unsigned char ki, unsigned char ls);
		long Computedtc(bool en, long errin, unsigned char kp, unsigned char ki, unsigned char ls, long u_limit);
		void showParameter();
		
		long long g_errorsum; //Adam@1112, change to public
		long g_i_term, g_p_term;
		///////add to show parameters////////
		bool g_en;
		long g_errin, g_out;
		unsigned char g_kp, g_ki, g_ls;
};
