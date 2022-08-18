/* 
	09/19/2017
*/
#include <DTC03AMaster_V101.h>

DTC03Master::DTC03Master()
{
}

float DTC03Master::ReturnTemp(unsigned int vact)
{
    float Temp = 0;
    if(g_IO_State & IO_SENSOR_I_MODE) {
        Temp = (1 / (log(float(vact) / float(RTHRatio_Hi)) / float(g_B_Const) + T0INV) - 273.15);
    } else {
        Temp = (1 / (log(float(vact) / float(RTHRatio_Lo)) / float(g_B_Const) + T0INV) - 273.15);
    }
    Temp = round(Temp * 1000);
    Temp /= 1000;
    return Temp;
}

unsigned int DTC03Master::ReturnVset(float tset)
{
    if(g_IO_State & IO_SENSOR_I_MODE) {
        return round(RTHRatio_Hi * exp(-1 * (float)g_B_Const * (T0INV - 1 / (tset + 273.15))));
    } else {
        return round(RTHRatio_Lo * exp(-1 * (float)g_B_Const * (T0INV - 1 / (tset + 273.15))));   
    }
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
    float Temp;
    switch (Command)
    {
    case I2C_DEVICE_STATE:
        g_Dev_State = Data;
        break;
    case I2C_FW_VERSION:
        break;
    case I2C_MEM_LOAD:
        g_Mem_Load = (unsigned char)Data;
        break;
    case I2C_REMOTE:
        if(g_Remote != (unsigned char)Data) {
            g_Remote = (unsigned char)Data;
            p_blinkTsetCursorFlag = 0;
            g_tsetstep = 1.0;
            PrintTset();
            ShowCursor(0);
        }
        break;
    case I2C_IO_STATE:
    case I2C_TEMP_SENSOR_EN:
        g_IO_State = (unsigned char)Data;
        break;
    case I2C_TEMP_SENSOR_MODE:
        g_IO_State = (unsigned char)Data;
        if(g_I_Bias != Ib_Auto) {
            if(g_IO_State & IO_SENSOR_I_MODE) {
                g_I_Bias = Ib_1_6mA;
            } else {
                g_I_Bias = Ib_200uA;
            }
        }
        break;
    case I2C_PID_MODE:
        g_PID_Mode = (unsigned char)Data;
        if(g_PID_Mode == PID_Autotune) {
            if(!g_lock_flag) {
                p_atunProcess_flag = 1;
                PrintAtune();
            }
        } else {
            if(g_lock_flag) {
                I2CReadData(I2C_ATUN_Result);
                PrintAtuneDone();
                I2CReadData(I2C_PID_K);
                I2CReadData(I2C_PID_Ti);
            }
        }
        break;
    case I2C_PID_TARGET:
        if(g_V_Set != Data) {
            g_V_Set = Data;
        }
        break;
    case I2C_PID_K:
        if(g_K != Data) {
            g_K = Data;
            PrintK();
        }
        break;
    case I2C_PID_Ti:
        if(g_Ti != Data) {
            g_Ti = Data;
            PrintTi();
        }
        break;
    case I2C_PID_Td:
        //g_Td = Data;
        break;
    case I2C_PID_HI_LIMIT:
        if(g_HI_LIMIT != Data) {
            g_HI_LIMIT = Data;
            PrintM();
        }
        break;
    case I2C_PID_LO_LIMIT:
        if(g_LO_LIMIT != Data) {
            g_LO_LIMIT = Data;
            PrintM();
        }
        break;
    case I2C_V_Limit:
        g_V_Lim = Data;
        break;
    case I2C_I_Limit:
        if(g_I_Lim != Data) {
            g_I_Lim = Data;
            if(!g_Remote) {
                g_I_Print = Bin_To_Ilim * (float)(g_I_Lim);
                g_I_Print = round(g_I_Print * 100);
                g_I_Print /= 100;
                PrintIlim();
            }
        }
        break;
    case I2C_V_TEC:
        g_V_Tec = (short)Data;
        break;
    case I2C_I_TEC:
        g_I_Tec = (short)Data;
        Temp = (float(g_I_Tec) * Bin_To_Itec) - 5;
        PrintItec(Temp);
        break;
    case I2C_TEMP_DATA:
        break;
    case I2C_TEMP_AVERAGE_DATA:
        g_V_Act = Data;
        Temp = ReturnTemp(g_V_Act);
        if((g_V_Act < V_ACT_LIM_H) && (g_V_Act > V_ACT_LIM_L)) {
            PrintTact(Temp);
            if(g_Remote && (g_I_Bias == Ib_Auto)) {
                if(Temp < 65) {
                    if(g_IO_State & IO_SENSOR_I_MODE) {
                        if(g_PID_Mode != PID_Autotune) {
                            I2CWriteData(I2C_PID_MODE, PID_Hold);
                        }
                        I2CWriteData(I2C_TEMP_SENSOR_MODE, 0x0000);
                        Temp = ReturnVset(g_T_Set);
                        I2CWriteData(I2C_PID_TARGET, Temp);
                        if(g_PID_Mode != PID_Autotune) {
                            I2CWriteData(I2C_PID_MODE, PID_Normal);
                        }
                    }
                } else if(Temp > 70) {
                    if(!(g_IO_State & IO_SENSOR_I_MODE)) {
                        if(g_PID_Mode != PID_Autotune) {
                            I2CWriteData(I2C_PID_MODE, PID_Hold);
                        }
                        I2CWriteData(I2C_TEMP_SENSOR_MODE, 0x0001);
                        Temp = ReturnVset(g_T_Set);
                        I2CWriteData(I2C_PID_TARGET, Temp);
                        if(g_PID_Mode != PID_Autotune) {
                            I2CWriteData(I2C_PID_MODE, PID_Normal);
                        }
                    }
                }
            }
        } else {
            PrintTact(-1);
        }
        break;
    case I2C_TEMP_B_CONSTANT:
        if(g_B_Const != Data) {
            g_B_Const = Data;        
            if(g_Remote) {
                g_V_Set = ReturnVset(g_T_Set);
                I2CWriteData(I2C_PID_TARGET, g_V_Set);
            }
            PrintB();
        }
        break;
    case I2C_ATUN_TYPE:
        if(g_Auto_Type = (unsigned char)Data) {
            g_Auto_Type = (unsigned char)Data;
        }
        break;
    case I2C_ATUN_DeltaDuty:
        if(g_Auto_Delta != (short)Data) {
            g_Auto_Delta = (short)Data;
            if(!g_Remote && !g_lock_flag) {
                g_I_AutoDelta = round(Bin_To_Idelta * (float)(g_Auto_Delta));
                PrintAtunDelta();
            }
        }
        break;
    case I2C_ATUN_Result:
        g_Auto_Result = (unsigned char)Data;
        break;
    default:
        break;
    }
}

