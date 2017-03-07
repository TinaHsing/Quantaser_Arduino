/* modification for disable sensortype purpose : data 2016/11/13
	1. in I2CReadData(), set g_sensortype to 0;
	2. add g_mod_status variable in DTC03Master_P02.h
	3. add VMOD x and y GLCD coordinate in DTC03Master_P02.h
	4. add PrintModStatus

*/
#include <DTC03Master_V300.h>
DTC03Master::DTC03Master()
{
}
void DTC03Master::SetPinMode()
{
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
//  pinMode(PUSHB, INPUT);
//  pinMode(ENSW, INPUT);
}
void DTC03Master::ParamInit()
{
  digitalWrite(ENC_A, HIGH);
  digitalWrite(ENC_B, HIGH);
  
  Wire.begin();
  lcd.Init();
  g_paramupdate = 0;
  g_sensortype=0;
  g_tsetstep = 1.00;
  g_en_state = 0;
  g_countersensor = 0;
  g_cursorstate=1;
  p_cursorStateCounter[0]=0;
  p_cursorStateCounter[1]=0;
  p_cursorStateCounter[2]=0;
  p_cursorStayTime=0;
  p_tBlink=0;
  p_tBlink_toggle=0;
  p_engModeFlag=0;
  p_blinkTsetCursorFlag=0;
  p_loopindex=0;
  p_ee_changed=0;
  p_holdCursorTimer=0;
  p_HoldCursortateFlag=0;
  g_wakeup = 1;
  g_test=0;
  
  for (int i=0;i<MV_ROW; i++) 
  {
  	p_MV_sum[i]=0;
  	p_MV_index[i]=0;
  	for (int j=0;j<MVTIME;j++) p_MV_array[i][j]=0;
  }
}
void DTC03Master::WelcomeScreen()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(0,0);
  lcd.print("5 in 1 Ver.1.00");
  lcd.GotoXY(0,ROWPIXEL0507*1);
  lcd.print("Initializing");
  for (byte i=5; i>0; i--)
  {
    lcd.GotoXY(COLUMNPIXEL0507*(12+1),ROWPIXEL0507*1);
    lcd.print(i);
    delay(1000);
  }
  lcd.ClearScreen(0);//0~255 means ratio of black  
}
void DTC03Master::ReadEEPROM()
{
	unsigned char noeedummy, temp_upper, temp_lower;
	noeedummy = EEPROM.read(EEADD_DUMMY);
	if(noeedummy == NOEE_DUMMY)
	{
		//DTC03
		g_vset = EEPROM.read(EEADD_VSET_UPPER)<<8 | EEPROM.read(EEADD_VSET_LOWER);
		g_currentlim = EEPROM.read(EEADD_currentlim);
		g_p = EEPROM.read(EEADD_P);
		g_kiindex = EEPROM.read(EEADD_KIINDEX);		
		g_bconst=EEPROM.read(EEADD_BCONST_UPPER)<<8 | EEPROM.read(EEADD_BCONST_LOWER);		
		g_mod_status = EEPROM.read(EEADD_MODSTATUS);
        g_r1 = EEPROM.read(EEADD_R1);
        g_r2 = EEPROM.read(EEADD_R2);
        g_tpidoff = EEPROM.read(EEADD_TPIDOFF);
        g_fbcbase = EEPROM.read(EEADD_FBC_UPPER)<<8 | EEPROM.read(EEADD_FBC_LOWER);
        g_vmodoffset = EEPROM.read(EEADD_MODOFF_UPPER)<<8 |  EEPROM.read(EEADD_MODOFF_LOWER);
        g_Rmeas = EEPROM.read(EEADD_RMEAS_UPPER)<<8 | EEPROM.read(EEADD_RMEAS_LOWER); 
        g_otp = EEPROM.read(EEADD_TOTP_UPPER)<<8 | EEPROM.read(EEADD_TOTP_LOWER);
        //DTC03_2
		g_vset2 = EEPROM.read(EEADD_VSET_UPPER_2)<<8 | EEPROM.read(EEADD_VSET_LOWER_2);
		g_p2 = EEPROM.read(EEADD_P_2);
		g_kiindex2 = EEPROM.read(EEADD_KIINDEX_2);		
        g_fbcbase2 = EEPROM.read(EEADD_FBC_UPPER_2)<<8 | EEPROM.read(EEADD_FBC_LOWER_2);
        g_vmodoffset2 = EEPROM.read(EEADD_MODOFF_UPPER_2)<<8 |  EEPROM.read(EEADD_MODOFF_LOWER_2);
        g_Rmeas2 = EEPROM.read(EEADD_RMEAS_UPPER_2)<<8 | EEPROM.read(EEADD_RMEAS_LOWER_2); 
        //LCD200I2C
        g_dacout = EEPROM.read(EEADD_IOUT_UPPER)<<8 | EEPROM.read(EEADD_IOUT_LOWER);
        g_vfth1 = EEPROM.read(EEADD_VTH1);
        g_vfth2 = EEPROM.read(EEADD_VTH2);
	}
	else
	{
		EEPROM.write(EEADD_DUMMY, NOEE_DUMMY);
		//DTC03		
		EEPROM.write(EEADD_VSET_UPPER, NOEE_VSET>>8);
		EEPROM.write(EEADD_VSET_LOWER, NOEE_VSET);
		EEPROM.write(EEADD_currentlim, NOEE_ILIM);		
		EEPROM.write(EEADD_P, NOEE_P);		
		EEPROM.write(EEADD_KIINDEX, NOEE_kiindex);
		EEPROM.write(EEADD_BCONST_UPPER, NOEE_BCONST>>8);
		EEPROM.write(EEADD_BCONST_LOWER, NOEE_BCONST);
		EEPROM.write(EEADD_MODSTATUS, NOEE_MODSTATUS);
		EEPROM.write(EEADD_R1, NOEE_R1);
		EEPROM.write(EEADD_R2, NOEE_R2);		
		EEPROM.write(EEADD_TPIDOFF, NOEE_TPIDOFF);
		EEPROM.write(EEADD_FBC_UPPER, NOEE_FBC>>8);
		EEPROM.write(EEADD_FBC_LOWER, NOEE_FBC);
		EEPROM.write(EEADD_MODOFF_UPPER, NOEE_MODOFF>>8);
		EEPROM.write(EEADD_MODOFF_LOWER, NOEE_MODOFF);		
		EEPROM.write(EEADD_RMEAS_UPPER, NOEE_RMEAS>>8);
		EEPROM.write(EEADD_RMEAS_LOWER, NOEE_RMEAS);
		EEPROM.write(EEADD_TOTP_UPPER, NOEE_TOTP>>8);
		EEPROM.write(EEADD_TOTP_LOWER, NOEE_TOTP);
		//DTC03_2
		EEPROM.write(EEADD_VSET_UPPER_2, NOEE_VSET>>8);
		EEPROM.write(EEADD_VSET_LOWER_2, NOEE_VSET);
		EEPROM.write(EEADD_currentlim_2, NOEE_ILIM);		
		EEPROM.write(EEADD_P_2, NOEE_P);		
		EEPROM.write(EEADD_KIINDEX_2, NOEE_kiindex);
		EEPROM.write(EEADD_BCONST_UPPER_2, NOEE_BCONST>>8);
		EEPROM.write(EEADD_BCONST_LOWER_2, NOEE_BCONST);
		EEPROM.write(EEADD_MODSTATUS_2, NOEE_MODSTATUS);
		EEPROM.write(EEADD_R1_2, NOEE_R1);
		EEPROM.write(EEADD_R2_2, NOEE_R2);		
		EEPROM.write(EEADD_TPIDOFF_2, NOEE_TPIDOFF);
		EEPROM.write(EEADD_FBC_UPPER_2, NOEE_FBC2>>8);
		EEPROM.write(EEADD_FBC_LOWER_2, NOEE_FBC2);
		EEPROM.write(EEADD_MODOFF_UPPER_2, NOEE_MODOFF2>>8);
		EEPROM.write(EEADD_MODOFF_LOWER_2, NOEE_MODOFF2);		
		EEPROM.write(EEADD_RMEAS_UPPER_2, NOEE_RMEAS2>>8);
		EEPROM.write(EEADD_RMEAS_LOWER_2, NOEE_RMEAS2);
		EEPROM.write(EEADD_TOTP_UPPER_2, NOEE_TOTP>>8);
		EEPROM.write(EEADD_TOTP_LOWER_2, NOEE_TOTP);
		//DTC03
		g_vset = NOEE_VSET;
		g_currentlim = NOEE_ILIM;
		g_p = NOEE_P;		
		g_kiindex = NOEE_kiindex;		
		g_bconst = NOEE_BCONST;
		g_mod_status = NOEE_MODSTATUS;
		g_r1 = NOEE_R1;
		g_r2 = NOEE_R2;
		g_tpidoff = NOEE_TPIDOFF;		
		g_fbcbase = NOEE_FBC;		
		g_vmodoffset = NOEE_MODOFF;		
		g_Rmeas = NOEE_RMEAS;		
		g_otp = NOEE_TOTP; 
		//DTC03_2
		g_vset2 = NOEE_VSET;
		g_p2 = NOEE_P;
		g_kiindex2 = NOEE_kiindex;
		g_fbcbase2 = NOEE_FBC2;
		g_vmodoffset2 = NOEE_MODOFF2;
		g_Rmeas2 = NOEE_RMEAS2;
		//LCD200I2C
		EEPROM.write(EEADD_IOUT_UPPER, NOEE_IOUT>>8);
		EEPROM.write(EEADD_IOUT_LOWER, NOEE_IOUT);
		EEPROM.write(EEADD_VTH1, NOEE_VTH1);
		EEPROM.write(EEADD_VTH2, NOEE_VTH2);
		
		g_dacout = NOEE_IOUT;
        g_vfth1 = NOEE_VTH1;
        g_vfth2 = NOEE_VTH2;
	}	
    g_tset = ReturnTemp(g_vset, 0); 
   	g_tset2 = ReturnTemp(g_vset2, 0); 
}
void DTC03Master::SaveEEPROM() {	
	if (p_ee_changed==1) {
		p_ee_changed = 0;
		switch(p_ee_change_state){
			//DTC03
            case EEADD_VSET_UPPER:
                EEPROM.write(EEADD_VSET_UPPER, g_vset>>8 );
                EEPROM.write(EEADD_VSET_LOWER, g_vset);
                break;
    
            case EEADD_BCONST_UPPER:
                EEPROM.write(EEADD_BCONST_UPPER, g_bconst>>8 );
                EEPROM.write(EEADD_BCONST_LOWER, g_bconst); 
                break;

            case EEADD_MODSTATUS:
                EEPROM.write(EEADD_MODSTATUS, g_mod_status);
                break;
                
            case EEADD_currentlim:
                EEPROM.write(EEADD_currentlim, g_currentlim);
                break;
                
            case EEADD_FBC_UPPER:
                EEPROM.write(EEADD_FBC_UPPER, g_fbcbase>>8);
                EEPROM.write(EEADD_FBC_LOWER, g_fbcbase);
                break;
                
            case EEADD_P:
                EEPROM.write(EEADD_P, g_p);
                break;
                
            case EEADD_KIINDEX:
                EEPROM.write(EEADD_KIINDEX, g_kiindex );
                break;
             
			case EEADD_TOTP_UPPER:
                EEPROM.write(EEADD_TOTP_UPPER, (g_otp>>8));
                EEPROM.write(EEADD_TOTP_LOWER, g_otp);
                break;
               
			case EEADD_R1:
                EEPROM.write(EEADD_R1, g_r1);
                break;
               
			case EEADD_R2:
                EEPROM.write(EEADD_R2, g_r2);
                break;  
                
			case EEADD_TPIDOFF:
                EEPROM.write(EEADD_TPIDOFF, g_tpidoff);
                break;
			case EEADD_MODOFF_UPPER:
                EEPROM.write(EEADD_MODOFF_UPPER, g_vmodoffset >>8);
                EEPROM.write(EEADD_MODOFF_LOWER, g_vmodoffset );
                break;
                
			case EEADD_RMEAS_UPPER:
                EEPROM.write(EEADD_RMEAS_UPPER, g_Rmeas >>8);
                EEPROM.write(EEADD_RMEAS_LOWER, g_Rmeas );
                break; 
                
			//DTC03_2
			case EEADD_VSET_UPPER_2:
                EEPROM.write(EEADD_VSET_UPPER_2, g_vset2>>8 );
                EEPROM.write(EEADD_VSET_LOWER_2, g_vset2);
                break;
                
            case EEADD_P_2:
                EEPROM.write(EEADD_P_2, g_p2);
                break;
			  
			case EEADD_KIINDEX_2:
                EEPROM.write(EEADD_KIINDEX_2, g_kiindex2 );
                break;
			//LCD200I2C	
			case EEADD_IOUT_UPPER:
                EEPROM.write(EEADD_IOUT_UPPER, g_dacout >>8);
                EEPROM.write(EEADD_IOUT_LOWER, g_dacout );
                break; 
				
			case EEADD_VTH1:
                EEPROM.write(EEADD_VTH1, g_vfth1);
                break; 
				
			case EEADD_VTH2:
                EEPROM.write(EEADD_VTH2, g_vfth2);
                break;         
        }
	}
}
void DTC03Master::CheckStatus()
{
		float tact, itec_f, tpcb_f, Ild_f, vpzt_f;
				//DTC03
				if (p_loopindex%300==0) {
					I2CReadData(I2C_COM_ITEC_ER,DTC03P05);
		            itec_f = float(g_itec)*CURRENTRatio;
		            if(!p_engModeFlag) PrintItec(itec_f);
//                if(!g_wakeup) I2CWriteAll();
				}								
				if (p_loopindex%300==1) {
					I2CReadData(I2C_COM_VACT,DTC03P05);
					if (MV_VACT) tact = ReturnTemp(MovingAVG(0,g_vact),0);
	  	    		else tact = ReturnTemp(g_vact,0);
	  	    		if(!p_engModeFlag) PrintTact(tact);
				}	
				if (p_loopindex%300==2) {
					I2CReadData(I2C_COM_PCB,DTC03P05);
		            tpcb_f = float(g_tpcb)/4.0-20.5;
		            if(p_engModeFlag) PrintTpcb(tpcb_f);
				}	
				//LCD200
				if (p_loopindex%300==3) {
					I2CReadData(LCD200_COM_IIN,LCD200ADD);
		            Ild_f = ReturnCurrent(g_Ild);
		            if(!p_engModeFlag) PrintLDcurrentAct(Ild_f);
				}
				//DTC03_2
				if (p_loopindex%300==4) {
					I2CReadData(I2C_COM_ITEC_ER,DTC03P05_2);
		            itec_f = float(g_itec)*CURRENTRatio;
		            if(!p_engModeFlag) PrintItec2(itec_f);
//                if(!g_wakeup2) I2CWriteAll();
				}								
				if (p_loopindex%300==5) {
					I2CReadData(I2C_COM_VACT,DTC03P05_2);
					if (MV_VACT2) tact = ReturnTemp(MovingAVG(1,g_vact),0);
	  	    		else tact = ReturnTemp(g_vact,0);
	  	    		if(!p_engModeFlag) PrintTact2(tact);
				}
				//PZTDRF
				if (p_loopindex%300==6) {
					I2CReadData(PZTDRF_COM_VPZT,PZTDRF);
					vpzt_f = ReturnVpzt(g_vpzt, 15.0);
//					if (MV_PZT) vpzt_f = g_vpzt;
//	  	    		else tact = ReturnTemp(g_vact,0);
	  	    		if(!p_engModeFlag) PrintPZTvolt(vpzt_f);
				}
	    p_loopindex++;		       
}
unsigned int DTC03Master::MovingAVG(unsigned char row, unsigned int Var)
{
	unsigned mv_out;
	p_MV_sum[row] -= p_MV_array[row][ p_MV_index[row] ];
	p_MV_array[row][ p_MV_index[row] ] = Var;
	p_MV_sum[row] += Var;
	mv_out = p_MV_sum[row]>>MVTIME_POWER;
	p_MV_index[row]++;
	if(p_MV_index[row]==MVTIME) p_MV_index[row]=0;
	return mv_out;
}

