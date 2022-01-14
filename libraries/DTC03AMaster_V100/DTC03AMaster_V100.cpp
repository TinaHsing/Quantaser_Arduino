/* 
	09/19/2017
*/
#include <DTC03A_MS.h>
#include <DTC03AMaster_V100.h>

DTC03Master::DTC03Master()
{
}

unsigned char DTC03Master::QCP0_CRC_Calculate(unsigned char *pData, unsigned char Length)
{
	unsigned char Result = 0xFF;
	while(Length--) {
        Result = pgm_read_byte_near(CRC_8_CCITT_Table + (Result ^ *pData++));
		//Result = CRC_8_CCITT_Table[Result ^ *pData++];
	}
	// while(Length--) {
	// 	Result ^= *pData++;
	// 	for(int i=0; i<8; i++) {
	// 		if(Result & 0x01) {
	// 			Result = (Result >> 1) ^ CRC_8_CCITT_Poly;
	// 		} else {
	// 			Result>>=1;
	// 		}
	// 	}
	// }
	return Result;
}

void DTC03Master::QCP0_Package(unsigned char RorW, unsigned short Command, unsigned short Data, unsigned char *pData)
{
	pData[0] = 0x69;
	pData[1] = RorW;
	pData[2] = (unsigned char)Command;
	pData[3] = (unsigned char)(Command >> 8);
	pData[4] = (unsigned char)Data;
	pData[5] = (unsigned char)(Data >> 8);
	unsigned char Temp = QCP0_CRC_Calculate(pData, (8 - 2));
	pData[6] = Temp;
	pData[7] = 0x96;
	return;
}

void DTC03Master::QCP0_Unpackage(unsigned char *pData, unsigned char *RorW, unsigned short *Command, unsigned short *Data)
{
	unsigned char CRC_RESULT = QCP0_CRC_Calculate(pData, (8 - 2));
	if(CRC_RESULT != pData[6]) {
		*RorW = 0xFF;
		return;
	}
	*RorW = pData[1];
	*Command = ((unsigned short)pData[3] << 8) + (unsigned short)pData[2];
	*Data = ((unsigned short)pData[5] << 8) + (unsigned short)pData[4];
	return;
}

void DTC03Master::QCP0_REG_PROCESS(unsigned short Command, unsigned short Data)
{
    switch (Command)
    {
    case I2C_DEVICE_STATE:
        break;
    case I2C_FW_VERSION:
        break;
    case I2C_IO_STATE:
        g_IO_State = (unsigned char)Data;
        break;
    case I2C_PID_MODE:
        g_PID_Mode = (unsigned char)Data;
        break;
    case I2C_PID_TARGET:
        g_V_Set = Data;
        break;
    case I2C_PID_K:
        g_K = Data;
        break;
    case I2C_PID_Ti:
        g_Ti = Data;
        break;
    case I2C_PID_Td:
        g_Td = Data;
        break;
    case I2C_PID_HiLimit:
        g_HiLimit = Data;
        break;
    case I2C_PID_LoLimit:
        g_LoLimit = Data;
        break;
    case I2C_V_Limit:
        g_V_Lim = Data;
        break;
    case I2C_I_Limit:
        g_I_Lim = Data;
        break;
    case I2C_V_TEC:
        g_V_Tec = (short)Data;
        break;
    case I2C_I_TEC:
        g_I_Tec = (short)Data;
        break;
    case I2C_TEMP_DATA:
        break;
    case I2C_TEMP_AVERAGE_DATA:
        g_V_Act = Data;
        break;
    case I2C_ATUN_TYPE:
        g_Auto_Type = (unsigned char)Data;
        break;
    case I2C_ATUN_DeltaDuty:
        g_Auto_Type = (unsigned char)g_Auto_Delta;
        break;
    default:
        break;
    }
}

void DTC03Master::SetPinMode()
{
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT);
    pinMode(ENC_SW, INPUT);
    pinMode(PUSH_ENABLE, INPUT);
}