void DTC03Master::SetPinMode()
{
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT);
    // pinMode(ENC_SW, INPUT);
    // pinMode(PUSH_ENABLE, INPUT);
}

void DTC03Master::ParamInit()
{
    Wire.begin();
    lcd.Init();
    g_Dev_State = PowerOff;
    g_Mem_Load = 0x00;
    g_Remote = 0x01;
    g_IO_State = 0x00;
    g_PID_Mode = PID_Normal;
    g_Auto_Result = 0;
    g_paramupdate = 0;
    g_tsetstep = 1.0;
    g_atune_status = 0;
    g_cursorstate = 0;
    p_LongPress = false;
    p_PressTime[0] = 0;
    p_PressTime[1] = 0;
    p_tBlink = 0;
    p_tBlink_toggle = 1;
    p_blinkTsetCursorFlag = 0;
    p_loopindex = 0;
    p_ee_update_state = EEADD_BCONST_UPPER;
    p_holdCursorTimer = 0;
    p_atunProcess_flag = 0;
    g_lock_flag = 0;
    g_param_t = millis();
    g_tenc = millis();
}

void DTC03Master::ReadEEPROM()
{
    unsigned char Dummy = EEPROM.read(EEADD_DUMMY);
    if(Dummy != NOEE_DUMMY) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_BCONST_LOWER, g_B_Const);
    if((g_B_Const > 4500) || (g_B_Const < 3000)) {
        goto MemoryError;
    }

    if(EEPROM.read(EEADD_I_Bias_IO)) {
        g_IO_State |= IO_SENSOR_I_MODE;
    } else {
        g_IO_State &= ~IO_SENSOR_I_MODE;
    }

    EEPROM.get(EEADD_I_Bias, g_I_Bias);
    if(g_I_Bias > Ib_1_6mA) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_VSET_LOWER, g_V_Set);
    g_T_Set = ReturnTemp(g_V_Set);
    if((g_T_Set < 7) || (g_T_Set > 200)) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_Ilim_LOWER, g_I_Lim);
    g_I_Print = Bin_To_Ilim * (float)(g_I_Lim);
    if(g_I_Print > 3) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_Vlim_LOWER, g_V_Lim);
    if(g_V_Lim > NOEE_VLIM) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_K_LOWER, g_K);
    EEPROM.get(EEADD_Ti_LOWER, g_Ti);
    EEPROM.get(EEADD_Td_LOWER, g_Td);
    if(g_Td != NOEE_Td) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_HiLimit_LOWER, g_HI_LIMIT);
    if((g_HI_LIMIT > NOEE_HI_LIMIT) || (g_HI_LIMIT < PI_Zero)) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_LoLimit_LOWER, g_LO_LIMIT);
    if((g_LO_LIMIT > PI_Zero) || (g_LO_LIMIT < NOEE_LO_LIMIT)) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_AutoType, g_Auto_Type);
    if(g_Auto_Type > Autotune_PID) {
        goto MemoryError;
    }

    EEPROM.get(EEADD_AutoDelta_LOWER, g_Auto_Delta);
    g_I_AutoDelta = round(Bin_To_Idelta * (float)(g_Auto_Delta));
    if((g_I_AutoDelta > 500) || (g_I_AutoDelta < -500)) {
        goto MemoryError;
    }
    return;