void DTC03Master::I2CWriteAll()
{
	for (int i=I2C_COM_INIT; i<=I2C_COM_WAKEUP; i++)
	{
		I2CWriteData(i,DTC03P05);
		I2CWriteData(i,DTC03P05_2);
	 } 
	for (int i=LCD200_COM_LDEN; i<=LCD200_COM_VFTH2; i++) I2CWriteData(i,LCD200ADD);
	
	
}
void DTC03Master::I2CWriteData(unsigned char com, unsigned char slaveAdd)
{
  unsigned char temp[2];
  switch(com)
  {
    case I2C_COM_INIT:
//        temp[0]= g_bconst - BCONSTOFFSET;
//        temp[1]= (g_bconst - BCONSTOFFSET) >> 8;
        if(g_en_state) temp[1] |= REQMSK_ENSTATE; //B10000000
        else temp[1] &= ~REQMSK_ENSTATE;
//    if(g_sensortype) temp[1]|= REQMSK_SENSTYPE; 
	    if(g_mod_status) temp[1]|= REQMSK_SENSTYPE; //B01000000 
		else temp[1] &= REQMSK_SENSTYPE;
        break;

    case I2C_COM_CTR:
        temp[0]= g_currentlim;
        switch(slaveAdd)
        {
        	case DTC03P05:
        		temp[1]= g_p;
        	break;
        	case DTC03P05_2:
        		temp[1]= g_p2;
        	break;
		}      
        break;

    case I2C_COM_VSET:
    	switch(slaveAdd)
        {
        	case DTC03P05:
        		temp[0]=g_vset;
       			temp[1]=g_vset>>8;
        	break;
        	case DTC03P05_2:
        		temp[0]=g_vset2;
       			temp[1]=g_vset2>>8;
        	break;
		}       
        break;

    case I2C_COM_R1R2:
        temp[0] = g_r1;
        temp[1] = g_r2;
        break;

    case I2C_COM_TPIDOFF:
        temp[0] = g_tpidoff;
        temp[1] = 0;
        break;

    case I2C_COM_FBC:
    	switch(slaveAdd)
        {
        	case DTC03P05:
        		temp[0] = g_fbcbase;
       			temp[1] = g_fbcbase>>8;
        	break;
        	case DTC03P05_2:
        		temp[0] = g_fbcbase2;
       			temp[1] = g_fbcbase2>>8;
        	break;
		}     
        break;

    case I2C_COM_VMOD:
    	switch(slaveAdd)
        {
        	case DTC03P05:
        		temp[0] = g_vmodoffset;
       			temp[1] = g_vmodoffset >>8;
        	break;
        	case DTC03P05_2:
        		temp[0] = g_vmodoffset2;
       			temp[1] = g_vmodoffset2 >>8;
        	break;
		}        
        break;
    
    case I2C_COM_KI:
    	switch(slaveAdd)
        {
        	case DTC03P05:
        		temp[0]=pgm_read_word_near(kilstable280+g_kiindex*2);
        		temp[1]=pgm_read_word_near(kilstable280+g_kiindex*2+1);
        	break;
        	case DTC03P05_2:
        		temp[0]=pgm_read_word_near(kilstable280+g_kiindex2*2);
        		temp[1]=pgm_read_word_near(kilstable280+g_kiindex2*2+1);
        	break;
		}        
        break;
    
    case I2C_COM_RMEAS:
    	switch(slaveAdd)
        {
        	case DTC03P05:
        		temp[0]=g_Rmeas;
    			temp[1]=g_Rmeas>>8;
        	break;
        	case DTC03P05_2:
        		temp[0]=g_Rmeas2;
    			temp[1]=g_Rmeas2>>8;
        	break;
		}     	
    	break;
    	
    case I2C_COM_OTP:
    		temp[0] = g_otp;
    		temp[1] = g_otp>>8;
    	break;
    	
    case I2C_COM_WAKEUP:
    		temp[0] = 1;
    		temp[1] = 0; // overshoot cancelation, set 0 in DTC03
    	break;
    
    case I2C_COM_TEST1:
        temp[0]=p_temp;
        p_temp++;
//        temp[1]=p_temp>>8;
        break;
    
    case I2C_COM_TEST2:
        temp[0]=g_cursorstate;
        temp[1]=p_tBlink_toggle;
        break;
        
    //LCD200I2C   
    case LCD200_COM_LDEN:
        temp[0]=g_en_state;
        break;
        
    case LCD200_COM_IOUT:
    	temp[0]=g_dacout;
        temp[1]=g_dacout>>8;
        break;    
        
    case LCD200_COM_VFTH1:
        temp[0]=NOEE_VTH1;
        temp[1]=0;
        break;
		
	case LCD200_COM_VFTH2:
        temp[0]=NOEE_VTH2;
        temp[1]=0;
        break;    

  }

  Wire.beginTransmission(slaveAdd);
  Wire.write(com);
  Wire.write(temp, 2);
  Wire.endTransmission();
  delayMicroseconds(I2CSENDDELAY);
}
void DTC03Master::I2CReadData(unsigned char com, unsigned char slaveAdd)
{
  unsigned char temp[2], b_upper, b_lower;
  unsigned int itectemp;
  bool itecsign;
  
  Wire.beginTransmission(slaveAdd);
  Wire.write(com);
  Wire.endTransmission();
  delayMicroseconds(I2CREADDELAY);
  Wire.requestFrom(slaveAdd,2);
  while(Wire.available()==2)
  {
    temp[0] = Wire.read();
    temp[1] = Wire.read();
  }
  switch(com)
  {
  	//DTC03P05 and DTC03P05_2 shared 
    case I2C_COM_VACT: 
		g_vact =(temp[1] <<8) | temp[0];
        break;

    case I2C_COM_ITEC_ER:
    	
        itectemp = ((temp[1] & REQMSK_ITECU) << 8)| temp[0];
        if(itectemp<=1) itectemp=0;   
        itecsign = temp[1] & REQMSK_ITECSIGN;        
        if(itecsign) g_itec = (-1)*(int)itectemp;
        else g_itec = (int)itectemp;
        
        switch(slaveAdd)
        {
        	case DTC03P05:
        		g_errcode1 = temp[1] & REQMSK_ERR1;
                g_errcode2 = temp[1] & REQMSK_ERR2;
                g_wakeup = temp[1] & REQMSK_WAKEUP;
        	break;
        	case DTC03P05_2:
        		g_errcode1_2 = temp[1] & REQMSK_ERR1;
                g_errcode2_2 = temp[1] & REQMSK_ERR2;
                g_wakeup2 = temp[1] & REQMSK_WAKEUP;
        	break;
		}  
        
        break;
        
    case I2C_COM_PCB:
        g_tpcb = (temp[1]<<8)|temp[0];
        break;
    //LCD200I2C 
    case LCD200_COM_IIN:
    	g_Ild = (temp[1]<<8)|temp[0];
    	break;
    //PZTDRF
    case PZTDRF_COM_VPZT:
    	g_vpzt = (temp[1]<<8)|temp[0]; 
    	break;
    case I2C_COM_TEST1:
    	g_test = temp[0];
    	break;
  }
}

