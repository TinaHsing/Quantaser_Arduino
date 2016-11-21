#include <avr/io.h>

// ==========DTC03_P05 I2C slave address ======
#define DTC03P05 		0x07
// =========receive Events Command and Address======
#define I2C_COM_INIT 	0x11
#define I2C_COM_CTR 	0x12
#define I2C_COM_VSET 	0x13
#define I2C_COM_KIINDEX	0x14
#define I2C_COM_VACT	0x15
#define I2C_COM_ITEC_ER	0x16
#define I2C_COM_VBEH	0x17
#define I2C_COM_VBEC	0x18
#define I2C_COM_FBC		0x19
#define	I2C_COM_VMOD	0x1A
#define I2C_COM_KI 		0x1B

//=========request Events Mask ============
#define REQMSK_ENSTATE 		0x80
#define REQMSK_SENSTYPE		0x40
#define REQMSK_BUPPER		0x07

#define REQMSK_ITECU		0x03
#define REQMSK_ITECSIGN		0x04
#define REQMSK_ERR1		 	0x10
#define REQMSK_ERR2			0x20			


// first colume ki, second colume ls
