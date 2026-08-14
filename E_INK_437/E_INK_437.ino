//Convert Image to cpp:https://javl.github.io/image2cpp/
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
/***EINK  ESP32S3
int BUSY_Pin = 8; 
int RST_Pin = 9; 
int DC_Pin = 10; 
int CS_Pin = 7; 
int SCK_Pin = 12; 
int MOSI_Pin = 11; 
***/
#include "qrCode.h"
#include "GxEPD2_437.h"
#include <Fonts/FreeSansBold9pt7b.h>
int BUSY_PIN = 0; 
int RST_PIN = 3; 
int DC_PIN = 2; 
int CS_PIN = 1; 
GxEPD2_437 display(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN);

void setup()
{
  Serial.begin(115200);
  display.init(115200);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_WHITE);
  display.setFont(&FreeSansBold9pt7b);
  //display.invertDisplay(1);
  display.fillRect(0, 0, 152, 40,  GxEPD_BLACK);
  display.setCursor(25, 25);
  display.print("PG GROUP");
  //display.drawLine(0,0,176,480,GxEPD_BLACK);
  //Draw Mode= Horizontal - 1 bit per pixel
  display.drawBitmap(0, 480-152, epd_bitmap_, 152, 152, GxEPD_RED);
  //display.draeeepd_bitmap_
  display.display();     // push buffers to the panel + refresh
  display.hibernate();   // required: puts panel into deep sleep afterwards
}

void loop()
{
  // Nothing here — this panel only needs power during a refresh.
  // Put the MCU to sleep or handle other tasks as needed.
}
