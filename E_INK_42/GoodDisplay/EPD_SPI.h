#ifndef _EPD_SPI_H_
#define _EPD_SPI_H_
#include "Arduino.h"
#include <SPI.h>
//IO settings
/* PIN CONNECT */
// EINK  ESP32
// 1:	GND	  GND
// 2:	3.3V	3.3
// 3:	SCL	  18
// 4:	MOSI	23
// 5:	RESET	16
// 6:	DC	  17
// 7:	CS	  5
// 8:	BUSY	4
//#include <GFX.h>
/*** EINK  ESP32S3
int BUSY_Pin = 8; 
int RST_Pin = 9; 
int DC_Pin = 10; 
int CS_Pin = 7; 
int SCK_Pin = 12; 
int SDI_Pin = 11; 
***/
// #define BUSY_Pin  8
// #define RST_Pin  9
// #define DC_Pin  10
// #define CS_Pin  7
// // Define SPI pins
// #define SPI_MISO -1
// #define SPI_MOSI 11
// #define SPI_SCK  12
// #define SPI_CS   -1
/*** EINK  ESP32C3---/
int BUSY_Pin = 0; 
int RST_Pin = 3; 
int DC_Pin = 2; 
int CS_Pin = 1; 
int SCK_Pin = 4; 
int SDI_Pin = 6; 
***/
#define BUSY_Pin  0
#define RST_Pin  3
#define DC_Pin  2
#define CS_Pin  1
// Define SPI pins
#define SPI_MISO -1
#define SPI_MOSI 6
#define SPI_SCK  4
#define SPI_CS   -1

#define isEPD_W21_BUSY digitalRead(BUSY_Pin)  //BUSY
#define EPD_W21_RST_0 digitalWrite(RST_Pin,LOW)  //RES
#define EPD_W21_RST_1 digitalWrite(RST_Pin,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW) //DC
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW) //CS
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)


void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);
unsigned char EPD_W21_ReadDATA(void);

#endif 
