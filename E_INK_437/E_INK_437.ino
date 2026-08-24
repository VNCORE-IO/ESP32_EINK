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
///ESP32C3
#define BUSY_Pin  0
#define RST_Pin  3
#define DC_Pin  2
#define CS_Pin  1
// Define SPI pins
#define SPI_MISO -1
#define SPI_MOSI 6
#define SPI_SCK  4
#define SPI_CS   -1

int BUSY_PIN = 0; 
int RST_PIN = 3; 
int DC_PIN = 2; 
int CS_PIN = 1; 
***/
#include "qrCode.h"
#include <GxEPD.h>
#include "GxEPD_437.h"
#include "qrCode.h"

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>


// for SPI pin definitions see e.g.:
// C:\Users\xxx\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.6.21\variants\standard\pins_arduino.h
#define EPD_CS 0
#define EPD_DC 1
#define EPD_RST 2
#define EPD_BUSY 10
// // ESP32C3 //
#define SDI_PIN 3
#define SCK_PIN 4
#define HAS_RED_COLOR
SPIClass customSPI(FSPI);
GxIO_Class io(customSPI, /*CS=*/EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RST);
GxEPD_Class display(io, /*RST=*/EPD_RST, /*BUSY=*/EPD_BUSY);
void showFontCallback() {
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);
  display.setCursor(300, 100);
  display.println("PG");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  customSPI.begin(SCK_PIN, -1, SDI_PIN, EPD_CS);
  display.init(115200);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(epd_bitmap_, 0, 0, 150, 150, GxEPD_RED);
  display.update();
  delay(2000);
  // display.drawBitmap(epd_bitmap_, 150, 0,150,150,GxEPD_BLACK);
  // display.update();
  // delay(2000);
  display.powerDown();
  display.drawBitmap(epd_bitmap_, 300, 0, 150, 150, GxEPD_BLACK);
  display.updateWindow(300, 0, 180, 176);
  delay(2000);
  display.powerDown();
}
void loop() {
}
