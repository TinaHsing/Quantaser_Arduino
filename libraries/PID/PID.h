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
	public:
		PID();		
//		void Init(long long p_limit,long long i_limit,long long error_limit);
		void Init(long long ,long long ,unsigned char, unsigned char , unsigned char );
		long Compute(bool, long, unsigned char, unsigned char, unsigned char);
		
		long long g_errorsum; //Adam@1112, change to public
		long g_i_term, g_i_term_bank, g_p_term;
		long g_errin, g_out;
		unsigned char g_kp, g_ki, g_ls, g_errgain, g_ki_temp, g_ls_temp;
		unsigned int g_index;
};
