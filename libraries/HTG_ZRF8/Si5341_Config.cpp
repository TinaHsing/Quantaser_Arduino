/*==============================================================================
  * @file           : Si5341_Config.c
  * @brief          : Si5341_Config program body.
  ==============================================================================
  * @description	: Si5341_Config is a program which configure Si5341
  * 				  clock generator.
  *
  ==============================================================================
  * @attention
  *
  * Copyright (c) 2023 Quantaser.
  * All rights reserved.
  *
  ==============================================================================*/

#include "Si5341_Config.h"
#include "Si5341_Reg.h"

/*=========================================================================================================*/
/*  Variables                                                                                              */
/*=========================================================================================================*/

/*=========================================================================================================*/
/*  Function                                                                                               */
/*=========================================================================================================*/

/**
  * @brief  Si5341 class initialize.
  * @retval None
  */
Si5341::Si5341()
{
}

/**
  * @brief  Si5341 set frequency.
  * @retval Result
  */
int Si5341::SetFreq(void)
{
	int Status = XST_SUCCESS;
	Si5341_Page = 0x00;
	Status = PreSet();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = SetAllReg();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = PostSet();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return Status;
}

/**
  * @brief  Preprocessing before set Si5341 register.
  * @retval Result
  */
int Si5341::PreSet(void)
{
	int Status = XST_SUCCESS;
	for(int i=0; i<SI5341_PRE_NUM; i++) {
		unsigned char Page = (unsigned char)(Si5341_Preamble[i].address >> 8);
		unsigned char Data[] = {(unsigned char)(Si5341_Preamble[i].address),
												Si5341_Preamble[i].value};
		if(Si5341_Page != Page) {
			unsigned char PageData[] = {0x01, Page};
			Status = Write(I2C_SI5341_ADDR, &PageData[0], 2);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Si5341_Page = Page;
		}
		Status = Write(I2C_SI5341_ADDR, &Data[0], 2);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}
	delay(SI5341_PRESET_DELAY);
	return Status;
}

/**
  * @brief  Set Si5341 all register.
  * @retval Result
  */
int Si5341::SetAllReg(void)
{
	int Status = XST_SUCCESS;
	for(int i=0; i<SI5341_REG_NUM; i++) {
		unsigned char Page = (unsigned char)(Si5341_Register[i].address >> 8);
		unsigned char Data[] = {(unsigned char)(Si5341_Register[i].address),
												Si5341_Register[i].value};
		if(Si5341_Page != Page) {
			unsigned char PageData[] = {0x01, Page};
			Status = Write(I2C_SI5341_ADDR, &PageData[0], 2);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Si5341_Page = Page;
		}
		Status = Write(I2C_SI5341_ADDR, &Data[0], 2);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}
	return Status;
}

/**
  * @brief  Postprocessing before set Si5341 register.
  * @retval Result
  */
int Si5341::PostSet(void)
{
	int Status = XST_SUCCESS;
	for(int i=0; i<SI5341_POST_NUM; i++) {
		unsigned char Page = (unsigned char)(Si5341_Postambl[i].address >> 8);
		unsigned char Data[] = {(unsigned char)(Si5341_Postambl[i].address),
												Si5341_Postambl[i].value};
		if(Si5341_Page != Page) {
			unsigned char PageData[] = {0x01, Page};
			Status = Write(I2C_SI5341_ADDR, &PageData[0], 2);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Si5341_Page = Page;
		}
		Status = Write(I2C_SI5341_ADDR, &Data[0], 2);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}
	return Status;
}

/**
  * @brief  I2C data write.
  * @param  Address : Indicate I2C device address(7-bit).
  * @param  Data : Indicate data buffer pointer.
  * @param  Length : Indicate transfer length.
  * @retval Result
  */
int Si5341::Write(unsigned char Address, unsigned char* Data, unsigned int Length)
{
	int Status = XST_SUCCESS;
	unsigned char Buffer[Length];
	for(int i=0; i<Length; i++) {
		Buffer[i] = Data[i];
	}
	Wire1.beginTransmission(Address);
    Wire1.write(Buffer, Length);
    Wire1.endTransmission();
    delayMicroseconds(I2C_SLEEP_US);
	return Status;
}

/**
  * @brief  I2C data read.
  * @param  Address : Indicate I2C device address(7-bit).
  * @param  Data : Indicate data buffer pointer.
  * @param  Length : Indicate transfer length.
  * @retval Result
  */
int Si5341::Read(unsigned char Address, unsigned char* Data, unsigned int Length)
{
	int Status = XST_SUCCESS;
    Wire1.requestFrom(Address, Length);
    if(Wire1.available() != Length) {
        return XST_FAILURE;
    }
    for(int i=0; i<Length; i++) {
        Data[i] = Wire1.read();
    }
    delayMicroseconds(I2C_SLEEP_US);
	return Status;
}
