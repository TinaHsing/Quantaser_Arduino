/* modification for disable sensortype purpose : data 2016/11/13
	1. in I2CReadData(), set g_sensortype to 0;
	2. add g_mod_status variable in DTC03Master_P02.h
	3. add VMOD x and y GLCD coordinate in DTC03Master_P02.h
	4. add PrintModStatus

*/
#include <DTC03Master_P02.h>
DTC03Master::DTC03Master()
{
}
void DTC03Master::SetPinMode()
{
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(PUSHB, INPUT);
  pinMode(ENSW, INPUT);
}
void DTC03Master::ParamInit()
{
  digitalWrite(ENC_A, HIGH);
  digitalWrite(ENC_B, HIGH);
  
  Wire.begin();
  lcd.Init();
  g_iarrayindex = 0;
  g_varrayindex = 0;
  g_itecsum = 0;
  g_vactsum = 0;
  g_paramupdate = 0;
  g_tsetstep = 1.00;
  g_en_state = 0;
  g_countersensor = 0;
  g_ttstart = 18.00;
  g_testgo =0;
  g_cursorstate=0;
  p_cursorStateCounter[0]=0;
  p_cursorStateCounter[1]=0;
  p_cursorStateCounter[2]=0;
  p_cursorStayTime=0;
  p_tBlink=0;
  p_tBlink_toggle=0;
  p_engModeFlag=0;
  p_blinkTsetCursorFlag=0;
  p_loopindex=0;
}
void DTC03Master::WelcomeScreen()
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(0,0);
  lcd.print("DTC03 Ver.2.01");
  lcd.GotoXY(0,ROWPIXEL0507*1);
  lcd.print("Initializing");