float DTC03Master::ReturnTemp(unsigned int vact, bool type)
{
  float tact;
  if(type)
    tact = (float)(vact/129.8701) - 273.15;
  else
    tact = 1/(log((float)vact/RTHRatio)/(float)g_bconst+T0INV)-273.15;
  return tact;
}
unsigned int DTC03Master::ReturnVset(float tset, bool type)
{
  unsigned int vset;
  float temp;
  if(type)
    vset = (unsigned int)((tset+273.15)*129.8701);
  else
    vset = (unsigned int)RTHRatio*exp(-1*(float)g_bconst*(T0INV-1/(tset+273.15)));
  return vset;
}

float DTC03Master::ReturnCurrent(unsigned int current_mon)
{
	float current_f = (float)current_mon*0.003052;
	return(current_f);
}

unsigned int DTC03Master::ReturnCurrentDacout(float current_f)
{
	unsigned int currentDacout = 65535-(unsigned int)current_f*327.675;
	return(currentDacout); 
}
float DTC03Master::ReturnVpzt(unsigned int vpzt_mon, float G)
{
	float vpzt_out = (float)vpzt_mon*5/1023*G;
	return vpzt_out;
	
}
void DTC03Master::BackGroundPrint()
{
  lcd.SelectFont(SystemFont5x7);
  //DTC03
  lcd.GotoXY(T1_S_X, T1_S_Y);
  lcd.print(Text_T1_S);
  lcd.GotoXY(T1_A_X, T1_A_Y);
  lcd.print(Text_T1_A);
  lcd.GotoXY(A1_X,A1_Y);
  lcd.print(Text_A1);
  lcd.GotoXY(P1_X,P1_Y);
  lcd.print(Text_P1);  
  lcd.GotoXY(I1_X, I1_Y);
  lcd.print(Text_I1);
  //DTC03_2
  lcd.GotoXY(T2_S_X, T2_S_Y);
  lcd.print(Text_T2_S);
  lcd.GotoXY(T2_A_X, T2_A_Y);
  lcd.print(Text_T2_A);
  lcd.GotoXY(A2_X,A2_Y);
  lcd.print(Text_A2);
  lcd.GotoXY(P2_X,P2_Y);
  lcd.print(Text_P2);  
  lcd.GotoXY(I2_X, I2_Y);
  lcd.print(Text_I2);
  //LCD200 
  lcd.GotoXY(I_LD_X, I_LD_Y);
  lcd.print(Text_I_LD);
  //PZTDRF
  lcd.GotoXY(V_PZT_X, V_PZT_Y);
  lcd.print(Text_V_PZT);
}
void DTC03Master::PrintNormalAll()
{
	//DTC03
	PrintTset();
	PrintP();
	PrintKi();
	//DTC03_2
	PrintTset2();
	PrintP2();
	PrintKi2();
	//LCD200
	PrintLDcurrentSet();
	//No need to add print Itec and Vact here, checkstatus() will do this
}