void DTC03Master::ParamInit()
{
    Wire.begin();
    lcd.Init();    
    g_enc_pressed = false;
    g_paramupdate = 0;
    //g_sensortype = 0;
    g_tsetstep = 1.00;
    g_PID_Mode = PID_Off;
    g_atune_status = 0;
    g_cursorstate = 1;
    p_cursorStateCounter[0] = 0;
    p_cursorStateCounter[1] = 0;
    p_cursorStateCounter[2] = 0;
    p_cursorStayTime = 0;
    p_tBlink = 0;
    p_tBlink_toggle = 0;
    p_blinkTsetCursorFlag = 0;
    p_loopindex = 0;
    p_ee_changed = 0;
    p_holdCursorTimer = 0;
    p_HoldCursortateFlag = 0;
    p_atunProcess_flag = 0;
    g_lock_flag = 0;
    g_tenc = millis();
}

void DTC03Master::ReadEEPROM()
{
    unsigned char noeedummy, temp_upper, temp_lower;
    noeedummy = EEPROM.read(EEADD_DUMMY);
    if (noeedummy == NOEE_DUMMY)
    {
        g_V_Set = EEPROM.read(EEADD_VSET_UPPER) << 8 | EEPROM.read(EEADD_VSET_LOWER);
        g_I_Lim = EEPROM.read(EEADD_currentlim);
        g_K = EEPROM.read(EEADD_P);
        g_bconst = EEPROM.read(EEADD_BCONST_UPPER) << 8 | EEPROM.read(EEADD_BCONST_LOWER);
    }
    else
    {
        EEPROM.write(EEADD_DUMMY, NOEE_DUMMY);
        EEPROM.write(EEADD_VSET_UPPER, NOEE_VSET >> 8);
        EEPROM.write(EEADD_VSET_LOWER, NOEE_VSET);
        EEPROM.write(EEADD_currentlim, NOEE_ILIM);
        EEPROM.write(EEADD_P, NOEE_P);
        EEPROM.write(EEADD_KIINDEX, NOEE_kiindex);
        EEPROM.write(EEADD_BCONST_UPPER, NOEE_BCONST >> 8);
        EEPROM.write(EEADD_BCONST_LOWER, NOEE_BCONST);
        EEPROM.write(EEADD_MODSTATUS, NOEE_MODSTATUS);
        EEPROM.write(EEADD_R1, NOEE_R1);
        EEPROM.write(EEADD_R2, NOEE_R2);
        EEPROM.write(EEADD_TPIDOFF, NOEE_TPIDOFF);
        EEPROM.write(EEADD_FBC_UPPER, NOEE_FBC >> 8);
        EEPROM.write(EEADD_FBC_LOWER, NOEE_FBC);
        EEPROM.write(EEADD_MODOFF_UPPER, NOEE_MODOFF >> 8);
        EEPROM.write(EEADD_MODOFF_LOWER, NOEE_MODOFF);
        EEPROM.write(EEADD_RMEAS_UPPER, NOEE_RMEAS >> 8);
        EEPROM.write(EEADD_RMEAS_LOWER, NOEE_RMEAS);
        EEPROM.write(EEADD_TOTP_UPPER, NOEE_TOTP >> 8);
        EEPROM.write(EEADD_TOTP_LOWER, NOEE_TOTP);
        EEPROM.write(EEADD_PAP, NOEE_PAP);
        EEPROM.write(EEADD_TBIAS, NOEE_TBIAS);
        EEPROM.write(EEADD_ATSTABLE, NOEE_ATSTABLE);

        g_V_Set = NOEE_VSET;
        g_I_Lim = NOEE_ILIM;
        g_K = NOEE_P;
        g_bconst = NOEE_BCONST;
    }
    g_tset = ReturnTemp(g_V_Set);
}

void DTC03Master::SaveEEPROM()
{
    if (p_ee_changed == 1)
    {
        p_ee_changed = 0;
        switch (p_ee_change_state)
        {
        case EEADD_VSET_UPPER:
            EEPROM.write(EEADD_VSET_UPPER, g_V_Set >> 8);
            EEPROM.write(EEADD_VSET_LOWER, g_V_Set);
            break;
        case EEADD_BCONST_UPPER:
            EEPROM.write(EEADD_BCONST_UPPER, g_bconst >> 8);
            EEPROM.write(EEADD_BCONST_LOWER, g_bconst);
            break;
        case EEADD_currentlim:
            EEPROM.write(EEADD_currentlim, g_I_Lim);
            break;
        case EEADD_P:
            EEPROM.write(EEADD_P, g_K);
            break;
        }
    }
}

