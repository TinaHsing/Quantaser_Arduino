#include <DTC03S_Master.h>
DTC03SMaster::DTC03SMaster()
{

}
void DTC03SMaster::SetPinMode()
{
	pinMode(ENC_A, INPUT);
	pinMode(PUSHB, INPUT);
	pinMode(SCANB, INPUT);
	pinMode(ENSW, INPUT);
}
void DTC03SMaster::ParamInit()
{
	digitalWrite(ENC_A, HIGH);
	lcd.Init();
	Wire.begin();
	g_en_state =0;
	g_scan =0;
	g_timer=0;
	g_tfine = 0;
	g_tenc[0] =0;
	g_tenc[1] =0;
	g_tenc[2] =0;
	p_en[0] = p_en[1] = 0;
	p_scan[0] = p_scan[1] = 0;
	p_tnow_flag[0] = 0;
	p_tnow_flag[1] = 0;
	g_oldcursorstate = 0;
	p_curstatus0flag = 0;
	p_rateflag = 1;
	p_trate = 0;
	p_loopcount = 0;
}
void DTC03SMaster::WelcomeScreen()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(0,0);
	lcd.print("DTC03S Ver.1.01");
	lcd.GotoXY(0,ROWPIXEL0507*1);
	lcd.print("Initializing...");
	delay(1000);
}
void DTC03SMaster::ReadEEPROM()
{
	unsigned char noeedummy, temp_upper, temp_lower;
	noeedummy =EEPROM.read(EEADD_DUMMY);
	if(noeedummy == NOEE_DUMMY)
	{
		temp_upper = EEPROM.read(EEADD_VSTART_UPPER);
		temp_lower = EEPROM.read(EEADD_VSTART_LOWER);
		g_vstart = temp_upper <<8 | temp_lower;

		temp_upper = EEPROM.read(EEADD_VEND_UPPER);
		temp_lower = EEPROM.read(EEADD_VEND_LOWER);
		g_vend = temp_upper <<8 | temp_lower;

		g_rateindex = EEPROM.read(EEADD_RATE_INDEX);
		
		temp_upper = EEPROM.read(EEADD_FBC_UPPER);
		temp_lower = EEPROM.read(EEADD_FBC_LOWER);
		g_fbcbase = temp_upper <<8 | temp_lower;

	}
	else
	{
		EEPROM.write(EEADD_DUMMY, NOEE_DUMMY);
		EEPROM.write(EEADD_VSTART_UPPER, NOEE_VSTART>>8);
		EEPROM.write(EEADD_VSTART_LOWER, NOEE_VSTART);
		EEPROM.write(EEADD_VEND_UPPER, NOEE_VEND>>8);
		EEPROM.write(EEADD_VEND_LOWER, NOEE_DUMMY);
		EEPROM.write(EEADD_RATE_INDEX, NOEE_RATE);
		EEPROM.write(EEADD_DUMMY, NOEE_DUMMY);
		EEPROM.write(EEADD_FBC_UPPER, NOEE_FBC>>8);
		EEPROM.write(EEADD_FBC_LOWER, NOEE_FBC);

		g_vstart = NOEE_VSTART;
		g_tstart = ReturnTemp(g_vstart, 0);
		g_vend = NOEE_VEND;
		g_tend = ReturnTemp(g_vend, 0);
		g_rateindex = NOEE_RATE;
		g_fbcbase = NOEE_FBC;

	}
	g_vset = g_vstart;
	g_tnow = g_tstart;
	g_trate = pgm_read_word_near(RateTable+g_rateindex);
	I2CWriteData(I2C_COM_VSET); // Set Vset to Slave in the begining
	if(g_tend >= g_tstart) g_heater =1;
	else g_heater =0;

}
void DTC03SMaster::I2CWriteData(unsigned char com)
{
	unsigned char temp[2];
	temp[0]=0;
    temp[1]=0;
  	switch(com)
  	{
    	case I2C_COM_INIT:
    	temp[0] = BVALUE;
    	temp[1] = (BVALUE >>8);
			if(g_en_state) temp[1] |= REQMSK_ENSTATE;
		break;

    	case I2C_COM_CTR:
    		temp[0]= 0;
    		temp[1]= PGAIN;
    	break;

    	case I2C_COM_VSET:
    		temp[0]=g_vset;
    		temp[1]=g_vset>>8;
    	break;

    	case I2C_COM_VBEH:
    		temp[0] = DEFAULTR1;
    		temp[1] = DEFAULTR2;
    	break;

    	case I2C_COM_VBEC:
    		temp[0] = DEFAULTTPIDOFF;
    		temp[1] = 0;
    	break;

    	case I2C_COM_FBC:
    		temp[0] = g_fbcbase;
    		temp[1] = g_fbcbase>>8;
    	break;
   
    	case I2C_COM_KI:
    		temp[0]=LS;
    		temp[1]=KI;
    	break;
    	
    	case I2C_COM_TEST:
    		temp[0]=p_trate;
    		temp[1]=p_trate>>8;
    	break;

  }
  Wire.beginTransmission(DTC03P05);//20161031 add
  Wire.write(com);//
  Wire.write(temp, 2);//
  Wire.endTransmission();
}
void DTC03SMaster::I2CReadVact()
{
	unsigned char temp[2];
	Wire.beginTransmission(DTC03P05);
	Wire.write(I2C_COM_VACT);
	Wire.endTransmission();
  	Wire.requestFrom(DTC03P05,2);
  	while(Wire.available()==2)
  	{
    	temp[0] = Wire.read();
    	temp[1] = Wire.read();
  	}
    g_vact =(temp[1] <<8) | temp[0];
}
float DTC03SMaster::ReturnTemp(unsigned int vact, bool type)
{
  float tact;
  if(type)
    tact = (float)(vact/129.8701) - 273.15;
  else
    tact = 1/(log((float)vact/RTHRatio)/BVALUE+T0INV)-273.15;
  return tact;
}
void DTC03SMaster::PrintBG()
{
	lcd.ClearScreen(0);
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(TSTART_COORD_X, TSTART_COORD_Y);
	lcd.print("Tstart:");
	lcd.GotoXY(TEND_COORD_X, TEND_COORD_Y);
	lcd.print("Tstop :");
	lcd.GotoXY(RATE_COORD_X, RATE_COORD_Y);
	lcd.print("RATE:");
	lcd.GotoXY(EN_COORD_X, EN_COORD_Y);
	lcd.print("CTRL:");
}
void DTC03SMaster::Printloopt(unsigned long tp)
{
	float loopt_avg;
	p_tlp[p_loopcount] = tp;
	p_loopcount++;
	
	if (p_loopcount == 5) {
		p_loopcount = 0;
		lcd.SelectFont(SystemFont5x7);
		lcd.GotoXY(LOOPT_X, LOOPT_Y);
		loopt_avg = float(( (p_tlp[1]-p_tlp[0])+(p_tlp[2]-p_tlp[1])+
		(p_tlp[3]-p_tlp[2])+(p_tlp[4]-p_tlp[3]) ))/4.0;
		if ( loopt_avg<10 ) lcd.print(' ');
		lcd.print(loopt_avg);
	}
	
}
void DTC03SMaster::PrintTstart()
{
	lcd.SelectFont(SystemFont5x7);	
	lcd.GotoXY(TSTART_COORD_X2, TSTART_COORD_Y);
	if(g_tstart<10.00) lcd.print(" ");
	lcd.print(g_tstart,2);
}
void DTC03SMaster::PrintTend()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(TEND_COORD_X2, TEND_COORD_Y);
	if(g_tend < 10.00) lcd.print(" ");
	lcd.print(g_tend,2); 
}
void DTC03SMaster::PrintTact(float tact)
{
	lcd.SelectFont(Arial_bold_14);
	lcd.GotoXY(TACT_COORD_X, TACT_COORD_Y);
	if(tact< 10.00) lcd.print(" ");
	lcd.print(tact, 2); 

}
void DTC03SMaster::PrintRate()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(RATE_COORD_X2, RATE_COORD_Y);
//	if (g_trate < 10) lcd.print(" ");
//	else if (g_trate < 100) lcd.print(" ");
	lcd.print(float(g_trate)/10,1);