void DTC03Master::PrintLDcurrentSet()
{
	lcd.SelectFont(SystemFont5x7);
 	lcd.GotoXY(I_LD_X2,I_LD_Y);
 	if(g_LDcurrent<10.0) lcd.print("  ");
 	else if(g_LDcurrent<100.0) lcd.print(" ");
 	lcd.print(g_LDcurrent,2);
}

void DTC03Master::PrintLDcurrentAct(float current)
{
	lcd.SelectFont(SystemFont5x7);
 	lcd.GotoXY(I_LD_ACT_X2,I_LD_Y);
 	if(!g_en_state) lcd.print("OFF   ");
 	else 
 	{
 		if(current<10.0) lcd.print("  ");
	 	else if(current<100.0) lcd.print(" ");
	 	lcd.print(current,2);
	 }		
}

void DTC03Master::PrintTset()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(T1_S_X2,T1_S_Y);
  if(g_tset<10.000)
    lcd.print(" ");
  
  lcd.print(g_tset,3);
}
void DTC03Master::PrintTset2()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(T2_S_X2,T2_S_Y);
  if(g_tset<10.000)
    lcd.print(" ");
  
  lcd.print(g_tset2,3);
}
void DTC03Master::PrintTact(float tact)
{

  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(T1_A_X2,T1_A_Y);
  if(g_errcode1) 
    {
      lcd.print("OPEN  ");
      return;
    }
    if(g_errcode2)
    {
      lcd.print("OT    ");
      return;
    }
   if(tact<10.000)
    lcd.print(" ");
    lcd.print(tact,3);
}
void DTC03Master::PrintTact2(float tact)
{

  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(T2_A_X2,T2_A_Y);
  if(g_errcode1_2) 
    {
      lcd.print("OPEN  ");
      return;
    }
    if(g_errcode2_2)
    {
      lcd.print("OT    ");
      return;
    }
   if(tact<10.000)
    lcd.print(" ");
    lcd.print(tact,3);
}
void DTC03Master::PrintItec(float itec)
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(A1_X2,A1_Y);
//  if ( abs(itec) <= 0.015 ) itec = 0;
  if(itec <0.00) lcd.print(itec,2); 

  else
   {
     lcd.print(" ");
     lcd.print(itec,2);
   } 
}
void DTC03Master::PrintItec2(float itec)
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(A2_X2,A2_Y);
//  if ( abs(itec) <= 0.015 ) itec = 0;
  if(itec <0.00) lcd.print(itec,2); 

  else
   {
     lcd.print(" ");
     lcd.print(itec,2);
   } 
}
void DTC03Master::PrintIlim()
{
  float currentlim;
  currentlim =ILIMSTART + ILIMSTEP *(float)(g_currentlim);
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(ILIM_COORD_X2,ILIM_COORD_Y);
//  lcd.print(" ");
  lcd.print(currentlim,2);
}
void DTC03Master::PrintP()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(P1_X2, P1_Y );
  if(g_p<10)
   lcd.print("  ");
  else if (g_p<100)
   lcd.print(" ");
  lcd.print(g_p);
}
void DTC03Master::PrintP2()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(P2_X2, P2_Y );
  if(g_p2<10)
   lcd.print("  ");
  else if (g_p2<100)
   lcd.print(" ");
  lcd.print(g_p2);
}
void DTC03Master::PrintKi()
{
  //unsigned int tconst;
  float tconst;
  lcd.SelectFont(SystemFont5x7);
  tconst = float(pgm_read_word_near(timeconst+g_kiindex))/100.0;
  lcd.GotoXY(I1_X2, I1_Y);
  if (g_kiindex < 3) {
  	if (g_kiindex==1) lcd.print(" OFF");
  	else lcd.print(tconst,2);
  }   
  else if (g_kiindex < 33){
   lcd.print(" ");
   lcd.print(tconst,1);
  }
  else{
  lcd.print("  ");
  lcd.print(tconst,0);
  }
}
void DTC03Master::PrintKi2()
{
  //unsigned int tconst;
  float tconst;
  lcd.SelectFont(SystemFont5x7);
  tconst = float(pgm_read_word_near(timeconst+g_kiindex2))/100.0;
  lcd.GotoXY(I2_X2, I2_Y);
  if (g_kiindex2 < 3) {
  	if (g_kiindex2==1) lcd.print(" OFF");
  	else lcd.print(tconst,2);
  }   
  else if (g_kiindex2 < 33){
   lcd.print(" ");
   lcd.print(tconst,1);
  }
  else{
  lcd.print("  ");
  lcd.print(tconst,0);
  }
}
void DTC03Master::PrintB()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(BCONST_COORD_X2, BCONST_COORD_Y);
  lcd.print(g_bconst); 
}
void DTC03Master::PrintModStatus() 
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(VMOD_COORD_X2, VMOD_COORD_Y);
  if(g_mod_status == 0) lcd.print("OFF");
  else lcd.print(" ON"); 
}
void DTC03Master::PrintEnable() //test i2c
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(Test1_COORD_X, Test1_COORD_Y);
  lcd.print(g_en_state); 