//  for (byte i=9; i>0; i--)
//  {
//    lcd.GotoXY(COLUMNPIXEL0507*(12+1),ROWPIXEL0507*1);
//    lcd.print(i);
//    delay(1000);
//  }
  lcd.ClearScreen(0);//0~255 means ratio of black  
}
void DTC03Master::I2CWriteData(unsigned char com)
{
  unsigned char temp[2];
  switch(com)
  {
    case I2C_COM_INIT:
        temp[0]= g_bconst - BCONSTOFFSET;
        temp[1]= (g_bconst - BCONSTOFFSET) >> 8;
        if(g_en_state) temp[1] |= REQMSK_ENSTATE;
//    if(g_sensortype) temp[1]|= REQMSK_SENSTYPE; //20161113 mark
	    if(g_mod_status) temp[1]|= REQMSK_SENSTYPE; //20161113   
        break;

    case I2C_COM_CTR:
        temp[0]= g_currentlim;
        temp[1]= g_p;
        break;

    case I2C_COM_VSET:
        temp[0]=g_vset;
        temp[1]=g_vset>>8;
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
        temp[0] = g_fbcbase;
        temp[1] = g_fbcbase>>8;
        break;

    case I2C_COM_VMOD:
        temp[0] = g_vmodoffset;
        temp[1] = g_vmodoffset >>8;
        break;
    
    case I2C_COM_KI:
        temp[0]=pgm_read_word_near(kilstable+g_kiindex*2);
        temp[1]=pgm_read_word_near(kilstable+g_kiindex*2+1);
        break;
    
    case I2C_COM_RMEAS:
    	temp[0]=p_Rmeas;
    	temp[1]=p_Rmeas>>8;
    	break;
    	
    case I2C_COM_OTP:
    		temp[0] = g_otp;
    		temp[1] = g_otp>>8;
    	break;
    
    case I2C_COM_TEST1:
        temp[0]=p_temp;
        temp[1]=p_temp>>8;
        break;
    
    case I2C_COM_TEST2:
        temp[0]=g_cursorstate;
        temp[1]=p_tBlink_toggle;
        break;

  }
  Wire.beginTransmission(DTC03P05);//
  Wire.write(com);//
  Wire.write(temp, 2);//
  Wire.endTransmission();//
  delayMicroseconds(I2CSENDDELAY);//
}
void DTC03Master::I2CReadData(unsigned char com)
{
  unsigned char temp[2], b_upper, b_lower;
  unsigned int itectemp;
  bool itecsign;
  
  Wire.beginTransmission(DTC03P05);
  Wire.write(com);
  Wire.endTransmission();
  delayMicroseconds(I2CREADDELAY);
  Wire.requestFrom(DTC03P05,2);
  while(Wire.available()==2)
  {
    temp[0] = Wire.read();
    temp[1] = Wire.read();
  }
  switch(com)
  {
    case I2C_COM_VACT:
        g_vact =(temp[1] <<8) | temp[0];
        break;

    case I2C_COM_ITEC_ER:
    	
        itectemp = ((temp[1] & REQMSK_ITECU) << 8)| temp[0];
        itecsign = temp[1] & REQMSK_ITECSIGN;
        g_errcode1 = temp[1] & REQMSK_ERR1;
        g_errcode2 = temp[1] & REQMSK_ERR2;
        if(itecsign) g_itec = (-1)*(int)itectemp;
        else g_itec = (int)itectemp;
        break;
        
    case I2C_COM_PCB:
        g_tpcb = (temp[1]<<8)|temp[0];
        break;
  }
}
void DTC03SMaster::CheckStatus()
{
		float tact, itec_f, tpcb_f;
				if (p_loopindex%3==0) {
					I2CReadData(I2C_COM_ITEC_ER);
		            itec_f = float(g_itec)*CURRENTRatio;
		            PrintItec(itec_f);
				}				
				if (p_loopindex%3==1) {
					I2CReadData(I2C_COM_PCB);
		            tpcb_f = float(g_tpcb)/4.0-20.5;
		            PrintTpcb(tpcb_f);
				}
				if (p_loopindex%3==2) {
					I2CReadData(I2C_COM_VACT);
	  	    		tact = ReturnTemp(g_vact,0);
	  	    		PrintTact(tact);
				}		
	    p_loopindex++;		       
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
void DTC03Master::BackGroundPrint()
{
  lcd.SelectFont(Iain5x7);
  lcd.GotoXY(TSET_COORD_X,TSET_COORD_Y);
  lcd.print(Text_SET);
  lcd.GotoXY(TACT_COORD_X,TACT_COORD_Y);    
  lcd.print(Text_ACT);
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(ITEC_COORD_X, ITEC_COORD_Y);
  lcd.print(Text_ITEC);
  lcd.GotoXY(ILIM_COORD_X, ILIM_COORD_Y);
  lcd.print(Text_ILIM);
  lcd.GotoXY(P_COORD_X,P_COORD_Y);
  lcd.print(Text_P);
  lcd.GotoXY(I_COORD_X,I_COORD_Y);
  lcd.print(Text_I);  
  lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
  lcd.print(Text_B);
  lcd.GotoXY(VMOD_COORD_X, VMOD_COORD_Y);
  lcd.print(Text_MS);
}
void DTC03Master::PrintNormalAll()
{
	PrintTset();
	PrintIlim();
	PrintP();
	PrintKi();
	PrintB();
	PrintModStatus();
	//No need to add print Itec and Vact here, checkstatus() will do this
}

void DTC03Master::PrintTset()
{
  lcd.SelectFont(fixed_bold10x15);
  lcd.GotoXY(TSET_COORD_X2,TSET_COORD_Y);
  if(g_tset<10.000)
    lcd.print("  ");
  else if(g_tset<100.000)
    lcd.print(" ");
  
  lcd.print(g_tset,3);
}
void DTC03Master::PrintTact(float tact)
{

  lcd.SelectFont(Arial_bold_14);
  lcd.GotoXY(TACT_COORD_X2,TACT_COORD_Y);
  if(g_errcode1) 
    {
      lcd.print("_error1");
      return;
    }
    if(g_errcode2)
    {
      lcd.print("_error2");
      return;
    }

  if(tact<=0.000)
   {
    if(abs(tact)<10.000)
    lcd.print(" ");
    lcd.print(tact,3);
   }
  else
  {
   if(tact<10.000)
    lcd.print("   ");
   else if (tact<100.000)
    lcd.print("  ");
   else
    lcd.print(" ");

    lcd.print(tact,3);
  }
  lcd.print(" ");
}
void DTC03Master::PrintItec(float itec)
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(ITEC_COORD_X2,ITEC_COORD_Y);
  if ( abs(itec) <= 0.015 ) itec = 0;
  if(itec <0.00) lcd.print(itec,2); 

  else
   {
     lcd.print(" ");
     lcd.print(itec,2);
     //lcd.print(itec,3);
   } 
  //lcd.print(" "); 
}
void DTC03SMaster::PrintTpcb(float tpcb)
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(TOTP_COORD_X2, TOTP_COORD_Y);
  if (tpcb < 100.0 ) lcd.print(" ");
  lcd.print(tpcb,0);
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
  lcd.GotoXY(P_COORD_X2, P_COORD_Y );
  if(g_p<10)
   lcd.print("  ");
  else if (g_p<100)
   lcd.print(" ");
  lcd.print(g_p);
}
void DTC03Master::PrintKi()
{
  //unsigned int tconst;
  float tconst;
  lcd.SelectFont(SystemFont5x7);
  tconst = float(pgm_read_word_near(timeconst+g_kiindex))/100.0;
  lcd.GotoXY(I_COORD_X2, I_COORD_Y);
  if (g_kiindex==1) lcd.print(tconst);
  else if (g_kiindex<32)
  {
   lcd.print(" ");
   if (g_kiindex==0) lcd.print("OFF");
   else lcd.print(tconst,1);
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


void DTC03Master::UpdateParam() // Still need to add the upper and lower limit of each variable
{
  unsigned char ki, ls;
  unsigned long timer1, timer2;
  if(g_paramupdate)
  {
    g_paramupdate = 0;
    switch(g_cursorstate)
    {
      case 0:
      break;

      case 1:
        g_tset += g_tsetstep*g_counter;
        if(g_tset>100) g_tset=100;
        if(g_tset<7) g_tset=7;
        g_vset = ReturnVset(g_tset, g_sensortype);
        I2CWriteData(I2C_COM_VSET);
        PrintTset();
        p_blinkTsetCursorFlag=0;
      break; 
      
      case 2:
      	g_currentlim += g_counter;
        if(g_currentlim>50) g_currentlim=50;
        if(g_currentlim<1) g_currentlim=1;        
        I2CWriteData(I2C_COM_CTR);
        PrintIlim();
      break;

      case 3:
      	g_p += g_counter;
        if(g_p>99) g_p=99;
        if(g_p<1) g_p=1;    
        I2CWriteData(I2C_COM_CTR);
        PrintP();
      break;

      case 4:
      	g_kiindex += g_counter;
        if(g_kiindex>50) g_kiindex=50;
        if(g_kiindex<1) g_kiindex=1;      
        I2CWriteData(I2C_COM_KI);
        PrintKi();
      break;

      case 5:
      	g_bconst += g_counter;
	    if(g_bconst>4499) g_bconst=4499;
	    if(g_bconst<3501) g_bconst=3501;
  	    g_vset = ReturnVset(g_tset, g_sensortype);
	    I2CWriteData(I2C_COM_VSET);//only send Vset, Bconst is not important for slave
	    PrintB();
	    break;

      case 6:
        g_mod_status = g_countersensor;
        I2CWriteData(I2C_COM_INIT);
        PrintModStatus(); 
        break;

      case 9:
        PrintFactaryMode();
        break;
        
      case 10:
      	g_r1 += g_counter;
	    if(g_r1>30) g_r1=30; // R1, 1~30 for 0.1~3.0 ohm set 
	    if(g_r1<1) g_r1=1;//
      	I2CWriteData(I2C_COM_R1R2);
        PrintR1();
        break;

      case 11:
      	g_r2 += g_counter;
        if(g_r2>30) g_r2=30;
        if(g_r2<1) g_r2=1;//R2, 1~30 for 1.0~3.0 ohm set 
        I2CWriteData(I2C_COM_R1R2);
        PrintR2();
        break;

      case 12:
      	g_tpidoff += g_counter;
        if(g_tpidoff>10) g_tpidoff=10; //Tpid offset, 0~10 for 1~10000 @1000 step
        if(g_tpidoff<1) g_tpidoff=1;
      
        I2CWriteData (I2C_COM_TPIDOFF);
        PrintTpidoff();
        break;


      case 13:
      	g_fbcbase +=(g_counter*100);
        if(g_fbcbase>44900) g_fbcbase=44900;
        if(g_fbcbase<16100) g_fbcbase=16100;
        I2CWriteData(I2C_COM_FBC);
        PrintVfbc();
        break;

      case 14:
      	g_vmodoffset +=g_counter;
        if(g_vmodoffset>33199) g_vmodoffset=33199;
        if(g_vmodoffset<32199) g_vmodoffset=32199;
        I2CWriteData(I2C_COM_VMOD);
        PrintVmod();
        break;

      case 15:
      	p_Rmeas += (g_counter*100);
      	if(g_fbcbase>45000) g_fbcbase=45000;
        if(g_fbcbase<20000) g_fbcbase=20000;
        I2CWriteData(I2C_COM_RMEAS);
        PrintRmeas();
        break;

      case 16:
      	g_otp += (g_counter*4);
		if (g_otp < 281) g_otp = 281; // 50C
		if (g_otp > 561) g_otp = 561; //120C
		I2CWriteData(I2C_COM_OTP);
        PrintTotp();  
    }
  }
}

void DTC03Master::UpdateEnable()
{
 bool en_state;
 if(analogRead(ENSW)>500) en_state=1;
 else en_state=0;
 if(g_en_state != en_state)
 {
  g_en_state=en_state;
  I2CWriteData(I2C_COM_INIT);
 }
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
	PrintTpcb();
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
  lcd.print(g_ttstart);
  lcd.print("  ");//
}
void DTC03Master::PrintTotp()//g_cursorstate=16
{
  lcd.SelectFont(SystemFont5x7);
  lcd.GotoXY(TOTP_COORD_X2, TOTP_COORD_Y);
  lcd.print(g_testgo);
}

void DTC03Master::CursorState()
{
  unsigned long t1, d1;
  unsigned int t_temp;
  unsigned char oldCursorState;
  if(analogRead(PUSHB)>HIGHLOWBOUNDRY)
  {
  	p_engmodeCounter=0;
  }           
  
  if(analogRead(PUSHB)<=HIGHLOWBOUNDRY) //change cursorstate when push encoder switch  
  {
  	t_temp=millis();
  	
  	if( abs(t_temp-p_cursorStateCounter[0])<50 ) 
	{
  		p_cursorStateCounter[1]=t_temp-p_cursorStateCounter[0];
  		p_cursorStateCounter[2]+=p_cursorStateCounter[1]; 		
	}
  	else p_cursorStateCounter[2]=0;
  	
  	if(!p_engModeFlag) //normal mode
  	{
  		if ( p_cursorStateCounter[2]>LONGPRESSTIME ) //long press case:
		{
	  		if (abs(t_temp-p_cursorStayTime) > CURSORSTATE_STAYTIME && p_tBlink_toggle ){
	  			if( g_cursorstate==0 || g_cursorstate==1 ) g_cursorstate=2;
		  		else g_cursorstate++;
		  		if( g_cursorstate>6 ) g_cursorstate=2;	  		
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
			if( abs(t_temp-p_tcursorStateBounce)> 200 )
			{
				if( g_cursorstate==0 ) g_cursorstate=1;
		  		if ( g_cursorstate==1 )
				{
		  			if(g_tsetstep <= 0.001) g_tsetstep = 1.0;
		            else g_tsetstep = g_tsetstep/10.0;
				}
		  		else //g_cursorstate=2~6
				{
		  			oldCursorState=g_cursorstate;
		  			g_cursorstate=0;
				}				
				ShowCursor(oldCursorState);//here oldCursorState from 2~6, g_cursorstate is 0 or 1
				p_tcursorStateBounce=t_temp;
			} 	  		
		}
	}
  	
  	else //eng mode
	{		
		if( abs(t_temp-p_tcursorStateBounce)> 200 )
		{		    			
			g_cursorstate++;
        	I2CWriteData(I2C_COM_TEST2);
		    if( p_cursorStateCounter[2]>LONGPRESSTIME ) 
		    {
			    g_cursorstate=0;
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
  	I2CWriteData(I2C_COM_TEST2);
    p_cursorStateCounter[0]=t_temp; 
  }
}
void DTC03Master::blinkTsetCursor()
{
	unsigned int t_temp;
	if( p_blinkTsetCursorFlag==1 )
	{
		t_temp=millis();
		
		lcd.SelectFont(fixed_bold10x15);
		if(g_tsetstep == 1.0) lcd.GotoXY(TSET_COORD_X2+2*COLUMNPIXEL1015, TSET_COORD_Y);
	    else if(g_tsetstep == 0.1) lcd.GotoXY(TSET_COORD_X2+4*COLUMNPIXEL1015, TSET_COORD_Y);
	    else if(g_tsetstep == 0.01) lcd.GotoXY(TSET_COORD_X2+5*COLUMNPIXEL1015, TSET_COORD_Y);
	    else lcd.GotoXY(TSET_COORD_X2+6*COLUMNPIXEL1015, TSET_COORD_Y);
	    
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
		    		lcd.GotoXY(ILIM_COORD_X-COLUMNPIXEL0507, ILIM_COORD_Y);
		    	break;
		    	case 3:
		    		lcd.GotoXY(P_COORD_X-COLUMNPIXEL0507, P_COORD_Y);
		    	break;
		    	case 4:
		    		lcd.GotoXY(I_COORD_X-COLUMNPIXEL0507, I_COORD_Y);
		    	break;
		    	case 5:
		    		lcd.GotoXY(BCONST_COORD_X-COLUMNPIXEL0507, BCONST_COORD_Y);
		    	break;
		    	case 6:
		    		lcd.GotoXY(VMOD_COORD_X-COLUMNPIXEL0507, VMOD_COORD_Y);		  		    
		    	break;	
			}
			lcd.print(" ");		    
		    break;
		
		    case 1:
		    p_blinkTsetCursorFlag=1;
		    break;
		
		    case 2:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(ILIM_COORD_X-COLUMNPIXEL0507, ILIM_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(VMOD_COORD_X-COLUMNPIXEL0507, VMOD_COORD_Y);//
		    lcd.print(" ");
		    break;
		    
		    case 3:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(P_COORD_X-COLUMNPIXEL0507, P_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(ILIM_COORD_X-COLUMNPIXEL0507, ILIM_COORD_Y);
		    lcd.print(" ");
		    break;
		    
		    case 4:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(I_COORD_X-COLUMNPIXEL0507, I_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(P_COORD_X-COLUMNPIXEL0507, P_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 5:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(BCONST_COORD_X-COLUMNPIXEL0507, BCONST_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(I_COORD_X-COLUMNPIXEL0507, I_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 6:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(VMOD_COORD_X-COLUMNPIXEL0507, VMOD_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(BCONST_COORD_X-COLUMNPIXEL0507, BCONST_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 10:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(R1_COORD_X-COLUMNPIXEL0507, R1_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(TOTP_COORD_X-COLUMNPIXEL0507, TOTP_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 11:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(R2_COORD_X-COLUMNPIXEL0507, R2_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(R1_COORD_X-COLUMNPIXEL0507, R1_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 12:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(TPIDOFF_COORD_X-COLUMNPIXEL0507, TPIDOFF_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(R2_COORD_X-COLUMNPIXEL0507, R2_COORD_Y);
		    lcd.print(" ");
		    break;
		    
		    case 13:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(VFBC_COORD_X-COLUMNPIXEL0507, VFBC_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(TPIDOFF_COORD_X-COLUMNPIXEL0507, TPIDOFF_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 14:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(VMOD_COOED_X-COLUMNPIXEL0507, VMOD_COOED_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(VFBC_COORD_X-COLUMNPIXEL0507, VFBC_COORD_Y);
		    lcd.print(" ");
		    break;
		
		    case 15:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(RMEAS_COORD_X-COLUMNPIXEL0507, RMEAS_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(VMOD_COOED_X-COLUMNPIXEL0507, VMOD_COOED_Y);
		    lcd.print(" ");
		    break;
		
		    case 16:
		    lcd.SelectFont(SystemFont5x7, WHITE);
		    lcd.GotoXY(TOTP_COORD_X-COLUMNPIXEL0507, TOTP_COORD_Y);
		    lcd.print(" ");
		    lcd.SelectFont(SystemFont5x7);
		    lcd.GotoXY(RMEAS_COORD_X-COLUMNPIXEL0507, RMEAS_COORD_Y);
		    lcd.print(" ");
		    break;
		}
  
}