void DTC03Master::CheckStatus()
{
    float Temp;
    switch (p_loopindex)
    {
    case 0:
        I2CReadData(I2C_I_TEC);
        Temp = float(g_I_Tec) * CURRENTRatio;
        PrintItec(Temp);
        break;
    case 1:
        I2CReadData(I2C_TEMP_AVERAGE_DATA);
        Temp = ReturnTemp(g_V_Act);
        PrintTact(Temp);    
        break;
    case 2:
        if(g_PID_Mode == PID_Autotune) {
            I2CReadData(I2C_PID_MODE);
            if(g_PID_Mode != PID_Autotune) {
                I2CReadData(I2C_PID_K);
                I2CReadData(I2C_PID_Ti);
                PrintAtuneDone();
            }
        }
        break;
    default:
        break;
    }
    if(p_loopindex == 2) {
        p_loopindex = 0;
    } else {
        p_loopindex++;
    }
}

void DTC03Master::I2CWriteAll()
{
    I2CWriteData(I2C_PID_MODE, g_PID_Mode);
    I2CWriteData(I2C_PID_TARGET, g_V_Set);
    I2CWriteData(I2C_PID_K, g_K);
    I2CWriteData(I2C_PID_Ti, g_Ti);
    I2CWriteData(I2C_PID_Td, 0);
    I2CWriteData(I2C_PID_HiLimit, g_HiLimit);
    I2CWriteData(I2C_PID_LoLimit, g_LoLimit);
    I2CWriteData(I2C_V_Limit, g_V_Lim);
    I2CWriteData(I2C_I_Limit, g_I_Lim);
    I2CWriteData(I2C_TEMP_SENSOR_EN, 0x0001);
    I2CWriteData(I2C_TEMP_SENSOR_MODE, (unsigned short)g_Temp_Sensor_Mode);
    I2CWriteData(I2C_ATUN_TYPE, g_Auto_Type);
    I2CWriteData(I2C_ATUN_DeltaDuty, g_Auto_Delta);
}

void DTC03Master::I2CWriteData(unsigned short Command , unsigned short Data)
{
    unsigned char RorW = 0xA5;
    unsigned char Package[8];   
    QCP0_Package((unsigned char)RorW, (unsigned short)Command, 
                 (unsigned short)Data, (unsigned char*)&Package);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(Package, 8);
    Wire.endTransmission();
    delayMicroseconds(I2CSENDDELAY);
    Wire.requestFrom(SLAVE_ADDR, 8);
    if(Wire.available() != 8) {
        return;
    }
    for(int i=0; i<8; i++) {
        Package[i] = Wire.read();
    }
    QCP0_Unpackage((unsigned char*)&Package, (unsigned char *)&RorW, 
                   (unsigned short *)&Command, (unsigned short *)&Data);
    
    QCP0_REG_PROCESS((unsigned short)Command, (unsigned short)Data);
}

void DTC03Master::I2CReadData(unsigned short Command)
{
    unsigned char RorW = 0xAA;
    unsigned char Package[8];
    unsigned short Data = 0x1234;  
    QCP0_Package((unsigned char)RorW, (unsigned short)Command, 
                 (unsigned short)Data, (unsigned char*)&Package);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(Package, 8);
    Wire.endTransmission();
    delayMicroseconds(I2CSENDDELAY);
    Wire.requestFrom(SLAVE_ADDR, 8);
    if(Wire.available() != 8) {
        return;
    }
    for(int i=0; i<8; i++) {
        Package[i] = Wire.read();
    }
    QCP0_Unpackage((unsigned char*)&Package, (unsigned char *)&RorW, 
                   (unsigned short *)&Command, (unsigned short *)&Data);
    
    QCP0_REG_PROCESS((unsigned short)Command, (unsigned short)Data);
}

float DTC03Master::ReturnTemp(unsigned int vact)
{
    return (1 / (log((float)vact / RTHRatio) / (float)g_bconst + T0INV) - 273.15);
}

unsigned int DTC03Master::ReturnVset(float tset)
{
    return ((unsigned int)RTHRatio * exp(-1 * (float)g_bconst * (T0INV - 1 / (tset + 273.15))));
}