//	lcd.print(p_rate,3);
}
void DTC03SMaster::PrintScan()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(SCAN_COORD_X, SCAN_COORD_Y);
	if(g_scan) lcd.print("SCAN");
	else lcd.print("STOP");
}
void DTC03SMaster::PrintEnable()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(EN_COORD_X2, SCAN_COORD_Y);
	if(g_en_state==0) lcd.print("OFF");
	else lcd.print(" ON");	
}
void DTC03SMaster::PrintTnow()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(TFINE_COORD_X, TFINE_COORD_Y);
	if (p_tnow_flag[1]) lcd.print(g_tnow+g_tfine);
	else lcd.print("     ");
    
	
}
void DTC03SMaster::checkTnowStatus()
{
	//
	if (p_en[0]) {
		if (p_scan[1] < p_scan[0]) p_tnow_flag[1] = 1;
	}
	else {
		p_tnow_flag[1] = 0;
		g_tfine = 0;
	}
	if (p_scan[1]) p_tnow_flag[1] = 0;
	//
	if (p_tnow_flag[0] != p_tnow_flag[1]) {
		if (p_tnow_flag[1]==1) {
		    g_cursorstate = 3;
		    p_curstatus0flag = 1;
	    }
		else {
			g_cursorstate = 0;
			g_oldcursorstate = 0;
		}
		PrintTnow();
	}
	p_tnow_flag[0] = p_tnow_flag[1];
	p_en[0] = p_en[1];
	p_scan[0] = p_scan[1];
}
void DTC03SMaster::PrintFbcbase()
{
	lcd.SelectFont(SystemFont5x7);
	lcd.GotoXY(TSTART_COORD_X, TSTART_COORD_Y);
	lcd.print(g_fbcbase);
}
unsigned int DTC03SMaster::ReturnVset(float tset, bool type)
{
  unsigned int vset;
  float temp;
  if(type)
    vset = (unsigned int)((tset+273.15)*129.8701);
  else
    vset = (unsigned int)RTHRatio*exp(-1*(float)BVALUE*(T0INV-1/(tset+273.15)));
  return vset;
}
void DTC03SMaster::CalculateRate()
{
	unsigned int t_temp;
	
	t_temp = millis(); 
	if ( g_en_state && g_scan) {
		
		if ( (t_temp-p_trate) >= SCANSAMPLERATE ) {			
			
			if(g_tend > g_tstart) 
			{
				g_tnow += p_rate;
				if( (g_tnow+g_tfine) > g_tend) g_tnow = g_tend - g_tfine;
			}	
		else 
			{
				g_tnow -= p_rate;
				if( (g_tnow+g_tfine) < g_tend) g_tnow = g_tend - g_tfine;
			}
		p_rateflag = 1;	
	    }
	}
					
	if (p_rateflag == 1) {
		p_rateflag = 0;
		p_trate = t_temp; 
//		I2CWriteData(I2C_COM_TEST);// use to check rate update time
		g_vset = ReturnVset(g_tnow+g_tfine, 0);
	    I2CWriteData(I2C_COM_VSET);
	}
		
}
void DTC03SMaster::CheckVact()
{
	float tact;
	I2CReadVact();
  	tact = ReturnTemp(g_vact,0);
  	PrintTact(tact);
}
void DTC03SMaster::UpdateEnable()//20161101
{
	if(analogRead(ENSW)>ANAREADVIH) p_en[1] = 1;
	else p_en[1] = 0;
	if(g_en_state != p_en[1])
	{
		
		g_en_state = p_en[1];
		I2CWriteData(I2C_COM_INIT);
		PrintEnable();
	}
}
void DTC03SMaster::CheckScan()
{
	unsigned long t_temp;
	if(digitalRead(SCANB)==0) 
	{
		t_temp = millis();
		if ((t_temp - g_tscan) > 50 ) {
		g_scan = !g_scan;
		p_scan[1] = g_scan;
		PrintScan();
		}
	}
	g_tscan = t_temp;	
}
void DTC03SMaster::CheckStatus()
{
	unsigned long t1, t2;
	
	if(analogRead(PUSHB)< ANAREADVIL)
	{
		t1 = millis();
		g_cursorstate +=1;				
		if(g_cursorstate ==3 || g_cursorstate ==4) g_cursorstate =0;
		g_oldcursorstate = g_cursorstate;
		
		while(analogRead(PUSHB)< ANAREADVIL)
		{
			t2 = millis();
			if(t2-t1 > LONGPRESSTIME)
				g_cursorstate =5;
		}
		if(g_cursorstate ==6)
		{
			g_cursorstate =0;
			PrintBG();
			PrintTstart();
			PrintTend();
			CheckVact();
			PrintEnable();
			PrintScan();
		}
	}
}
void DTC03SMaster::ShowCursor()
{
	switch(g_cursorstate)
	{
		case 0:
			lcd.SelectFont(SystemFont5x7,BLACK);
			if (p_curstatus0flag) {
				p_curstatus0flag = 0;
				lcd.GotoXY(TFINE_COORD_X-COLUMEPIXEL0507, TFINE_COORD_Y);
			}
			else lcd.GotoXY(RATE_COORD_X-COLUMEPIXEL0507, RATE_COORD_Y);
			lcd.print(" ");
			lcd.SelectFont(SystemFont5x7, WHITE);
			lcd.GotoXY(TSTART_COORD_X-COLUMEPIXEL0507, TSTART_COORD_Y);
			lcd.print(" ");
		break;

		case 1:
			lcd.SelectFont(SystemFont5x7,BLACK);
			lcd.GotoXY(TSTART_COORD_X-COLUMEPIXEL0507, TSTART_COORD_Y);
			lcd.print(" ");
			lcd.SelectFont(SystemFont5x7, WHITE);
			lcd.GotoXY(TEND_COORD_X-COLUMEPIXEL0507, TEND_COORD_Y);
			lcd.print(" ");
		break;

		case 2:
			lcd.SelectFont(SystemFont5x7,BLACK);
			lcd.GotoXY(TEND_COORD_X-COLUMEPIXEL0507, TEND_COORD_Y);
			lcd.print(" ");
			lcd.SelectFont(SystemFont5x7, WHITE);
			lcd.GotoXY(RATE_COORD_X-COLUMEPIXEL0507, RATE_COORD_Y);
			lcd.print(" ");
		break;
		case 3:
			lcd.SelectFont(SystemFont5x7,BLACK);
			switch (g_oldcursorstate){
				case 0 :
					lcd.GotoXY(TSTART_COORD_X-COLUMEPIXEL0507, TSTART_COORD_Y);
			        break;
			    case 1 :
					lcd.GotoXY(TEND_COORD_X-COLUMEPIXEL0507, TEND_COORD_Y);
			        break;
			    case 2 :
					lcd.GotoXY(RATE_COORD_X-COLUMEPIXEL0507, RATE_COORD_Y);
			        break;					
			}
			lcd.print(" ");
			lcd.SelectFont(SystemFont5x7, WHITE);
			lcd.GotoXY(TFINE_COORD_X-COLUMEPIXEL0507, TFINE_COORD_Y);
			lcd.print(" ");
		break;

		case 4:
			lcd.ClearScreen(0);
		break;
	}
}
void DTC03SMaster::UpdateParam()
{
	unsigned long t1;

	if(g_paramterupdate)
	{
		g_paramterupdate =0;
		switch(g_cursorstate)
		{
			case 0:
				g_tstart += g_counter2*0.01;
				if(g_tstart > 60.00) g_tstart =60.00;
				if(g_tstart < 7.00) g_tstart = 7.00;
				if(~g_en_state){
					g_tnow = g_tstart;
					g_vset = ReturnVset(g_tstart, 0);
					I2CWriteData(I2C_COM_VSET);
				}
				else{
					if (~g_scan) {
						g_vset = ReturnVset(g_tnow+g_tfine, 0);
						I2CWriteData(I2C_COM_VSET);
					}
				}
				PrintTstart();
			break;

			case 1:
				g_tend += g_counter2*0.01;
				if(g_tend > 60.00) g_tend = 60.00;
				if(g_tend< 7.00) g_tend = 7.00;
				PrintTend(); 
			break;

			case 2:
				g_rateindex +=g_counter;
				if(g_rateindex <1) g_rateindex =1;
				if(g_rateindex > MAXRATEINDEX) g_rateindex = MAXRATEINDEX;				
				g_trate = pgm_read_word_near(RateTable+g_rateindex);
				p_rate = float(g_trate)*SCANSAMPLERATE/10000.0;
				PrintRate(); 
			break;
			case 3:
				g_tfine += g_counter2*0.01;	
				if (g_tfine > FINETUNEAMP) g_tfine = FINETUNEAMP;
				if (g_tfine < -FINETUNEAMP) g_tfine = -FINETUNEAMP;
				g_vset = ReturnVset(g_tnow+g_tfine, 0);
				I2CWriteData(I2C_COM_VSET);
				PrintTnow() ;
			break;

			case 5:
				if(g_fbcbase>44900) g_fbcbase=44900;//
      			if(g_fbcbase<15100) g_fbcbase=15100;//
      			g_fbcbase +=(g_counter2*100);
      			I2CWriteData(I2C_COM_FBC);
			break;					
		}

	}
}
void DTC03SMaster::Encoder() // use rising edge triger of ENC_B
{
	unsigned char encoded, sum;
	unsigned int dt;
	unsigned long tenc;
	bool MSB, LSB;
	tenc = millis();
	
	if (( abs(tenc - g_tenc[0])) > COUNTERINCRE ) {
		g_tenc[1] = 0;
		g_tenc[2] = 0;
	}
	else g_tenc[1] = tenc - g_tenc[0];
	g_tenc[2] += g_tenc[1];
	 
	MSB = digitalRead(ENC_B);
	LSB = digitalRead(ENC_A);
	encoded = (MSB <<1)| LSB;
    if(encoded == 0b10)
	{
		g_paramterupdate =1;
		g_counter =-1;
		if (g_tenc[2] > COUNTERSPEEDUP) g_counter2 =-20;
		else g_counter2 =-1;		
	}
	else if(encoded == 0b11)
	{
		g_paramterupdate =1;
		g_counter =1;
		if (g_tenc[2] > COUNTERSPEEDUP) g_counter2 =20;
		else g_counter2 = 1;	
	}
	g_tenc[0] = tenc;	
}