MemoryError:
    g_B_Const = NOEE_BCONST;
    g_IO_State |= NOEE_I_Bias_IO;
    g_I_Bias = NOEE_I_Bias;
    g_V_Set = NOEE_VSET;
    g_I_Lim = NOEE_ILIM;
    g_V_Lim = NOEE_VLIM;
    g_K = NOEE_K;
    g_Ti = NOEE_Ti;
    g_Td = NOEE_Td;
    g_HI_LIMIT = NOEE_HI_LIMIT;
    g_LO_LIMIT = NOEE_LO_LIMIT;
    g_Auto_Type = NOEE_AutoType;
    g_Auto_Delta = NOEE_AutoDelta;

    g_T_Set = ReturnTemp(g_V_Set);
    g_I_Print = Bin_To_Ilim * (float)(g_I_Lim);
    g_I_Print = round(g_I_Print * 100);
    g_I_Print /= 100;
    g_I_AutoDelta = round(Bin_To_Idelta * (float)(g_Auto_Delta));

    EEPROM.write(EEADD_DUMMY, NOEE_DUMMY);
    EEPROM.put(EEADD_BCONST_LOWER, g_B_Const);
    EEPROM.write(EEADD_I_Bias_IO, NOEE_I_Bias_IO);
    EEPROM.put(EEADD_I_Bias, g_I_Bias);
    EEPROM.put(EEADD_VSET_LOWER, g_V_Set);
    EEPROM.put(EEADD_Ilim_LOWER, g_I_Lim);
    EEPROM.put(EEADD_Vlim_LOWER, g_V_Lim);
    EEPROM.put(EEADD_K_LOWER, g_K);
    EEPROM.put(EEADD_Ti_LOWER, g_Ti);
    EEPROM.put(EEADD_Td_LOWER, g_Td);
    EEPROM.put(EEADD_HiLimit_LOWER, g_HI_LIMIT);
    EEPROM.put(EEADD_LoLimit_LOWER, g_LO_LIMIT);
    EEPROM.put(EEADD_AutoType, g_Auto_Type);
    EEPROM.put(EEADD_AutoDelta_LOWER, g_Auto_Delta);

}