void DTC03Master::WelcomeScreen()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(0, 0);
    //  lcd.print("DTC03 Ver.3.01");
    lcd.print("DTC03 Ver.3.02"); // 3.02 for autotune
    lcd.GotoXY(0, ROWPIXEL0507 * 1);
    lcd.print("Initializing");
    for (byte i = 5; i > 0; i--)
    {
        lcd.GotoXY(COLUMNPIXEL0507 * (12 + 1), ROWPIXEL0507 * 1);
        lcd.print(i);
        delay(1000);
    }
    lcd.ClearScreen(0); //0~255 means ratio of black
}

void DTC03Master::BackGroundPrint()
{
    lcd.SelectFont(Iain5x7);
    lcd.GotoXY(TSET_COORD_X, TSET_COORD_Y);
    lcd.print(Text_SET);
    lcd.GotoXY(TACT_COORD_X, TACT_COORD_Y);
    lcd.print(Text_ACT);
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(ITEC_COORD_X, ITEC_COORD_Y);
    lcd.print(Text_ITEC);
    lcd.GotoXY(ILIM_COORD_X, ILIM_COORD_Y);
    lcd.print(Text_ILIM);
    lcd.GotoXY(P_COORD_X, P_COORD_Y);
    lcd.print(Text_P);
    lcd.GotoXY(I_COORD_X, I_COORD_Y);
    lcd.print(Text_I);
    lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
    lcd.print(Text_B);
    lcd.GotoXY(ATUNE_DELTA_COORD_X, ATUNE_DELTA_COORD_Y);
    lcd.print(Text_ATUNE_DELTA);
    lcd.GotoXY(ATUNE_COORD_X, ATUNE_COORD_Y);
    lcd.print(Text_AT);
}

void DTC03Master::PrintNormalAll()
{
    PrintTset();
    PrintIlim();
    PrintP();
    PrintKi();
    PrintB();
    PrintAtune();
    //No need to add print Itec and Vact here, checkstatus() will do this
}

void DTC03Master::PrintTset()
{
    lcd.SelectFont(fixed_bold10x15);
    lcd.GotoXY(TSET_COORD_X2, TSET_COORD_Y);
    if (g_tset < 10.000)
        lcd.print("  ");
    else if (g_tset < 100.000)
        lcd.print(" ");

    lcd.print(g_tset, 3);
}

void DTC03Master::PrintTact(float tact)
{
    lcd.SelectFont(Arial_bold_14);
    lcd.GotoXY(TACT_COORD_X2, TACT_COORD_Y);
    
    if (tact <= 0.000) {
        if (abs(tact) < 10.000)
            lcd.print(" ");
        lcd.print(tact, 3);
    } else {
        if (tact < 10.000)
            lcd.print("   ");
        else if (tact < 100.000)
            lcd.print("  ");
        else
            lcd.print(" ");

        lcd.print(tact, 3);
    }
    lcd.print(" ");
}

void DTC03Master::PrintItec(float itec)
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(ITEC_COORD_X2, ITEC_COORD_Y);
    //  if ( abs(itec) <= 0.015 ) itec = 0;
    if (itec < 0.00) {
        lcd.print(itec, 2);
    } else {
        lcd.print(" ");
        lcd.print(itec, 2);
    }
}

void DTC03Master::PrintIlim()
{
    float currentlim;
    currentlim = ILIMSTART + ILIMSTEP * (float)(g_I_Lim);
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(ILIM_COORD_X2, ILIM_COORD_Y);
    //  lcd.print(" ");
    lcd.print(currentlim, 2);
}

void DTC03Master::PrintP()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(P_COORD_X2, P_COORD_Y);
    if (g_PID_Mode != PID_Off) {
        if (g_K < 10)
            lcd.print("  ");
        else if (g_K < 100)
            lcd.print(" ");
        lcd.print(g_K);
    } else {
        lcd.print("RTE");
    }
}

void DTC03Master::PrintKi()
{
    //unsigned int tconst;
    float tconst;
    lcd.SelectFont(SystemFont5x7);
    tconst = 0;//float(pgm_read_word_near(timeconst + g_kiindex)) / 100.0;
    lcd.GotoXY(I_COORD_X2, I_COORD_Y);
    if(tconst == 0)
    {
        lcd.print(" OFF");
    }
    else
    {
        lcd.print("  ");
        lcd.print(tconst, 0);
    }
}

