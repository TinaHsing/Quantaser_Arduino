#include <SawtoothGen.h> 

SawtoothGen::SawtoothGen(uint8_t refsens, uint8_t outputsens, uint8_t ctrl, float min, float max) 
{
	p_refsense = refsens;
	p_outputsense = outputsens;
	p_chargecontrol = ctrl;
	
	pinMode(p_refsense, INPUT);
	pinMode(p_outputsense, INPUT);
	pinMode(p_chargecontrol, OUTPUT);
	digitalWrite(p_chargecontrol, LOW); // initial charge C
	
	p_minvolt = int(min/5.0*1024) ;
	p_maxvolt = int(max/5.0*1024) ;
	p_compflag = 1;
}
SawtoothGen::SawtoothGen(uint8_t integrator_sense, uint8_t charge_ctrl) 
{
	p_outputsense = integrator_sense;
	p_chargecontrol = charge_ctrl;
	
	pinMode(p_outputsense, INPUT);
	pinMode(p_chargecontrol, OUTPUT);
	digitalWrite(p_chargecontrol, LOW); // initial charge C
}
void SawtoothGen::Sawtooth_out() 
{
	int AI_read;
	
	AI_read = analogRead(p_outputsense);	
	if(AI_read >= AI_MAX) digitalWrite(p_chargecontrol,HIGH); // discharge C					
	if(AI_read <= AI_MIN) digitalWrite(p_chargecontrol,LOW); // charge C

}
//void SawtoothGen::Sawtooth_out() 
//{
//	unsigned int vref, vout;
//	
//	vout = analogRead(p_outputsense);
//	if(p_compflag) {
//		vref = analogRead(p_refsense);
//		if(vout>=vref || vout>=p_maxvolt){
//			digitalWrite(p_chargecontrol,HIGH); // discharge C
//    		p_compflag=0;
//		} 		
//	}
//	else{
//	   if(vout <= p_minvolt){
//	    digitalWrite(p_chargecontrol,LOW); // charge C
//	    p_compflag=1;
//	   }
//    }
//}