//  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(Test2_COORD_X, Test2_COORD_Y);
  lcd.print(g_test); 
}
void DTC03Master::PrintPZTvolt(float vpzt)
{
	lcd.SelectFont(SystemFont5x7);
  	lcd.GotoXY(V_PZT_X2, V_PZT_Y);
  	if(vpzt<10.0) lcd.print("  ");
  	else if(vpzt<100.0) lcd.print(" ");
  	lcd.print(vpzt,2); 
}
void DTC03Master::UpdateEnable()
{
 bool en_state;
 if(analogRead(ENSW)>500) en_state=1;
 else en_state=0;
 if(g_en_state != en_state)
 {
  g_en_state=en_state;
  I2CWriteData(I2C_COM_INIT,DTC03P05);
  I2CWriteData(I2C_COM_INIT,DTC03P05_2);
  I2CWriteData(LCD200_COM_LDEN,LCD200ADD);
 }
 
 //----------------------//
//  en_temp = analogRead(ENSW);
// if(en_temp>500) en_state=1;
// else en_state=0;
// if(g_en_state != en_state)
// {
//  g_en_state=en_state;
//  I2CWriteData(I2C_COM_INIT);
//  PrintEnable();
// }
}

void DTC03Master::PrintFactaryMode() //show error message to avoid entering Eng.Mode 
{
   lcd.ClearScreen(0); //clear the monitor
   lcd.SelectFont(Callibri11_bold);
   lcd.GotoXY(12,4);   
   lcd.print("FACTORY MODE");
   lcd.SelectFont(SystemFont5x7,WHITE);
   lcd.GotoXY(14,32);   
   lcd.print("Turn Off POWER");
   lcd.SelectFont(Wendy3x5);
   lcd.GotoXY(0,25);   
   lcd.print("PLEASE"); 
   lcd.GotoXY(36,42);   
   lcd.print("TO RE_START THE SYSTEM"); 
   delay(1000);
}