void DTC03Master::PrintB()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(BCONST_COORD_X2, BCONST_COORD_Y);
    lcd.print(g_bconst);
}

void DTC03Master::PrintAtune()
{
    lcd.SelectFont(SystemFont5x7);
    if (p_atunProcess_flag) {
        g_lock_flag = 1;
        p_atunProcess_flag = 0;
        lcd.GotoXY(P_COORD_X, P_COORD_Y);
        lcd.print(" ____  ");
        lcd.GotoXY(I_COORD_X, I_COORD_Y);
        lcd.print("|Auto|");
        lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
        lcd.print("|Tune|");
        lcd.GotoXY(ATUNE_DELTA_COORD_X, ATUNE_DELTA_COORD_Y);
        lcd.print("|....|");
        lcd.GotoXY(ATUNE_COORD_X, ATUNE_COORD_Y);
        lcd.print("|..  |");
    } else {
        lcd.GotoXY(ATUNE_COORD_X2, ATUNE_COORD_Y);
        if (g_atune_status == 0)
            lcd.print("OFF");
        else
            lcd.print(" ON");
    }
}

void DTC03Master::PrintAtuneDone()
{
    g_lock_flag = 0;
    g_atune_status = 0;

    BackGroundPrint();
    PrintNormalAll();
}

void DTC03Master::UpdateEnable()
{
    if (analogRead(PUSH_ENABLE) > 500) {
        if(!g_atune_status && (g_PID_Mode != PID_On)) {
            g_PID_Mode = PID_On;
            I2CWriteData(I2C_PID_MODE, PID_On);
        }
    } else {
        if(g_PID_Mode != PID_Off) {
            g_PID_Mode = PID_Off;
            I2CWriteData(I2C_PID_MODE, PID_Off);
        }
    }
}

void DTC03Master::CursorState()
{
    unsigned long t1, d1;
    unsigned int t_temp;
    if (!g_lock_flag)
    {
        if(g_enc_pressed)
        {
            t_temp = millis();

            if (abs(t_temp - p_cursorStateCounter[0]) < ACCUMULATE_TH) //ACCUMULATE_TH=50
            {
                p_cursorStateCounter[1] = t_temp - p_cursorStateCounter[0];
                p_cursorStateCounter[2] += p_cursorStateCounter[1];
            }
            else
            {
                p_cursorStateCounter[2] = 0;
            }

            if (p_cursorStateCounter[2] > LONGPRESSTIME) //long press case:
            {
                if (abs(t_temp - p_cursorStayTime) > CURSORSTATE_STAYTIME && p_tBlink_toggle)
                {
                    p_HoldCursortateFlag = 0;
                    if (g_cursorstate == 0 || g_cursorstate == 1)
                        g_cursorstate = 2;
                    else
                        g_cursorstate++;

                    if (g_cursorstate > 7)
                        g_cursorstate = 2;

                    ShowCursor(0); //the index is not important
                    p_cursorStayTime = t_temp;
                }
            }
            else //short press case:
            {
                if (abs(t_temp - p_tcursorStateBounce) > DEBOUNCE_WAIT) //DEBOUNCE_WAIT=ACCUMULATE_TH*4
                {
                    if (g_cursorstate == 0)
                        g_cursorstate = 1;

                    if (g_cursorstate == 1) {
                        if (g_tsetstep <= 0.001)
                            g_tsetstep = 1.0;
                        else
                            g_tsetstep = g_tsetstep / 10.0;

                        ShowCursor(0);
                    } else {  //g_cursorstate=2~7
                        if (g_cursorstate == 7 && g_atune_status) {
                            p_atunProcess_flag = 1;
                            g_PID_Mode = PID_Autotune;
                            I2CWriteData(I2C_PID_MODE, PID_Autotune);
                            PrintAtune();
                        }
                        p_HoldCursortateFlag = 1;
                        p_timerResetFlag = 1;
                    }
                    p_tcursorStateBounce = t_temp;
                }
            }
            p_cursorStateCounter[0] = t_temp;
        }
    }
}