void DTC03Master::SaveEEPROM()
{
    switch (p_ee_update_state) {
    case EEADD_BCONST_LOWER:
        p_ee_update_state = EEADD_I_Bias_IO;
        EEPROM.update(EEADD_BCONST_LOWER, g_B_Const);
        EEPROM.update(EEADD_BCONST_UPPER, g_B_Const >> 8);
        break;
    case EEADD_I_Bias_IO:
        p_ee_update_state = EEADD_I_Bias;
        EEPROM.update(EEADD_I_Bias_IO, (g_IO_State & IO_SENSOR_I_MODE));
        break;
    case EEADD_I_Bias:
        p_ee_update_state = EEADD_VSET_LOWER;
        EEPROM.update(EEADD_I_Bias, g_I_Bias);
        break;
    case EEADD_VSET_LOWER:
        p_ee_update_state = EEADD_Ilim_LOWER;
        EEPROM.update(EEADD_VSET_LOWER, g_V_Set);
        EEPROM.update(EEADD_VSET_UPPER, g_V_Set >> 8);
        break;
    case EEADD_Ilim_LOWER:
        p_ee_update_state = EEADD_Vlim_LOWER;
        EEPROM.update(EEADD_Ilim_LOWER, g_I_Lim);
        EEPROM.update(EEADD_Ilim_UPPER, g_I_Lim >> 8);
        break;
    case EEADD_Vlim_LOWER:
        p_ee_update_state = EEADD_K_LOWER;
        EEPROM.update(EEADD_Vlim_LOWER, g_V_Lim);
        EEPROM.update(EEADD_Vlim_UPPER, g_V_Lim >> 8);
        break;
    case EEADD_K_LOWER:
        p_ee_update_state = EEADD_Ti_LOWER;
        EEPROM.update(EEADD_K_LOWER, g_K);
        EEPROM.update(EEADD_K_UPPER, g_K >> 8);
        break;
    case EEADD_Ti_LOWER:
        p_ee_update_state = EEADD_Td_LOWER;
        EEPROM.update(EEADD_Ti_LOWER, g_Ti);
        EEPROM.update(EEADD_Ti_UPPER, g_Ti >> 8);
        break;
    case EEADD_Td_LOWER:
        p_ee_update_state = EEADD_HiLimit_LOWER;
        EEPROM.update(EEADD_Td_LOWER, g_Td);
        EEPROM.update(EEADD_Td_UPPER, g_Td >> 8);
        break;
    case EEADD_HiLimit_LOWER:
        p_ee_update_state = EEADD_LoLimit_LOWER;
        EEPROM.update(EEADD_HiLimit_LOWER, g_HI_LIMIT);
        EEPROM.update(EEADD_HiLimit_UPPER, g_HI_LIMIT >> 8);
        break;
    case EEADD_LoLimit_LOWER:
        p_ee_update_state = EEADD_AutoType;
        EEPROM.update(EEADD_LoLimit_LOWER, g_LO_LIMIT);
        EEPROM.update(EEADD_LoLimit_UPPER, g_LO_LIMIT >> 8);
        break;
    case EEADD_AutoType:
        p_ee_update_state = EEADD_AutoDelta_LOWER;
        EEPROM.update(EEADD_AutoType, g_Auto_Type);
        break;
    case EEADD_AutoDelta_LOWER:
        p_ee_update_state = EEADD_BCONST_LOWER;
        EEPROM.update(EEADD_AutoDelta_LOWER, g_Auto_Delta);
        EEPROM.update(EEADD_AutoDelta_UPPER, g_Auto_Delta >> 8);
        break;
    default:
        p_ee_update_state = EEADD_BCONST_LOWER;
        break;
    }
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

void DTC03Master::I2CWriteAll()
{
    I2CWriteData(I2C_PID_MODE, g_PID_Mode);
    I2CWriteData(I2C_PID_TARGET, g_V_Set);
    I2CWriteData(I2C_PID_K, g_K);
    I2CWriteData(I2C_PID_Ti, g_Ti);
    I2CWriteData(I2C_PID_Td, 0);
    I2CWriteData(I2C_PID_HI_LIMIT, g_HI_LIMIT);
    I2CWriteData(I2C_PID_LO_LIMIT, g_LO_LIMIT);
    I2CWriteData(I2C_V_Limit, g_V_Lim);
    I2CWriteData(I2C_I_Limit, g_I_Lim);
    I2CWriteData(I2C_TEMP_SENSOR_EN, 0x0001);
    if(g_I_Bias == Ib_200uA) {
        I2CWriteData(I2C_TEMP_SENSOR_MODE, 0x0000);
    } else if(g_I_Bias == Ib_1_6mA) {
        I2CWriteData(I2C_TEMP_SENSOR_MODE, 0x0001);
    }
    I2CWriteData(I2C_ATUN_TYPE, g_Auto_Type);
    I2CWriteData(I2C_ATUN_DeltaDuty, (unsigned short)g_Auto_Delta);
    I2CWriteData(I2C_MEM_LOAD, 0x0001);
}

void DTC03Master::WaitPowerOn()
{
    do {
        I2CReadData(I2C_DEVICE_STATE);
        delay(10);
    } while (g_Dev_State != PowerOn);
}

bool DTC03Master::MemReload()
{
    I2CReadData(I2C_MEM_LOAD);
    return (!g_Mem_Load && g_Remote);
}

void DTC03Master::CheckStatus()
{
    unsigned short Temp;
    switch (p_loopindex)
    {
    case 0:
        if(g_Remote) {
            I2CReadData(I2C_REMOTE);
        }
        break;
    case 1:
        I2CReadData(I2C_IO_STATE);
        break;
    case 2:
         if(!g_Remote || g_lock_flag) {
            I2CReadData(I2C_PID_MODE);
        }
        break;
    case 3:
         if(!g_Remote) {
            I2CReadData(I2C_PID_TARGET);
            g_T_Set = ReturnTemp(g_V_Set);
            if(g_T_Set > 200) {
                g_T_Set = 200;
            } else if(g_T_Set < 7) {
                g_T_Set = 7;
            }
        }
        break;
    case 4:
        I2CReadData(I2C_I_Limit);
        break;
    case 5:
         if(!g_Remote && !g_lock_flag) {
            I2CReadData(I2C_PID_K);
        }
        break;
    case 6:
         if(!g_Remote && !g_lock_flag) {
            I2CReadData(I2C_PID_Ti);
        }
        break;
    case 7:
         if(!g_Remote && !g_lock_flag) {
            I2CReadData(I2C_TEMP_B_CONSTANT);
        }
        break;
    case 8:
        if(!g_Remote && !g_lock_flag) {
            I2CReadData(I2C_PID_HI_LIMIT);
        }
        break;
    case 9:
        if(!g_Remote && !g_lock_flag) {
            I2CReadData(I2C_PID_LO_LIMIT);
        }
        break;
    case 10:
        I2CReadData(I2C_I_TEC);
        break;
    case 11:
        I2CReadData(I2C_TEMP_AVERAGE_DATA);
        break;
    case 12:
         if(!g_Remote && !g_lock_flag) {
            I2CReadData(I2C_ATUN_DeltaDuty);
        }
        break;
    default:
        break;
    }
    if(p_loopindex == 12) {
        p_loopindex = 0;
    } else {
        p_loopindex++;
    }
}

void DTC03Master::UpdateParam() // Still need to add the upper and lower limit of each variable
{
    unsigned short Temp;
    unsigned int encstep = 1;
    unsigned long param_time = 0;
    if (g_paramupdate)
    {
        param_time = millis();
        if(abs(param_time - g_param_t) < ENC_SPEED_T) {
            encstep = 10;
        } else {
            encstep = 1;
        }
        g_param_t = millis();
        g_paramupdate = 0;
        switch (g_cursorstate)
        {
        case 0:
            if(g_EncodeDir) {
                g_T_Set += g_tsetstep;
            } else {
                g_T_Set -= g_tsetstep;
            }
            if (g_T_Set > 200)
                g_T_Set = 200;
            if (g_T_Set < 7)
                g_T_Set = 7;

            if(g_I_Bias == Ib_200uA) {
                if(g_T_Set > 70) {
                    g_T_Set = 70;
                }
            } else if(g_I_Bias == Ib_1_6mA) {
                if(g_T_Set < 65) {
                    g_T_Set = 65;
                }
            }
            
            Temp = ReturnVset(g_T_Set);
            I2CWriteData(I2C_PID_TARGET, Temp);
            PrintTset();
            p_blinkTsetCursorFlag = 0;
            break;
        case 1:
            if(g_EncodeDir) {
                g_I_Print += 0.1;
            } else {
                g_I_Print -= 0.1;
            }
            if (g_I_Print > 3)
                g_I_Print = 3;
            if (g_I_Print < 0.1)
                g_I_Print = 0.1;
                
            Temp = round(g_I_Print * Ilim_To_Bin);
            I2CWriteData(I2C_I_Limit, Temp);
            PrintIlim();
            break;
        case 2:
            g_atune_status = g_EncodeDir;
            if (g_atune_status) {
                I2CWriteData(I2C_ATUN_TYPE, Autotune_PI);
            }
            PrintAtune();
            break;
        case 3:
            if(g_EncodeDir) {
                if((g_I_AutoDelta > 0) || (g_I_AutoDelta < -10)) {
                    g_I_AutoDelta += 10;
                } else {
                    g_I_AutoDelta = 10;
                }
            } else {
                if((g_I_AutoDelta < 0) || (g_I_AutoDelta > 10)) {
                    g_I_AutoDelta -= 10;
                } else {
                    g_I_AutoDelta = -10;
                }
            }
            if (g_I_AutoDelta > 500)
                g_I_AutoDelta = 500;
            if (g_I_AutoDelta < -500)
                g_I_AutoDelta = -500;

            Temp = round(float(g_I_AutoDelta) * Idelta_To_Bin);
            I2CWriteData(I2C_ATUN_DeltaDuty, Temp);
            PrintAtunDelta();
            break;
        case 4:
            if(g_K >= 10000) {
                encstep *= 10;
            }
            Temp = g_K;
            if(g_EncodeDir) {
                Temp += (10 * encstep);
            } else {
                if(Temp > (10 * encstep)) {
                    Temp -= (10 * encstep);
                } else {
                    Temp = 0;
                }
            }
            if(Temp > 50000) {
                Temp = 50000;
            }
            I2CWriteData(I2C_PID_K, Temp);
            break;
        case 5:
            if(g_Ti >= 10000) {
                encstep *= 10;
            }
            Temp = g_Ti;
            if(g_EncodeDir) {
                Temp += (10 * encstep);
            } else {
                if(Temp > (10 * encstep)) {
                    Temp -= (10 * encstep);
                } else {
                    Temp = 0;
                }
            }
            if(Temp > 50000) {
                Temp = 50000;
            }
            I2CWriteData(I2C_PID_Ti, Temp);
            break;
        case 6:
            if(g_EncodeDir) {
                if(g_I_Bias == Ib_1_6mA) {
                    g_I_Bias = Ib_Auto;
                } else {
                    g_I_Bias++;
                }
            } else {
                if(g_I_Bias == Ib_Auto) {
                    g_I_Bias = Ib_1_6mA;
                } else {
                    g_I_Bias--;
                }
            }
            if(g_I_Bias == Ib_200uA) {
                I2CWriteData(I2C_TEMP_SENSOR_MODE, 0x0000);
                if(g_T_Set > 70) {
                    g_T_Set = 70;
                }
            } else if(g_I_Bias == Ib_1_6mA) {
                I2CWriteData(I2C_TEMP_SENSOR_MODE, 0x0001);
                if(g_T_Set < 65) {
                    g_T_Set = 65;
                }
            }
            Temp = ReturnVset(g_T_Set);
            I2CWriteData(I2C_PID_TARGET, Temp);
            PrintIb();
            PrintTset();
            break;
        case 7:
            Temp = g_B_Const;
            if(g_EncodeDir) {
                Temp += encstep;
            } else {
                Temp -= encstep;
            }
            if (Temp > 4500)
                Temp = 4500;
            if (Temp < 3000)
                Temp = 3000;
            I2CWriteData(I2C_TEMP_B_CONSTANT, Temp);
            break;
        case 8:
            if(g_EncodeDir) {
                if(g_HI_LIMIT <= 32768) {
                    I2CWriteData(I2C_PID_HI_LIMIT, NOEE_HI_LIMIT); 
                }
            } else {
                if(g_HI_LIMIT > 32768) {
                    I2CWriteData(I2C_PID_HI_LIMIT, 32768);
                }
            }
            break;
        }
    }
}

void DTC03Master::WelcomeScreen()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(0, 0);
    //  lcd.print("DTC03 Ver.3.01");
    lcd.print("DTC03A Ver.1.01"); // 3.02 for autotune
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
    lcd.GotoXY(ATUNE_COORD_X, ATUNE_COORD_Y);
    lcd.print(Text_AT);
    lcd.GotoXY(ATUNE_DELTA_COORD_X, ATUNE_DELTA_COORD_Y);
    lcd.print(Text_dA);
    lcd.GotoXY(K_COORD_X, K_COORD_Y);
    lcd.print(Text_K);
    lcd.GotoXY(Ti_COORD_X, Ti_COORD_Y);
    lcd.print(Text_Ti);
    lcd.GotoXY(I_BIAS_COORD_X, I_BIAS_COORD_Y);
    lcd.print(Text_Ib);
    lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
    lcd.print(Text_B);
    lcd.GotoXY(MODE_COORD_X, MODE_COORD_Y);
    lcd.print(Text_M);
}

