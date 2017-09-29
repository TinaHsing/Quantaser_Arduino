#include <PID.h>
#include <Arduino.h>
#include <Stdio.h>

PID::PID()
{}
//void PID::Init( long long p_limit, long long i_limit,long long error_limit)
//{
//	g_errorsum = 0;
//	g_index=0;
//	g_p_limit = p_limit;
//    g_i_limit = i_limit;//20171107
//    g_errorlimit = error_limit;
//    #ifdef DEBUGFLAG01
//		Serial.println("====================comput print detail=============");
//		Serial.print("loopcount to show:");
//		Serial.println(PIDDEBUGFLAG);
//		Serial.println("errin, kp, ki, ls, p_term, i_term, output");
//	#else
//	#endif
//}
void PID::Init( long long p_limit, long long i_limit, unsigned char ki, unsigned char ls, unsigned char err_gain)
//void PID::Init( long long p_limit, long long i_limit,long long error_limit)
{
	g_errorsum = 0;
	g_index=0;
	g_p_limit = p_limit;
    g_i_limit = i_limit;//20171107
    g_i_term = 0;
	g_i_term_bank = 0; 
    g_errorlimit = (g_i_limit<<ls)/ki;
    Serial.print("g_errorlimit: ");
    Serial.print((unsigned long)g_i_limit);
    Serial.print(", ");
    Serial.print(ls);
    Serial.print(", ");
    Serial.print(ki);
    Serial.print(", ");
    Serial.println((long) g_errorlimit);
    g_errgain = err_gain;
//    #ifdef DEBUGFLAG01
//		Serial.println("====================comput print detail=============");
//		Serial.print("loopcount to show:");
//		Serial.println(PIDDEBUGFLAG);
//		Serial.println("kp, errin, errorsum, ki, ls, p_term, i_term, output");
//	#else
//	#endif
}

long PID::Compute(bool en, long errin, unsigned char kp, unsigned char ki, unsigned char ls)
{
	long p_term, i_term, output;
	long long esumki;//
	unsigned long t1;

	
	errin = errin >> g_errgain;

	if(en)
	{	
		if(ki != g_ki_temp ||  ls != g_ls_temp) 
		{
			g_errorlimit =  (g_i_limit<<ls)/ki;		
		}
		
        g_errorsum += errin;        
		if(g_errorsum >= g_errorlimit) g_errorsum = g_errorlimit ;
		else if(g_errorsum <= (-1)*g_errorlimit) g_errorsum = (-1)*g_errorlimit ;
		
		p_term = kp * errin;       
        i_term = (long)(((long long)(g_errorsum)*(long long)(ki))>>ls);
        if(ki != g_ki_temp ||  ls != g_ls_temp) 
		{
			g_i_term_bank = g_i_term;
        	g_errorsum = 0;	
			i_term = 0;		
		}

		g_i_term = g_i_term_bank + i_term;

//        g_i_term += ((errin*ki)>>ls);
        
        if(p_term > g_p_limit) p_term = g_p_limit;
		else if(p_term < (-1)*g_p_limit) p_term = (-1)*g_p_limit ;
		
		if(g_i_term > g_i_limit) g_i_term = g_i_limit;
		else if(g_i_term < (-1)*g_i_limit) g_i_term = (-1)*g_i_limit ;
		
//        if(i_term > g_i_limit) i_term = g_i_limit;//20161107 added
//		else if(i_term < (-1)*g_i_limit) i_term = (-1)*g_i_limit ;
        g_ki_temp = ki;
        g_ls_temp = ls;
        output = -(p_term+g_i_term);//20161027
	}
	else
	{
		p_term=0; 
		g_i_term = 0;
//		i_term=0;
        g_errorsum =0;
		output =0;

	}
	////////add to show parameters////////
        g_errin=errin;
        g_kp=kp;
        g_ki=ki;
        g_ls=ls;
        g_p_term=p_term;
        g_out=output;
        
        #ifdef DEBUGFLAG01
		
		g_index++;
		t1= micros();
		if(g_index == PIDDEBUGFLAG )
		{
			g_index = 0;
			Serial.print(g_errin);
			Serial.print(",");			
		    Serial.print(g_kp);
			Serial.print(",");			
            Serial.print(g_ki);
			Serial.print(",");
			Serial.print(g_ls);
			Serial.print(",");
			Serial.print(p_term);
			Serial.print(",");
			Serial.print(g_i_term);
			Serial.print(",");
			Serial.println(g_out);
//			Serial.print(",");
//			Serial.println(en);
//			Serial.print(",");
//			Serial.print((long) g_errorlimit,HEX);
//			Serial.print(",");
//			Serial.print( (long)(g_errorsum>>32) ,HEX);
//			Serial.print(",");
//			Serial.println((long) g_errorsum,HEX);
		}
		#else
		#endif
	return output;
}
//long PID::Compute(bool en, long errin, unsigned char kp, unsigned char ki, unsigned char ls)
//{
//	long p_term, output;
//	long long esumki;//
//	if(en)
//	{
//        g_errorsum+=errin; 
//		if(g_errorsum > g_errorlimit) g_errorsum = g_errorlimit ;
//		else if(g_errorsum < (-1)*g_errorlimit) g_errorsum = (-1)*g_errorlimit ;
//		p_term = kp * errin;
//		
//        
//        g_i_term = (long)(((long long)(g_errorsum)*(long long)(ki))>>ls);//20161105 divieded first to avoid 
//                
//        if(p_term > g_p_limit) p_term = g_p_limit;
//		else if(p_term < (-1)*g_p_limit) p_term = (-1)*g_p_limit ;
//		
//        if(g_i_term > g_i_limit) g_i_term = g_i_limit;//20161107 added
//		else if(g_i_term < (-1)*g_i_limit) g_i_term = (-1)*g_i_limit ;//
//        
//        output = -(p_term+g_i_term);//20161027
//
//
//        #ifdef DEBUGFLAG01
//		unsigned long t1;
//		g_index++;
//		t1= micros();
//		if(g_index == PIDDEBUGFLAG )
//		{
//			g_index = 0;
//			Serial.print(errin);
//			Serial.print(",");
//		    Serial.print(kp);
//			Serial.print(",");			
//            //Serial.print(g_errorsum);
//            //Serial.print(",");
//            
////            Serial.print(long(g_errorsum>>32),HEX);
////            Serial.print(",");
////            Serial.print(long(g_errorsum),HEX);
////            Serial.print(",");
//            
//            //Serial.print(long(esumki),HEX);//
//            //Serial.print(",");//
//            //Serial.print(long(esumki>>32),HEX);//
//            //Serial.print(",");//
//            
//            Serial.print(ki);
//			Serial.print(",");
//			Serial.print(ls);
//			Serial.print(",");
//			Serial.print(p_term);
//			Serial.print(",");
//			Serial.print(g_i_term);
//			Serial.print(",");
//			Serial.println(output);
////			Serial.print(",");
////			Serial.println(t1);
//		}
//		#else
//		#endif
//
//	}
//	else
//	{
//        g_errorsum =0;
//		output =0;
//
//	}
//	return output;
//}