void DTC03Master::HoldCursortate() //put this method in loop
{
    unsigned int t_temp, timer;
    unsigned char oldCursorState;
    if (p_HoldCursortateFlag == 1)
    {
        //start timer when enter this section-----
        t_temp = millis();
        if (p_timerResetFlag == 1)
        {
            p_timerResetFlag = 0;
            p_holdCursorTimer = t_temp;
        }
        timer = t_temp - p_holdCursorTimer; //alway reset the timer when g_cursorstate=2~6 @ short press case

        if (timer < DEBOUNCE_WAIT * 2)
        {
        }    // do nothing if reset the timer
        else //wait too long! g_imer > 2*DEBOUNCE_WAIT
        {
            p_HoldCursortateFlag = 0;
            oldCursorState = g_cursorstate;
            g_cursorstate = 0;
            ShowCursor(oldCursorState); //here oldCursorState from 2~6
            g_cursorstate = 1;
        }
    }
}

void DTC03Master::blinkTsetCursor()
{
    unsigned int t_temp;
    if (p_blinkTsetCursorFlag == 1)
    {
        t_temp = millis();

        lcd.SelectFont(fixed_bold10x15);
        if (g_tsetstep == 1.0)
            lcd.GotoXY(TSET_COORD_X2 + 2 * COLUMNPIXEL1015, TSET_COORD_Y);
        else if (g_tsetstep == 0.1)
            lcd.GotoXY(TSET_COORD_X2 + 4 * COLUMNPIXEL1015, TSET_COORD_Y);
        else if (g_tsetstep == 0.01)
            lcd.GotoXY(TSET_COORD_X2 + 5 * COLUMNPIXEL1015, TSET_COORD_Y);
        else
            lcd.GotoXY(TSET_COORD_X2 + 6 * COLUMNPIXEL1015, TSET_COORD_Y);

        if (abs(t_temp - p_tBlink) > BLINKDELAY)
        {
            if (p_tBlink_toggle)
            {
                lcd.print(" ");
                p_tBlink_toggle = !p_tBlink_toggle;
            }
            else
            {
                PrintTset();
                p_tBlink_toggle = !p_tBlink_toggle;
            }
            p_tBlink = t_temp;
        }
    }
}

