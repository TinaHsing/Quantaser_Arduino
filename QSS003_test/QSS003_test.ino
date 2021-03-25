#include <Wire.h>
#include <LTC2615.h>
#include <SPI.h>
#include <LTC1865.h>
#include <SD.h>
#include <C12880_V01.h>
#include <EEPROM.h>
#include <QSS003_P02.h>


QSS003 board;
C12880 spectro;
unsigned int iFileNum=1;
File writeFile;
bool SD_MODE = 0;
char FileName[12];

void setup() {
 bool bFileFull = true;
 
 Serial.begin(115200);
 board.ReadEEPROM();

 board.printVar();
 board.Initialize();
 board.currentOut(LEDA, 0);
 board.currentOut(LEDB, 0);
 board.currentOut(LEDC, 0);      

#if 0
 SD_MODE = digitalRead(MODE_SD_USB);
 if(SD_MODE)
   {
     while (!SD.begin(SD_CS))	// If SD card is not ready hold the program and turn on LED until SD card is ready	
        digitalWrite(LEDM, HIGH);       
     digitalWrite(LEDM, LOW); 	  
     // scan FileName in SD card to test if SD card is full
     for(int f=1; f <=MAXFILE; f++)
      {
      	sprintf(FileName,"data%03d.hex",f);
        if(!SD.exists(FileName))
         {
             iFileNum = f+1;
             bFileFull = false;    
             break;
         }
    	}
    // if SD card is full, turn on LED forever
     while(bFileFull)
	    digitalWrite(LEDM,HIGH);
     writeFile = SD.open(FileName,FILE_WRITE);
   }
  else
#endif
  {
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
  }
 spectro.SpectroInit(CLKA, STA, CLKB, STB, ADCCONV, ADC_CHA);
}

void loop() {
  unsigned long taa, tbb;
  bool run = board.checkParam();
  board.SaveEEPROM();

  while (run)  //(digitalRead(SWM))
  {
      run = 0;
      digitalWrite(LEDM, HIGH);
      taa=board.us2cyc(board.gul_ta);
      tbb=board.us2cyc(board.gul_tb);
      // board.currentOut(LEDA, board.gui_i1);
      // board.currentOut(LEDB, board.gui_i2);
      // board.currentOut(LEDC, board.gui_i3);
      if(board.gui_rp<1) 
      {
        board.gui_rp =1;
      }
//      delayMicroseconds(board.gul_tw);      
      //delay(board.gul_tw);      
      for (unsigned int i =0; i <board.gui_rp; i++)
      {
        Serial.print("\nRun");
        Serial.println(i);
        spectro.RunDevice(taa, tbb, SD_MODE, writeFile);
      }
      // board.currentOut(LEDA, 0);
      // board.currentOut(LEDB, 0);
      // board.currentOut(LEDC, 0);      
      digitalWrite(LEDM, LOW);
#if 0
      if(SD_MODE)
      {
         writeFile.close();
         iFileNum++;
         sprintf(FileName,"data%03d.hex",iFileNum);
         writeFile = SD.open(FileName,FILE_WRITE);
      }
#endif
      delay(2000);     
  }
  

}
