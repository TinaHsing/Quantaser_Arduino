#ifndef DTC03_MS_H
#define DTC03_MS_H

#include <avr/io.h>
#include <avr/pgmspace.h>//
// ==========DTC03_P05 I2C slave address ======
#define DTC03P05 		0x07
// =========receive Events Command and Address======
//Write
#define I2C_COM_INIT 	0x11
#define I2C_COM_CTR 	I2C_COM_INIT+1
#define I2C_COM_KI	    I2C_COM_CTR+1
#define I2C_COM_VSET 	I2C_COM_KI+1
#define I2C_COM_R1R2	I2C_COM_VSET+1
#define I2C_COM_FBC		I2C_COM_R1R2+1
#define I2C_COM_OTP  	I2C_COM_FBC+1
#define	I2C_COM_VMOD	I2C_COM_OTP+1
#define	I2C_COM_TPIDOFF I2C_COM_VMOD+1
#define	I2C_COM_RMEAS   I2C_COM_TPIDOFF+1
#define I2C_COM_WAKEUP  I2C_COM_RMEAS+1 
#define	I2C_COM_ATUN    I2C_COM_WAKEUP+1

//Read
#define I2C_COM_ITEC_ER	I2C_COM_ATUN+1
#define I2C_COM_VACT	I2C_COM_ITEC_ER+1
#define I2C_COM_PCB		I2C_COM_VACT+1


#define I2C_COM_TEST1	I2C_COM_PCB+1
#define I2C_COM_TEST2 	I2C_COM_TEST1+1

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
#define REQMSK_ATUNE_DONE 		0X03

// first colume ki, second colume ls
const PROGMEM unsigned char kilstable420[] =
{ 
0,   0, //dummy		index
0,   0, //0			1
14,	138, //0.05		2
15,	138,//0.1		3
16,	138,//0.2		4
17,	184,//0.3		5
17,	138,//0.4		6
18,	220,//0.5		7
18,	184,//0.6		8
18,	157,//0.7		9
18,	138,//0.8		10
19,	245,//0.9		11
19,	220,//1			12
19,	200,//1.1		13
19,	184,//1.2		14
19,	169,//1.3		15
19,	157,//1.4		16
19,	147,//1.5		17
19,	138,//1.6		18
19,	130,//1.7		19
20,	245,//1.8		20
20,	232,//1.9		21
20,	220,//2			22
20,	176,//2.5		23
20,	147,//3			24
21,	252,//3.5		25
21,	220,//4
21,	196,//4.5
21,	176,//5
21,	147,//6
22,	252,//7
22,	220,//8
22,	196,//9
22,	176,//10
22,	147,//12
23,	252,//14
23,	220,//16
23,	196,//18
23,	176,//20
23,	141,//25
24,	235,//30
24,	201,//35
24,	176,//40
24,	157,//45
24,	141,//50
24,	128,//55
25,	235,//60
25,	217,//65
25,	201,//70
25,	188,//75
25,	176,//80
};
//const PROGMEM unsigned char kilstable280[] =
//{ 
//0,   0, //dummy		index
//0,   0, //0			1
//15,	184, //0.05		2
//16,	184,//0.1		3
//17,	184,//0.2		4
//18,	245,//0.3		5
//18,	184,//0.4		6
//18,	147,//0.5		7
//19,	245,//0.6		8
//19,	210,//0.7		9
//19,	184,//0.8		10
//19,	163,//0.9		11
//19,	147,//1			12
//19,	133,//1.1		13
//19,	122,//1.2		14
//19,	113,//1.3		15
//19,	105,//1.4		16
//20,	196,//1.5		17
//20,	184,//1.6		18
//20,	173,//1.7		19
//20,	163,//1.8		20
//20,	155,//1.9		21
//20,	147,//2			22
//20,	117,//2.5		23
//21,	196,//3			24
//21,	168,//3.5		25
//21,	147,//4
//21,	130,//4.5
//22,	235,//5
//22,	196,//6
//22,	168,//7
//22,	147,//8
//22,	130,//9
//23,	235,//10
//23,	196,//12
//23,	168,//14
//23,	147,//16
//23,	130,//18
//24,	235,//20
//24,	188,//25
//24,	157,//30
//24,	134,//35
//25,	235,//40
//25,	209,//45
//25,	188,//50
//25,	171,//55
//25,	157,//60
//25,	145,//65
//25,	134,//70
//26,	251,//75
//26,	235,//80
//};
const PROGMEM unsigned char kilstable230[] =
{ 
0,   0, //dummy		index
0,   0, //0			1
15,	151, //0.05		2
16,	151,//0.1		3
17,	151,//0.2		4
18,	201,//0.3		5
18,	151,//0.4		6
19,	241,//0.5		7
19,	201,//0.6		8
19,	172,//0.7		9
19,	151,//0.8		10
19,	134,//0.9		11
20,	241,//1			12
20,	219,//1.1		13
20,	201,//1.2		14
20,	186,//1.3		15
20,	172,//1.4		16
20,	161,//1.5		17
20,	151,//1.6		18
20,	142,//1.7		19
20,	134,//1.8		20
21,	254,//1.9		21
21,	241,//2			22
21,	193,//2.5		23
21,	161,//3			24
21,	138,//3.5		25
22,	241,//4
22,	214,//4.5
22,	193,//5
22,	161,//6
22,	138,//7
23,	241,//8
23,	214,//9
23,	193,//10
23,	161,//12
23,	138,//14
24,	241,//16
24,	214,//18
24,	193,//20
24,	154,//25
24,	129,//30
25,	221,//35
25,	193,//40
25,	172,//45
25,	154,//50
25,	140,//55
25,	129,//60
26,	237,//65
26,	221,//70
26,	206,//75
26,	193,//80
};
const PROGMEM unsigned char kilstable168[] =
{
0,   0,//dummy 
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
