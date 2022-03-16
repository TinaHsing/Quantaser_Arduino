#ifndef DTC03_MS_H
#define DTC03_MS_H

#include <avr/io.h>
#include <avr/pgmspace.h>//
// ==========DTC03A_P01 I2C slave address ======
#define SLAVE_ADDR 		0x5A
// =========receive Events Command and Address======
#define I2C_DEVICE_STATE		0x0000
#define I2C_FW_VERSION			0x0001
#define I2C_REMOTE				0x0010
#define I2C_IO_STATE			0x0100
#define I2C_PID_MODE			0x0200
#define I2C_PID_TARGET			0x0201
#define I2C_PID_K				0x0202
#define I2C_PID_Ti				0x0203
#define I2C_PID_Td				0x0204
#define I2C_PID_HI_LIMIT		0x0205
#define I2C_PID_LO_LIMIT		0x0206
#define I2C_V_Limit				0x0300
#define I2C_I_Limit				0x0301
#define I2C_V_TEC				0x0304
#define I2C_I_TEC				0x0305
#define I2C_TEMP_SENSOR_EN		0x0400
#define I2C_TEMP_SENSOR_MODE	0x0401
#define I2C_TEMP_DATA			0x0402
#define I2C_TEMP_AVERAGE_DATA	0x0403
#define I2C_TEMP_B_CONSTANT		0x0404
#define I2C_ATUN_TYPE			0x0500
#define I2C_ATUN_DeltaDuty		0x0501
#define I2C_ATUN_Result			0x0502

//=========request Events Mask ============
#define REQMSK_ENSTATE 		0x80
#define REQMSK_SENSTYPE		0x40
#define REQMSK_BUPPER		0x07

#define REQMSK_ITECU		0x03
#define REQMSK_ITECSIGN		0x04
#define REQMSK_ERR1		 	0x10
#define REQMSK_ERR2			0x20
#define REQMSK_WAKEUP   	0x40			//Tina 2017/2/15
#define REQMSK_ATUNE_RUNTIMEERR	0X01
#define REQMSK_ATUNE_DBR 		0X02
#define REQMSK_ATUNE_DONE 		0X04
#define REQMSK_ATUNE_STATUS		0x01

//==========IO state============
#define IO_MCU_READY		0x01 << 0
#define IO_PWM_EN			0x01 << 1
#define IO_TEC_EN			0x01 << 2
#define IO_SENSOR_PWR_EN	0x01 << 3
#define IO_SENSOR_I_MODE	0x01 << 4
#define IO_I_LIM_MCU_n		0x01 << 5
#define IO_V_LIM_MCU_n		0x01 << 6
#define IO_SENSOR_FAULT_n	0x01 << 7
#define IO_OVER_COOL_n		0x01 << 8

//==========Type Enum============
typedef enum {
	PID_Normal = 0x00U,
	PID_Autotune = 0x01U,
	PID_Hold = 0x02U,
} PID_Mode;

typedef enum {
	Autotune_P = 0x00U,
	Autotune_PI = 0x01U,
	Autotune_PID = 0x02U
} Autotune_Type;

const PROGMEM unsigned char CRC_8_CCITT_Table[] = {
	0x00, 0x1B, 0x36, 0x2D, 0x6C, 0x77, 0x5A, 0x41, 0xD8, 0xC3, 0xEE, 0xF5, 0xB4, 0xAF, 0x82, 0x99,
	0xD3, 0xC8, 0xE5, 0xFE, 0xBF, 0xA4, 0x89, 0x92, 0x0B, 0x10, 0x3D, 0x26, 0x67, 0x7C, 0x51, 0x4A,
	0xC5, 0xDE, 0xF3, 0xE8, 0xA9, 0xB2, 0x9F, 0x84, 0x1D, 0x06, 0x2B, 0x30, 0x71, 0x6A, 0x47, 0x5C,
	0x16, 0x0D, 0x20, 0x3B, 0x7A, 0x61, 0x4C, 0x57, 0xCE, 0xD5, 0xF8, 0xE3, 0xA2, 0xB9, 0x94, 0x8F,
	0xE9, 0xF2, 0xDF, 0xC4, 0x85, 0x9E, 0xB3, 0xA8, 0x31, 0x2A, 0x07, 0x1C, 0x5D, 0x46, 0x6B, 0x70,
	0x3A, 0x21, 0x0C, 0x17, 0x56, 0x4D, 0x60, 0x7B, 0xE2, 0xF9, 0xD4, 0xCF, 0x8E, 0x95, 0xB8, 0xA3,
	0x2C, 0x37, 0x1A, 0x01, 0x40, 0x5B, 0x76, 0x6D, 0xF4, 0xEF, 0xC2, 0xD9, 0x98, 0x83, 0xAE, 0xB5,
	0xFF, 0xE4, 0xC9, 0xD2, 0x93, 0x88, 0xA5, 0xBE, 0x27, 0x3C, 0x11, 0x0A, 0x4B, 0x50, 0x7D, 0x66,
	0xB1, 0xAA, 0x87, 0x9C, 0xDD, 0xC6, 0xEB, 0xF0, 0x69, 0x72, 0x5F, 0x44, 0x05, 0x1E, 0x33, 0x28,
	0x62, 0x79, 0x54, 0x4F, 0x0E, 0x15, 0x38, 0x23, 0xBA, 0xA1, 0x8C, 0x97, 0xD6, 0xCD, 0xE0, 0xFB,
	0x74, 0x6F, 0x42, 0x59, 0x18, 0x03, 0x2E, 0x35, 0xAC, 0xB7, 0x9A, 0x81, 0xC0, 0xDB, 0xF6, 0xED,
	0xA7, 0xBC, 0x91, 0x8A, 0xCB, 0xD0, 0xFD, 0xE6, 0x7F, 0x64, 0x49, 0x52, 0x13, 0x08, 0x25, 0x3E,
	0x58, 0x43, 0x6E, 0x75, 0x34, 0x2F, 0x02, 0x19, 0x80, 0x9B, 0xB6, 0xAD, 0xEC, 0xF7, 0xDA, 0xC1,
	0x8B, 0x90, 0xBD, 0xA6, 0xE7, 0xFC, 0xD1, 0xCA, 0x53, 0x48, 0x65, 0x7E, 0x3F, 0x24, 0x09, 0x12,
	0x9D, 0x86, 0xAB, 0xB0, 0xF1, 0xEA, 0xC7, 0xDC, 0x45, 0x5E, 0x73, 0x68, 0x29, 0x32, 0x1F, 0x04,
	0x4E, 0x55, 0x78, 0x63, 0x22, 0x39, 0x14, 0x0F, 0x96, 0x8D, 0xA0, 0xBB, 0xFA, 0xE1, 0xCC, 0xD7,
};

#endif
