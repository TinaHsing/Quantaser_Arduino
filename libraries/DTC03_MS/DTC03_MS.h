#ifndef DTC03_MS_H
#define DTC03_MS_H

#include <avr/io.h>
#include <avr/pgmspace.h>//
// ==========DTC03_P05 I2C slave address ======
#define DTC03P05 		0x07
// =========receive Events Command and Address======
#define I2C_COM_INIT 	0x11
#define I2C_COM_CTR 	0x12
#define I2C_COM_KI	    0x13
#define I2C_COM_VSET 	0x14
#define I2C_COM_R1R2	0x15
#define I2C_COM_FBC		0x16
#define I2C_COM_OTP  	0x17
#define I2C_COM_ITEC_ER	0x18
#define I2C_COM_VACT	0x19

#define I2C_COM_PCB		0X1A

#define	I2C_COM_VMOD	0x1B // mod offset, not use in Scan DTC03 case
#define I2C_COM_TEST	0x1C


//=========request Events Mask ============
#define REQMSK_ENSTATE 		0x80 //B1000 0000
#define REQMSK_SENSTYPE		0x40 //B0100 0000
#define REQMSK_BUPPER		0x07 //B0111

#define REQMSK_ITECU		0x03 //B0011
#define REQMSK_ITECSIGN		0x04 //B0100
#define REQMSK_ERR1		 	0x10 //B0001 0000
#define REQMSK_ERR2			0x20 //B0010 0000			


// first colume ki, second colume ls
/*matrix modified on 168us 20161103*/
const PROGMEM unsigned char kilstable[] =
{ 
0,   0, //dummy
0,   0, //0
16,	220, //0.05
17,	220,//0.1
18,	220,//0.2
18,	147,//0.3
19,	220,//0.4
19,	176,//0.5
19,	147,//0.6
20,	251,//0.7
20,	220,//0.8
20,	196,//0.9
20,	176,//1
20,	160,//1.1
20,	147,//1.2
20,	135,//1.3
21,	252,//1.4
21,	235,//1.5
21,	220,//1.6
21,	207,//1.7
21,	196,//1.8
21,	185,//1.9
21,	176,//2
21,	141,//2.5
22,	235,//3
22,	201,//3.5
22,	176,//4
22,	157,//4.5
22,	141,//5
23,	235,//6
23,	201,//7
23,	176,//8
23,	157,//9
23,	141,//10
24,	235,//12
24,	201,//14
24,	176,//16
24,	157,//18
24,	141,//20
25,	225,//25
25,	188,//30
25,	161,//35
25,	141,//40
26,	251,//45
26,	225,//50
26,	204,//55
26,	187,//60
26,	173,//65
26,	161,//70
26,	150,//75
26,	141,//80

};


const PROGMEM unsigned int timeconst[] =
{
  0, //index 0, dummy index
  0, //index 1, time const=0, OFF
  5, //index 2, time const=0.05
  10, //index 3, time const=0.1
  20,//index 4
  30,
  40,
  50,
  60,
  70,
  80,
  90,
  100,
  110,
  120,
  130,
  140,
  150,
  160,
  170,
  180,
  190,
  200,
  250,
  300,
  350,
  400,
  450,
  500,
  600,
  700,
  800,
  900, //index 32, time const=9.0
  1000,
  1200,
  1400,
  1600,
  1800,
  2000,
  2500,
  3000,
  3500,
  4000,
  4500,
  5000,
  5500,
  6000,
  6500,
  7000,
  7500,
  8000, //index 50, time const=80
};

#endif