void DTC03Master::PrintEngBG()
{
  lcd.ClearScreen(0);
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(R1_COORD_X,R1_COORD_Y);
  lcd.print(Text_R1);
  lcd.GotoXY(R2_COORD_X,R2_COORD_Y);
  lcd.print(Text_R2);
  lcd.GotoXY(TPIDOFF_COORD_X,TPIDOFF_COORD_Y);
  lcd.print(Text_pidOS);
  lcd.GotoXY(VFBC_COORD_X,VFBC_COORD_Y);
  lcd.print(Text_Vfbc);
  lcd.GotoXY(VMOD_COOED_X,VMOD_COOED_Y);
  lcd.print(Text_Vmod);
  lcd.GotoXY(RMEAS_COORD_X,RMEAS_COORD_Y);
  lcd.print(Text_Rmeas);
  lcd.GotoXY(TOTP_COORD_X, TOTP_COORD_Y);
  lcd.print(Text_Totp);
  lcd.GotoXY(TPCB_COORD_X, TPCB_COORD_Y);
  lcd.print(Text_Tpcb);
}
void DTC03Master::PrintEngAll()
{
	PrintR1();
	PrintR2();
	PrintTpidoff();
	PrintVfbc();
	PrintVmod();
	PrintRmeas();
	PrintTotp();
	CheckStatus();
}
void DTC03Master::PrintR1() //g_cursorstate=10
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(R1_COORD_X2, R1_COORD_Y);
  lcd.print(g_r1);
  lcd.print("  ");//
}
void DTC03Master::PrintR2() //g_cursorstate=11
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(R2_COORD_X2, R2_COORD_Y);
  lcd.print(g_r2);
  lcd.print("  ");//
}
void DTC03Master::PrintTpidoff() //g_cursorstate=12
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(TPIDOFF_COORD_X2, TPIDOFF_COORD_Y);
  lcd.print(g_tpidoff);
  lcd.print("  ");//
}
void DTC03Master::PrintVfbc()//g_cursorstate=13
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(VFBC_COORD_X2, VFBC_COORD_Y);
  lcd.print(g_fbcbase);
}
void DTC03Master::PrintVmod()//g_cursorstate=14
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(VMOD_COOED_X2, VMOD_COOED_Y);
  lcd.print(g_vmodoffset);
  lcd.print("  ");//
}
void DTC03Master::PrintRmeas()//g_cursorstate=15
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(RMEAS_COORD_X2, RMEAS_COORD_Y);
  lcd.print(g_Rmeas);
}
void DTC03Master::PrintTotp()//g_cursorstate=16
{
  float Topt_set;
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(TOTP_COORD_X2, TOTP_COORD_Y);
  Topt_set = float(g_otp)/4.0-20.5;
  if (Topt_set < 99.5 ) lcd.print(" ");
  lcd.print(Topt_set,0);
}
void DTC03Master::PrintTpcb(float tpcb)
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(TPCB_COORD_X2, TPCB_COORD_Y);
  if (tpcb < 100.0 ) lcd.print(" ");
  lcd.print(tpcb,0);
}
void DTC03Master::CursorState()
{
  unsigned long t1, d1;
  unsigned int t_temp;
  if(analogRead(PUSHB)>HIGHLOWBOUNDRY)
  {
  	p_engmodeCounter=0;
  }           
  
  if(analogRead(PUSHB)<=HIGHLOWBOUNDRY) //change cursorstate when push encoder switch  
  {
  	t_temp=millis();
  	
  	if( abs(t_temp-p_cursorStateCounter[0])<ACCUMULATE_TH ) //ACCUMULATE_TH=50
	{
  		p_cursorStateCounter[1]=t_temp-p_cursorStateCounter[0];
  		p_cursorStateCounter[2]+=p_cursorStateCounter[1]; 		
	}
  	else p_cursorStateCounter[2]=0;
  	
  	if(!p_engModeFlag) //normal mode
  	{
  		if ( p_cursorStateCounter[2]>LONGPRESSTIME ) //long press case:
		{
	  		if (abs(t_temp-p_cursorStayTime) > CURSORSTATE_STAYTIME && p_tBlink_toggle )
			{	
				p_HoldCursortateFlag=0; 			
	  			if( g_cursorstate==0 || g_cursorstate==1 ) g_cursorstate=2;
		  		else g_cursorstate++;
		  		
		  		if( g_cursorstate>7 ) g_cursorstate=1;	  		
		  		ShowCursor(0);//the index is not important
		  		p_engmodeCounter++;
				if(p_engmodeCounter > ENGCOUNTER) 
		        {
		        	g_cursorstate = 8;
		        	p_engModeFlag=1;
		        	p_cursorStateCounter[2]=0; 
		         	PrintFactaryMode();						        
		        } 
		  		p_cursorStayTime=t_temp;
			} 		
		}
	  	else //short press case:
		{
			if( abs(t_temp-p_tcursorStateBounce)> DEBOUNCE_WAIT ) //DEBOUNCE_WAIT=ACCUMULATE_TH*4
			{
				if( g_cursorstate==0 ) g_cursorstate=1;
		  		if ( g_cursorstate==1 )
				{
		  			if(g_tsetstep <= 0.001) g_tsetstep = 1.0;
		            else g_tsetstep = g_tsetstep/10.0;
		            ShowCursor(0);
				}
		  		else //g_cursorstate=2~6
				{
					p_HoldCursortateFlag=1;
					p_timerResetFlag=1;
				}								
				p_tcursorStateBounce=t_temp;
			} 	  		
		}
	} 	
  	else //eng mode
	{		
		if( abs(t_temp-p_tcursorStateBounce)> DEBOUNCE_WAIT )
		{		    			
			g_cursorstate++;
		    if( p_cursorStateCounter[2]>LONGPRESSTIME ) 
		    {
			    g_cursorstate=1;
			    p_engModeFlag=0;
			    lcd.ClearScreen(0);
			    BackGroundPrint();
			    PrintNormalAll();			
		    }
		    if( g_cursorstate==9 ) 
		    {
			    PrintEngBG();
			    PrintEngAll();
			    g_cursorstate=10;
		    }
		    if( g_cursorstate>16 ) g_cursorstate=10;
	  	    ShowCursor(0);
	  	    p_tcursorStateBounce=t_temp;
		}		
	}
//	I2CWriteData(I2C_COM_TEST2);
	p_cursorStateCounter[0]	= t_temp;
  }
}

