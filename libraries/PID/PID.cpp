#include <PID.h>
#include <Arduino.h>
#include <Stdio.h>

PID::PID()
{}
void PID::Init( long long p_limit, long long i_limit,long long error_limit)
{
	g_errorsum = 0;
	g_index=0;
	g_p_limit = p_limit;
    g_i_limit = i_limit;//20171107
    g_errorlimit = error_limit;
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
	long p_term, output;
	long long esumki;//
	if(en)
	{
        g_errorsum+=errin;
		if(g_errorsum > g_errorlimit) g_errorsum = g_errorlimit ;
		else if(g_errorsum < (-1)*g_errorlimit) g_errorsum = (-1)*g_errorlimit ;
		p_term = kp * errin;
		
        //if (g_errorsum>0)g_i_term = (g_errorsum >>ls)*ki;//20161105 divieded first to avoid 
		//else if (g_errorsum<0)g_i_term = (-1)*long((abs(g_errorsum)>>ls)*ki);//20161105
        
        g_i_term = (long)(((long long)(g_errorsum)*(long long)(ki))>>ls);//20161105 divieded first to avoid 
        
        //esumki=(long long)(g_errorsum)*(long long)(ki);//
        
        if(p_term > g_p_limit) p_term = g_p_limit;
		else if(p_term < (-1)*g_p_limit) p_term = (-1)*g_p_limit ;
		
        if(g_i_term > g_i_limit) g_i_term = g_i_limit;//20161107 added
		else if(g_i_term < (-1)*g_i_limit) g_i_term = (-1)*g_i_limit ;//
        
        output = -(p_term+g_i_term);//20161027
    

//        #ifdef DEBUGFLAG01
//		unsigned long t1;
//		g_index++;
//		t1= micros();
//		if(g_index == PIDDEBUGFLAG )
//		{
//			g_index = 0;
//		    Serial.print(kp);
//			Serial.print(",");
//			Serial.print(errin);
//			Serial.print(",");
//            //Serial.print(g_errorsum);
//            //Serial.print(",");
//            
//            Serial.print(long(g_errorsum>>32),HEX);
//            Serial.print(",");
//            Serial.print(long(g_errorsum),HEX);
//            Serial.print(",");
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

	}
	else
	{
        g_errorsum =0;
		output =0;

	}
	////////add to show parameters////////
        g_en=en;
        g_errin=errin;
        g_kp=kp;
        g_ki=ki;
        g_ls=ls;
        g_p_term=p_term;
        g_out=output;
	return output;
}
void PID::showParameter()
{
	#ifdef DEBUGFLAG01
		unsigned long t1;
		g_index++;
		t1= micros();
		if(g_index == PIDDEBUGFLAG )
		{
			g_index = 0;
			Serial.print(g_en);
			Serial.print(",");
		    Serial.print(g_kp);
			Serial.print(",");
			Serial.print(g_errin);
			Serial.print(",");
            Serial.print(g_ki);
			Serial.print(",");
			Serial.print(g_ls);
			Serial.print(",");
			Serial.print(g_p_term);
			Serial.print(",");
			Serial.print(g_i_term);
			Serial.print(",");
			Serial.println(g_out);
//			Serial.print(",");
//			Serial.println(t1);
		}
		#else
		#endif
	
} 
//long PID::Computedtc(bool en, long errin, unsigned char kp, unsigned char ki, unsigned char ls, long u_limit)
//{
//	int derr, es;
//	long p_term, i_term, output, p_limit, errorsum_limit;
//	if(en)
//	{
//		g_errorsum+=errin;
//		p_limit = g_p_limit;
//		errorsum_limit = g_errorlimit;
//		if(g_errorsum > g_errorlimit) g_errorsum = g_errorlimit ;
//		else if(g_errorsum < (-1)*g_errorlimit) g_errorsum = (-1)*g_errorlimit ;
//		derr = errin;
//		p_term = kp * errin;
//		
//        if (g_errorsum>0)i_term = (g_errorsum >>ls)*ki*kp;//20161107 
//		else if (g_errorsum<0)i_term = (-1)*long((abs(g_errorsum)>>ls)*ki*kp);//20161105
//        
//        if(p_term > g_p_limit) p_term = g_p_limit;
//		else if(p_term < (-1)*g_p_limit) p_term = (-1)*g_p_limit ;
//		
//        if(i_term > g_i_limit) i_term = g_i_limit;//20161107 added
//		else if(i_term < (-1)*g_i_limit) i_term = (-1)*g_i_limit ;//
//        
//        output = -(p_term+i_term);//20161027		
//        es =u_limit- abs(output);
//        if(es > 0) es =0;
//        
//        if (g_errorsum>0)i_term = (g_errorsum >>ls)*ki*(kp+es);//20161107 
//		else if (g_errorsum<0)i_term = (-1)*long((abs(g_errorsum)>>ls)*ki*(kp+es));
//		output = -(p_term+i_term);
//
//		     
//		#ifdef DEBUGFLAG01
//		unsigned long t1;
//		g_index++;
//		t1= micros();
//		if(g_index == PIDDEBUGFLAG )
//		{
//			g_index = 0;
//		    Serial.print(kp);
//			Serial.print(",");
//			Serial.print(errin);
//			Serial.print(",");
//            Serial.print(g_errorsum);
//            Serial.print(",");
//            Serial.print(ki);
//			Serial.print(",");
//			Serial.print(ls);
//			Serial.print(",");
//			Serial.print(p_term);
//			Serial.print(",");
//			Serial.print(i_term);
//			Serial.print(",");
//			Serial.print(output);
//			Serial.print(",");
//			Serial.print(u_limit);
//			Serial.print(",");
//			Serial.println(es);
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