void DTC03Master::PrintTset()
{
    lcd.GotoXY(TSET_COORD_X2, TSET_COORD_Y);
    if(g_Remote) {
        lcd.SelectFont(fixednums7x15_S);
        if (g_T_Set < 10.000)
            lcd.print("  ");
        else if (g_T_Set < 100.000)
            lcd.print(" ");
        lcd.print(g_T_Set, 3);
        lcd.print(" ");
    } else {
        lcd.SelectFont(Arial_bold_14);
        lcd.print(" REMOTE");
    }
}

void DTC03Master::PrintTact(float tact)
{
    lcd.GotoXY(TACT_COORD_X2, TACT_COORD_Y);
    if(tact > 0) {
        lcd.SelectFont(fixednums7x15_S);
        if (tact < 10.000)
            lcd.print("  ");
        else if (tact < 100.000)
            lcd.print(" ");

        lcd.print(tact, 3);
    } else {
        lcd.SelectFont(Arial_bold_14);
        lcd.print(" ERROR ");
    }
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
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(ILIM_COORD_X2, ILIM_COORD_Y);
    lcd.print(g_I_Print, 2);
    lcd.GotoXY(ILIM_COORD_X2 + (4 * COLUMNPIXEL0507), ILIM_COORD_Y);
    lcd.print("   ");
    
}