void DTC03Master::HoldCursortate() //put this method in loop
{
	unsigned int t_temp, timer;
	unsigned char oldCursorState;
	if(p_HoldCursortateFlag==1)
	{
		//start timer when enter this section-----
		t_temp=millis();
		if(p_timerResetFlag==1) 
		{
			p_timerResetFlag=0;
			p_holdCursorTimer=t_temp;
		}
		timer=t_temp-p_holdCursorTimer; //alway reset the timer when g_cursorstate=2~6 @ short press case
		
		if( timer < DEBOUNCE_WAIT*2 ) {} // do nothing if reset the timer			
		else //wait too long! g_imer > 2*DEBOUNCE_WAIT
		{
			p_HoldCursortateFlag=0;
			oldCursorState=g_cursorstate;
			g_cursorstate=0;
			ShowCursor(oldCursorState);//here oldCursorState from 2~6
			g_cursorstate=1;
		}		
	}
}
void DTC03Master::blinkTsetCursor()
{
	unsigned int t_temp;
	unsigned char x, y;
	if(g_cursorstate==1) 
	{
		x=T1_S_X2;
		y=T1_S_Y;
	}
	else if(g_cursorstate==4)
	{
		x=T2_S_X2;
		y=T2_S_Y;
	}
	if( p_blinkTsetCursorFlag==1 )
	{
		t_temp=millis();
		
		lcd.SelectFont(SystemFont5x7);
		if(g_tsetstep == 1.0) lcd.GotoXY(x+COLUMNPIXEL0507, y);
	    else if(g_tsetstep == 0.1) lcd.GotoXY(x+3*COLUMNPIXEL0507, y);
	    else if(g_tsetstep == 0.01) lcd.GotoXY(x+4*COLUMNPIXEL0507, y);
	    else lcd.GotoXY(x+5*COLUMNPIXEL0507, y);
	    
		if( abs(t_temp-p_tBlink)>BLINKDELAY )
		{		  	
			if(p_tBlink_toggle) {
				lcd.print(" ");
				p_tBlink_toggle=!p_tBlink_toggle;
			}
			else{
				PrintTset();
				p_tBlink_toggle=!p_tBlink_toggle;
			}
			p_tBlink=t_temp;		
		}
	}
			
}
void DTC03Master::PrintCursor(unsigned char x_new, unsigned char y_new, unsigned char x_old, unsigned char y_old)
{
	lcd.SelectFont(SystemFont5x7, WHITE);
	lcd.GotoXY(x_new-COLUMNPIXEL0507, y_new);
	lcd.print(" ");		    
	lcd.SelectFont(SystemFont5x7);		    
	lcd.GotoXY(x_old-COLUMNPIXEL0507, y_old);
	lcd.print(" ");
}
void DTC03Master::ShowCursor(unsigned char state_old)
{
		if( g_cursorstate!=1) p_blinkTsetCursorFlag=0;

		switch(g_cursorstate)
		{
		    case 0:
			    lcd.SelectFont(SystemFont5x7);
			    switch(state_old)
			    {
			    	case 2:
			    		lcd.GotoXY(P1_X-COLUMNPIXEL0507, P1_Y);
			    	break;
			    	case 3:
			    		lcd.GotoXY(I1_X-COLUMNPIXEL0507, I1_Y);
			    	break;
			    	case 5:
			    		lcd.GotoXY(P2_X-COLUMNPIXEL0507, P2_Y);
			    	break;
			    	case 6:
			    		lcd.GotoXY(I2_X-COLUMNPIXEL0507, I2_Y);		  		    
			    	break;	
			    	case 7:
			    		lcd.GotoXY(I_LD_X-COLUMNPIXEL0507, I_LD_Y);;		  		    
			    	break;
				}
				lcd.print(" ");		    
			    break;
		
		    case 1:
		    	PrintCursor(T1_S_X,T1_S_Y,I_LD_X,I_LD_Y);
		    	p_blinkTsetCursorFlag=1;
		    	break;
		    
		    case 2:
		    	PrintCursor(P1_X,P1_Y,T1_S_X,T1_S_Y);
		    	break;
		    
		    case 3:
		    	PrintCursor(I1_X,I1_Y,P1_X,P1_Y);
		    	break;
		    	
		    case 4:
		    	PrintCursor(T2_S_X,T2_S_Y,I1_X,I1_Y);
		    	p_blinkTsetCursorFlag=1;
		    	break;
		    	
		    case 5:
		    	PrintCursor(P2_X,P2_Y,T2_S_X,T2_S_Y);
		    	break;
		
		    case 6:
		    	PrintCursor(I2_X,I2_Y,P2_X,P2_Y);
		    	break;
		    	
		    case 7:
		    	PrintCursor(I_LD_X,I_LD_Y,I2_X,I2_Y);
		    	break;
		
		    case 10:
		    	PrintCursor(R1_COORD_X,R1_COORD_Y,TOTP_COORD_X,TOTP_COORD_Y);
		    	break;
		
		    case 11:
		    	PrintCursor(R2_COORD_X,R2_COORD_Y,R1_COORD_X,R1_COORD_Y);
		    	break;
		
		    case 12:
		    	PrintCursor(TPIDOFF_COORD_X,TPIDOFF_COORD_Y,R2_COORD_X,R2_COORD_Y);
		    	break;
		    
		    case 13:
		    	PrintCursor(VFBC_COORD_X,VFBC_COORD_Y,TPIDOFF_COORD_X,TPIDOFF_COORD_Y);
		    	break;
		
		    case 14:
		    	PrintCursor(VMOD_COOED_X,VMOD_COOED_Y,VFBC_COORD_X,VFBC_COORD_Y);
		    	break;
		
		    case 15:
		    	PrintCursor(RMEAS_COORD_X,RMEAS_COORD_Y,VMOD_COOED_X,VMOD_COOED_Y);
		    	break;
		
		    case 16:
		    	PrintCursor(TOTP_COORD_X,TOTP_COORD_Y,RMEAS_COORD_X,RMEAS_COORD_Y);
		    	break;
		}
  
}
void DTC03Master::UpdateParam() // Still need to add the upper and lower limit of each variable
{
  unsigned char ki, ls;
  unsigned long timer1, timer2;
  if(g_paramupdate)
  {
    g_paramupdate = 0;
    p_ee_changed = 1;
    switch(g_cursorstate)
    {
      case 0:
      break;
	//DTC03
      case 1:
        g_tset += g_tsetstep*g_counter;
        if(g_tset>100) g_tset=100;
        if(g_tset<7) g_tset=7;
        g_vset = ReturnVset(g_tset, g_sensortype);
        I2CWriteData(I2C_COM_VSET,DTC03P05);
        PrintTset();
        p_blinkTsetCursorFlag=0;
        p_ee_change_state=EEADD_VSET_UPPER;	
      break; 
      
//      case 2:
//      	g_currentlim += g_counter;
//        if(g_currentlim>51) g_currentlim=51;
//        if(g_currentlim<1) g_currentlim=1;        
//        I2CWriteData(I2C_COM_CTR);
//        PrintIlim();
//        p_ee_change_state=EEADD_currentlim; 
//      break;

      case 2:
      	g_p += g_counter;
        if(g_p>99) g_p=99;
        if(g_p<1) g_p=1;    
        I2CWriteData(I2C_COM_CTR,DTC03P05);
        PrintP();
        p_ee_change_state=EEADD_P;
      break;

      case 3:
      	g_kiindex += g_counter;
        if(g_kiindex>50) g_kiindex=50;
        if(g_kiindex<1) g_kiindex=1;      
        I2CWriteData(I2C_COM_KI,DTC03P05);
        PrintKi();
        p_ee_change_state=EEADD_KIINDEX;
      break;
      //DTC03_2
      case 4:
        g_tset2 += g_tsetstep*g_counter;
        if(g_tset2>100) g_tset2=100;
        if(g_tset2<7) g_tset2=7;
        g_vset2 = ReturnVset(g_tset2, g_sensortype);
        I2CWriteData(I2C_COM_VSET,DTC03P05_2);
        PrintTset2();
        p_blinkTsetCursorFlag=0;
        p_ee_change_state=EEADD_VSET_UPPER_2;	
      break; 
      
      case 5:
      	g_p2 += g_counter;
        if(g_p2>99) g_p2=99;
        if(g_p2<1) g_p2=1;    
        I2CWriteData(I2C_COM_CTR,DTC03P05_2);
        PrintP2();
        p_ee_change_state=EEADD_P_2;
      break;

      case 6:
      	g_kiindex2 += g_counter;
        if(g_kiindex2>50) g_kiindex2=50;
        if(g_kiindex2<1) g_kiindex2=1;      
        I2CWriteData(I2C_COM_KI,DTC03P05_2);
        PrintKi2();
        p_ee_change_state=EEADD_KIINDEX_2;
      break;
      //LCD200
      case 7:
      	g_LDcurrent += g_counter;
        if(g_LDcurrent>200) g_LDcurrent=200;
        if(g_LDcurrent<0) g_LDcurrent=0;
		g_dacout = ReturnCurrentDacout(g_LDcurrent);      
        I2CWriteData(LCD200_COM_IOUT,LCD200ADD);
        PrintLDcurrentSet();
        p_ee_change_state=EEADD_IOUT_UPPER;
      break;

//      case 5:
//      	g_bconst += g_counter;
//	    if(g_bconst>4499) g_bconst=4499;
//	    if(g_bconst<3501) g_bconst=3501;
//	    I2CWriteData(I2C_COM_INIT);
//  	    g_vset = ReturnVset(g_tset, g_sensortype);
//	    I2CWriteData(I2C_COM_VSET);//only send Vset, Bconst is not important for slave
//	    PrintB();
//	    p_ee_change_state=EEADD_BCONST_UPPER;
//	    break;
//
//      case 6:
//        g_mod_status = g_countersensor;
//        I2CWriteData(I2C_COM_INIT,DTC03P05);
//        PrintModStatus(); 
//        p_ee_change_state=EEADD_MODSTATUS;
//        break;

//      case 9:
//        PrintFactaryMode();
//        break;
        
      case 10:
      	g_r1 += g_counter;
	    if(g_r1>30) g_r1=30; // R1, 1~30 for 0.1~3.0 ohm set 
	    if(g_r1<1) g_r1=1;//
      	I2CWriteData(I2C_COM_R1R2,DTC03P05);
        PrintR1();
        p_ee_change_state=EEADD_R1;
        break;

      case 11:
      	g_r2 += g_counter;
        if(g_r2>30) g_r2=30;
        if(g_r2<1) g_r2=1;//R2, 1~30 for 1.0~3.0 ohm set 
        I2CWriteData(I2C_COM_R1R2,DTC03P05);
        PrintR2();
        p_ee_change_state=EEADD_R2;
        break;

      case 12:
      	g_tpidoff += g_counter;
        if(g_tpidoff>10) g_tpidoff=10; //Tpid offset, 0~10 for 1~10000 @1000 step
        if(g_tpidoff<1) g_tpidoff=1;
      
        I2CWriteData (I2C_COM_TPIDOFF,DTC03P05);
        PrintTpidoff();
        p_ee_change_state=EEADD_TPIDOFF;
        break;


      case 13:
      	g_fbcbase +=(g_counter*100);
        if(g_fbcbase>44900) g_fbcbase=44900;
        if(g_fbcbase<16100) g_fbcbase=16100;
        I2CWriteData(I2C_COM_FBC,DTC03P05);
        PrintVfbc();
        p_ee_change_state=EEADD_FBC_UPPER;
        break;

      case 14:
      	g_vmodoffset +=g_counter;
        if(g_vmodoffset>33199) g_vmodoffset=33199;
        if(g_vmodoffset<32199) g_vmodoffset=32199;
        I2CWriteData(I2C_COM_VMOD,DTC03P05);
        PrintVmod();
        p_ee_change_state=EEADD_MODOFF_UPPER;
        break;

      case 15:
      	g_Rmeas += (g_counter*100);
      	if(g_Rmeas>45000) g_Rmeas=45000;
        if(g_Rmeas<20000) g_Rmeas=20000;
        I2CWriteData(I2C_COM_RMEAS,DTC03P05);
        PrintRmeas();
        p_ee_change_state=EEADD_RMEAS_UPPER;
        break;

      case 16:
      	g_otp += (g_counter*4);
		if (g_otp < 281) g_otp = 281; // 50C
		if (g_otp > 561) g_otp = 561; //120C
		I2CWriteData(I2C_COM_OTP,DTC03P05);
        PrintTotp();  
        p_ee_change_state=EEADD_TOTP_UPPER;
    }
  }
}
void DTC03Master::Encoder()
{
  unsigned char encoded, sum, dt;
  unsigned long tenc;
  bool MSB, LSB;
  tenc= millis();
  dt = tenc - g_tenc;
  if(dt < DEBOUNCETIME) return;
  if(dt > COUNTRESETTIME) g_icount =0;
  MSB = digitalRead(ENC_B);
  LSB = digitalRead(ENC_A);
  encoded = (MSB << 1)|LSB;
  sum = (g_lastencoded << 2)| encoded;
  if(g_icount%4==0)
  {
    g_paramupdate=1;// 20161031 when ineterrupt=4times g_paramupdate=1
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) 
    {

      g_counter = -1;
      g_countersensor =1;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) 
    {

      g_counter = 1;
      g_countersensor =0;
    }
  }
  g_lastencoded = encoded;
  g_tenc = tenc;
  g_icount++;
}








