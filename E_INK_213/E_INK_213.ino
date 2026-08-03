//Convert Image to cpp:https://javl.github.io/image2cpp/
/* PIN CONNECT */
// EINK  ESP32
// 1:	GND	  GND
// 2:	3.3V	3.3
// 3:	SCL	  18
// 4:	MOSI	23/Users/pg/Library/CloudStorage/SynologyDrive-0ther/PG/ESP32_E_INK/E_INK_26/E_INK_26.i
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
// int BUSY_Pin = 8; 
// int RST_Pin = 9; 
// int DC_Pin = 10; 
// int CS_Pin = 7; 
////EINK  ESP32C3
#define BUSY_Pin 0
#define RST_Pin 3
#define DC_Pin 2
#define CS_Pin 1
// Define SPI pins
#define SPI_MISO -1
#define SPI_MOSI 6
#define SPI_SCK 4
#define SPI_CS -1
#include "qrCode.h"
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
 #include "monthly_calendar.h" 
// // select the display class and display driver class in the following file (new style):
// #include "GxEPD2_display_selection_new_style.h"
// // or select the display constructor line in one of the following files (old style):
// #include "GxEPD2_display_selection.h"
// #include "GxEPD2_display_selection_added.h"

//3Color
GxEPD2_3C<GxEPD2_213c, GxEPD2_213c::HEIGHT> display(GxEPD2_213c(/*CS=*/ CS_Pin, /*DC=*/DC_Pin, /*RST=*/RST_Pin, /*BUSY=*/BUSY_Pin));  // GDEW0213Z16 104x212, UC8151 (IL0373)
//GxEPD2_3C<GxEPD2_213_Z19c, GxEPD2_213_Z19c::HEIGHT> display(GxEPD2_213_Z19c(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0213Z19 104x212, UC8151D
//GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEY0213Z98 122x250, SSD1680

const char PGGroup[] = "PGGroup";
const char lineB[] = "Welcome...!";
void drawQR(){
  display.setPartialWindow(110, 2, 100, 100);
  display.firstPage();
  do{
    display.drawInvertedBitmap(110, 2, epd_bitmap_, 100, 100, GxEPD_RED);
  }while(display.nextPage());
}
void showPG() {
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.setTextColor(GxEPD_WHITE);
  display.setFont(&FreeSerifBold12pt7b);
  display.getTextBounds(PGGroup, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  //display.setPartialWindow(0, 0, tbw + 5, 1.5 * tbh);
  display.firstPage();
  do {
    display.fillRect(0, 0, tbw + 5, 1.5 * tbh, GxEPD_RED);
    display.setCursor(0, tbh);
    display.print(PGGroup);
    display.drawInvertedBitmap(110, 2, epd_bitmap_, 100, 98, GxEPD_BLACK);
  } while (display.nextPage());
}
void fillDisplay(){
  display.setFullWindow();
  display.firstPage();
  do {
    display.startWrite();
    display.writeFillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
    display.endWrite();
  } while (display.nextPage());
}
void showCalendar(){
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    showCalendar(&display);
  }
  while (display.nextPage());
}
void setup() {
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false);  // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  display.setRotation(1);
  fillDisplay();
  //drawQR();
  //showPG();
  showCalendar();
  display.hibernate();
}
void loop(){};