void DTC03Master::PrintAtune()
{
    lcd.SelectFont(SystemFont5x7);
    if (p_atunProcess_flag) {
        g_lock_flag = 1;
        p_atunProcess_flag = 0;
        lcd.GotoXY(ATUNE_COORD_X, ATUNE_COORD_Y);
        lcd.print(" ____  ");
        lcd.GotoXY(ATUNE_DELTA_COORD_X, ATUNE_DELTA_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(K_COORD_X, K_COORD_Y);
        lcd.print("|Auto| ");
        lcd.GotoXY(Ti_COORD_X, Ti_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(I_BIAS_COORD_X, I_BIAS_COORD_Y);
        lcd.print("|Tune| ");
        lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
        lcd.print("|____| ");
        lcd.GotoXY(MODE_COORD_X, MODE_COORD_Y);
        lcd.print("       ");
    } else {
        lcd.GotoXY(ATUNE_COORD_X2, ATUNE_COORD_Y);
        if (g_atune_status == 0)
            lcd.print("OFF");
        else
            lcd.print("ON ");
    }
}

void DTC03Master::PrintAtuneResult()
{
    lcd.SelectFont(SystemFont5x7);
    if(!g_Auto_Result) {
        lcd.GotoXY(ATUNE_COORD_X, ATUNE_COORD_Y);
        lcd.print(" ____  ");
        lcd.GotoXY(ATUNE_DELTA_COORD_X, ATUNE_DELTA_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(K_COORD_X, K_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(Ti_COORD_X, Ti_COORD_Y);
        lcd.print("|Done| ");
        lcd.GotoXY(I_BIAS_COORD_X, I_BIAS_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
        lcd.print("|____| ");
        lcd.GotoXY(MODE_COORD_X, MODE_COORD_Y);
        lcd.print("       ");
    } else {
        lcd.GotoXY(ATUNE_COORD_X, ATUNE_COORD_Y);
        lcd.print(" ____  ");
        lcd.GotoXY(ATUNE_DELTA_COORD_X, ATUNE_DELTA_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(K_COORD_X, K_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(Ti_COORD_X, Ti_COORD_Y);
        lcd.print("|Fail| ");
        lcd.GotoXY(I_BIAS_COORD_X, I_BIAS_COORD_Y);
        lcd.print("|    | ");
        lcd.GotoXY(BCONST_COORD_X, BCONST_COORD_Y);
        lcd.print("|____| ");
        lcd.GotoXY(MODE_COORD_X, MODE_COORD_Y);
        lcd.print("       ");
    }
}

void DTC03Master::PrintAtuneDone()
{
    PrintAtuneResult();
    delay(3000);
    g_lock_flag = 0;
    g_atune_status = 0;
    BackGroundPrint();
    PrintNormalAll();
}

void DTC03Master::PrintAtunDelta()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(ATUNE_DELTA_COORD_X2, ATUNE_DELTA_COORD_Y);
    lcd.print(g_I_AutoDelta);
    if(g_I_AutoDelta > 0) {
        lcd.print(" ");
    }
    if(abs(g_I_AutoDelta) < 10) {
        lcd.print("  ");
    } else if(abs(g_I_AutoDelta) < 100) {
        lcd.print(" ");
    }
}

void DTC03Master::PrintK()
{
    if(g_lock_flag) {
        return;
    }
    float K = float(g_K) * 0.01;
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(K_COORD_X2, K_COORD_Y);
    if(K == 0) {
        lcd.print("OFF  ");
    } else {
        if(K < 100) {
            lcd.print(K, 1);
            if (K < 10)
                lcd.print("  ");
        } else {
            lcd.print(K, 0);
            lcd.print("  ");
        }
    }
}

void DTC03Master::PrintTi()
{
    if(g_lock_flag) {
        return;
    }
    float Ti = float(g_Ti) * 0.01;;
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(Ti_COORD_X2, Ti_COORD_Y);
    if(Ti == 0) {
        lcd.print("OFF ");
    } else {
        if(Ti < 100) {
            lcd.print(Ti, 1);
            if (Ti < 10)
                lcd.print(" ");
        } else {
            lcd.print(Ti, 0);
            lcd.print(" ");
        }
    }
}

void DTC03Master::PrintIb()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(I_BIAS_COORD_X2, I_BIAS_COORD_Y);
    if(g_I_Bias == Ib_Auto) {
        lcd.print("Auto");
    } else if(g_I_Bias == Ib_200uA) {
        lcd.print("200u");
    } else if(g_I_Bias == Ib_1_6mA) {
        lcd.print("1.6m");
    }
}

void DTC03Master::PrintB()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(BCONST_COORD_X2, BCONST_COORD_Y);
    lcd.print(g_B_Const);
}

void DTC03Master::PrintM()
{
    lcd.SelectFont(SystemFont5x7);
    lcd.GotoXY(MODE_COORD_X2, MODE_COORD_Y);
    if((g_HI_LIMIT <= PI_Zero) || (g_LO_LIMIT >= PI_Zero)) {
        lcd.print("Heat");
    } else {
        lcd.print("Both");
    }
}

void DTC03Master::PrintNormalAll()
{
    PrintTset();
    PrintIlim();
    PrintAtune();
    PrintAtunDelta();
    PrintK();
    PrintTi();
    PrintIb();
    PrintB();
    PrintM();
    //No need to add print Itec and Vact here, checkstatus() will do this
}

void DTC03Master::CursorState()
{
    if (g_lock_flag) {
        p_PressTime[0] = millis();
        return;
    }
    if (analogRead(ENC_SW) <= HIGHLOWBOUNDRY) {
        p_PressTime[1] += abs(millis() - p_PressTime[0]);
        if(p_PressTime[1] > LONGPRESSTIME) {
            if(g_Remote) {
                if(g_cursorstate == 0) {
                    p_blinkTsetCursorFlag = 0;
                    g_tsetstep = 1.0;
                    PrintTset();
                }
                if (g_cursorstate == 0 || g_cursorstate == 8) {
                    ShowCursor(1);
                } else {
                    ShowCursor(g_cursorstate + 1);
                }
            } else {
                I2CWriteData(I2C_REMOTE, 0x0001);
            }
            p_LongPress = true;
            p_PressTime[1] = 0;
        }
    } else {
        if(g_Remote && !p_LongPress && (p_PressTime[1] > DEBOUNCE_WAIT)) {
            if (g_cursorstate == 0) {
                p_blinkTsetCursorFlag = 1;
                if (g_tsetstep <= 0.001) {
                    g_tsetstep = 1.0;
                } else {
                    g_tsetstep /= 10.0;
                }
            } else {
                if (g_cursorstate == 2 && g_atune_status) {
                    I2CWriteData(I2C_PID_MODE, PID_Autotune);
                }
                g_tsetstep = 1.0;
                ShowCursor(0);
            }
        }
        p_LongPress = false;
        p_PressTime[1] = 0;
    }
    p_PressTime[0] = millis();
}

void DTC03Master::blinkTsetCursor()
{
    if (p_blinkTsetCursorFlag == 1) {
        unsigned int Temp = millis();
        lcd.SelectFont(fixednums7x15_S);
        if (g_tsetstep == 1.0)
            lcd.GotoXY(TSET_COORD_X2 + 2 * COLUMNPIXEL0715, TSET_COORD_Y);
        else if (g_tsetstep == 0.1)
            lcd.GotoXY(TSET_COORD_X2 + 4 * COLUMNPIXEL0715, TSET_COORD_Y);
        else if (g_tsetstep == 0.01)
            lcd.GotoXY(TSET_COORD_X2 + 5 * COLUMNPIXEL0715, TSET_COORD_Y);
        else
            lcd.GotoXY(TSET_COORD_X2 + 6 * COLUMNPIXEL0715, TSET_COORD_Y);

        if (abs(Temp - p_tBlink) > BLINKDELAY) {
            if (p_tBlink_toggle) {
                lcd.print(" ");
            } else {
                PrintTset();
            }
            p_tBlink_toggle = !p_tBlink_toggle;
            p_tBlink = Temp;
        }
    }
}

void DTC03Master::ShowCursor(unsigned char CursorState)
{
    switch (CursorState) {
    case 0:
        lcd.SelectFont(SystemFont5x7);
        switch (g_cursorstate) {
        case 1:
            lcd.GotoXY(ILIM_COORD_X - COLUMNPIXEL0507, ILIM_COORD_Y);
            break;
        case 2:
            lcd.GotoXY(ATUNE_COORD_X - COLUMNPIXEL0507, ATUNE_COORD_Y);
            break;
        case 3:
            lcd.GotoXY(ATUNE_DELTA_COORD_X - COLUMNPIXEL0507, ATUNE_DELTA_COORD_Y);
            break;
        case 4:
            lcd.GotoXY(K_COORD_X - COLUMNPIXEL0507, K_COORD_Y);
            break;
        case 5:
            lcd.GotoXY(Ti_COORD_X - COLUMNPIXEL0507, Ti_COORD_Y);
            break;
        case 6:
            lcd.GotoXY(I_BIAS_COORD_X - COLUMNPIXEL0507, I_BIAS_COORD_Y);
            break;
        case 7:
            lcd.GotoXY(BCONST_COORD_X - COLUMNPIXEL0507, BCONST_COORD_Y);
            break;
        case 8:
            lcd.GotoXY(MODE_COORD_X - COLUMNPIXEL0507, MODE_COORD_Y);
            break;
        }
        lcd.print(" ");
        break;
    case 1:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(ILIM_COORD_X - COLUMNPIXEL0507, ILIM_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(MODE_COORD_X - COLUMNPIXEL0507, MODE_COORD_Y); //
        lcd.print(" ");
        break;
    case 2:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(ATUNE_COORD_X - COLUMNPIXEL0507, ATUNE_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(ILIM_COORD_X - COLUMNPIXEL0507, ILIM_COORD_Y);
        lcd.print(" ");
        break;
    case 3:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(ATUNE_DELTA_COORD_X - COLUMNPIXEL0507, ATUNE_DELTA_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(ATUNE_COORD_X - COLUMNPIXEL0507, ATUNE_COORD_Y);
        lcd.print(" ");
        break;
    case 4:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(K_COORD_X - COLUMNPIXEL0507, K_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(ATUNE_DELTA_COORD_X - COLUMNPIXEL0507, ATUNE_DELTA_COORD_Y);
        lcd.print(" ");
        break;
    case 5:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(Ti_COORD_X - COLUMNPIXEL0507, Ti_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(K_COORD_X - COLUMNPIXEL0507, K_COORD_Y);
        lcd.print(" ");
        break;
    case 6:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(I_BIAS_COORD_X - COLUMNPIXEL0507, I_BIAS_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(Ti_COORD_X - COLUMNPIXEL0507, Ti_COORD_Y);
        lcd.print(" ");
        break;
    case 7:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(BCONST_COORD_X - COLUMNPIXEL0507, BCONST_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(I_BIAS_COORD_X - COLUMNPIXEL0507, I_BIAS_COORD_Y);
        lcd.print(" ");
        break;
    case 8:
        lcd.SelectFont(SystemFont5x7, WHITE);
        lcd.GotoXY(MODE_COORD_X - COLUMNPIXEL0507, MODE_COORD_Y);
        lcd.print(" ");
        lcd.SelectFont(SystemFont5x7);
        lcd.GotoXY(BCONST_COORD_X - COLUMNPIXEL0507, BCONST_COORD_Y);
        lcd.print(" ");
        break;
    }
    g_cursorstate = CursorState;
}

void DTC03Master::Encoder()
{
    unsigned char dt = 0;
    unsigned long tenc = 0;
    bool ENC_B_STATE = false;
    if (!g_lock_flag && g_Remote) {
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