void DTC03Master::ShowCursor(unsigned char state_old)
{
    if (g_cursorstate != 1)
        p_blinkTsetCursorFlag = 0;

    switch (g_cursorstate)
    {
    case 0:
        lcd.SelectFont(SystemFont5x7);
        switch (state_old)
        {
        case 2:
            lcd.GotoXY(ILIM_COORD_X - COLUMNPIXEL0507, ILIM_COORD_Y);
            break;
        case 3:
            lcd.GotoXY(P_COORD_X - COLUMNPIXEL0507, P_COORD_Y);
            break;
        case 4:
            lcd.GotoXY(I_COORD_X - COLUMNPIXEL0507, I_COORD_Y);
            break;
        case 5:
            lcd.GotoXY(BCONST_COORD_X - COLUMNPIXEL0507, BCONST_COORD_Y);
            break;
        case 6:
            lcd.GotoXY(ATUNE_DELTA_COORD_X - COLUMNPIXEL0507, ATUNE_DELTA_COORD_Y);
            break;
        case 7:
            lcd.GotoXY(ATUNE_COORD_X - COLUMNPIXEL0507, ATUNE_COORD_Y);
            break;
        }
        lcd.print(" ");
        break;
    case 1:
        p_blinkTsetCursorFlag = 1;
        break;
    case 2:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(ILIM_COORD_X - COLUMNPIXEL0507, ILIM_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(ATUNE_COORD_X - COLUMNPIXEL0507, ATUNE_COORD_Y); //
        lcd.print(" ");
        break;
    case 3:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(P_COORD_X - COLUMNPIXEL0507, P_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(ILIM_COORD_X - COLUMNPIXEL0507, ILIM_COORD_Y);
        lcd.print(" ");
        break;
    case 4:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(I_COORD_X - COLUMNPIXEL0507, I_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(P_COORD_X - COLUMNPIXEL0507, P_COORD_Y);
        lcd.print(" ");
        break;
    case 5:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(BCONST_COORD_X - COLUMNPIXEL0507, BCONST_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(I_COORD_X - COLUMNPIXEL0507, I_COORD_Y);
        lcd.print(" ");
        break;
    case 6:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(ATUNE_DELTA_COORD_X - COLUMNPIXEL0507, ATUNE_DELTA_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(BCONST_COORD_X - COLUMNPIXEL0507, BCONST_COORD_Y);
        lcd.print(" ");
        break;
    case 7:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(ATUNE_COORD_X - COLUMNPIXEL0507, ATUNE_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(ATUNE_DELTA_COORD_X - COLUMNPIXEL0507, ATUNE_DELTA_COORD_Y);
        lcd.print(" ");
        break;
    }
}

void DTC03Master::UpdateParam() // Still need to add the upper and lower limit of each variable
{
    unsigned char ki, ls;
    unsigned long timer1, timer2;
    if (g_paramupdate)
    {
        g_paramupdate = 0;
        p_ee_changed = 1;
        switch (g_cursorstate)
        {
        case 0:
            break;
        case 1:
            if(g_EncodeDir) {
                g_tset += g_tsetstep;
            } else {
                g_tset -= g_tsetstep;
            }
            if (g_tset > 200)
                g_tset = 200;
            if (g_tset < 7)
                g_tset = 7;
            g_V_Set = ReturnVset(g_tset);
            I2CWriteData(I2C_PID_TARGET, g_V_Set);
            PrintTset();
            p_blinkTsetCursorFlag = 0;
            p_ee_change_state = EEADD_VSET_UPPER;
            break;
        case 2:
            if(g_EncodeDir) {
                g_I_Lim++;
            } else {
                g_I_Lim--;
            }
            if (g_I_Lim > 51)
                g_I_Lim = 51;
            if (g_I_Lim < 1)
                g_I_Lim = 1;
            I2CWriteData(I2C_I_Limit, g_I_Lim);
            PrintIlim();
            p_ee_change_state = EEADD_currentlim;
            break;
        case 3:
            if(g_EncodeDir) {
                g_K++;
            } else {
                g_K--;
            }
            if (g_K > 150)
                g_K = 150;
            if (g_K < 1)
                g_K = 1;
            I2CWriteData(I2C_PID_K, g_K);
            PrintP();
            p_ee_change_state = EEADD_P;
            p_ee_changed = 1;
        case 4:
            if(g_EncodeDir) {
                if(g_Ti < 5000)
                    g_Ti++;
            } else {
                if(g_Ti > 0)
                    g_Ti--;
            }
            I2CWriteData(I2C_PID_Ti, g_Ti);
            PrintKi();
            p_ee_change_state = EEADD_KIINDEX;
            break;
        case 5:
            if(g_EncodeDir) {
                g_bconst++;
            } else {
                g_bconst--;
            }
            if (g_bconst > 4499)
                g_bconst = 4499;
            if (g_bconst < 3501)
                g_bconst = 3501;
            g_V_Set = ReturnVset(g_tset);
            I2CWriteData(I2C_PID_TARGET, g_V_Set);
            PrintB();
            p_ee_change_state = EEADD_BCONST_UPPER;
            break;
        case 6:
            break;
        case 7:
            g_atune_status = !g_EncodeDir;
            if (g_atune_status) {
                I2CWriteData(I2C_ATUN_TYPE, Autotune_PI);
                I2CWriteData(I2C_ATUN_DeltaDuty, 42);
            }
            PrintAtune();
            break;
        }
    }
}

void DTC03Master::Encoder()
{
    unsigned char dt = 0;
    unsigned long tenc = 0;
    bool ENC_B_STATE = false;
    if (!g_lock_flag)
    {
        tenc = millis();
        dt = tenc - g_tenc;
        if (dt < DEBOUNCETIME)
            return;
        g_tenc = tenc;

        ENC_B_STATE = digitalRead(ENC_B);
        if(ENC_B_STATE) {
            g_EncodeDir = false;
        } else {
            g_EncodeDir = true;
        }
        g_paramupdate = 1;
    }
}

void DTC03Master::EncoderButton()
{
    if (analogRead(ENC_SW) <= HIGHLOWBOUNDRY) {//change cursorstate when push encoder switch
        g_enc_pressed = true;
    } else {
        g_enc_pressed = false;
    }
}

