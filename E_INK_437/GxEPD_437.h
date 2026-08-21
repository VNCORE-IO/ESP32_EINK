#ifndef _GxEPD_437_H_
#define _GxEPD_437_H_
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#define WIDTH 176
#define HEIGHT 480
// // ESP32C3 //
#define SDI_PIN 3
#define SCK_PIN 4
#define RST_PIN 2
#define DC_PIN 1
#define CS_PIN 0
#define BUSY_PIN 10

#define EPD_MOSI_0  digitalWrite(SDI_PIN,LOW)
#define EPD_MOSI_1  digitalWrite(SDI_PIN,HIGH) 
#define EPD_CLK_0 digitalWrite(SCK_PIN,LOW)
#define EPD_CLK_1 digitalWrite(SCK_PIN,HIGH)
#define EPD_CS_0 digitalWrite(CS_PIN,LOW)
#define EPD_CS_1 digitalWrite(CS_PIN,HIGH)
#define EPD_DC_0  digitalWrite(DC_PIN,LOW)
#define EPD_DC_1  digitalWrite(DC_PIN,HIGH)
#define EPD_RST_0 digitalWrite(RST_PIN,LOW)
#define EPD_RST_1 digitalWrite(RST_PIN,HIGH)
#define isEPD_BUSY digitalRead(BUSY_PIN)

#define GxEPD_BLACK 0x0000
#define GxEPD_WHITE 0xFFFF
#define GxEPD_RED   0xF800

class GxEPD_437 : public Adafruit_GFX
{
  public:
    static const bool hasColor = true;
    static const bool hasPartialUpdate = false; // full refresh only, as with Z39c

    GxEPD_437(int8_t cs, int8_t dc, int8_t rst, int8_t busy);

    // Initializes pins, SPI and the panel. Call once in setup().
    void init(uint32_t serial_diag_bitrate = 0);
    void fillScreen(uint16_t color) override;
    void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    // Draws a bitmap at (x, y) into the in-memory buffer. `bitmap` must be
    // PROGMEM data in "Horizontal - 1 bit per pixel" format (MSB first,
    // row width padded to a whole byte). `color` selects which plane the
    // set bits (1) are drawn into: GxEPD_BLACK or GxEPD_RED.
    // Call display() afterwards to push it to the panel.
    void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    // Fast path for full-screen images exported at exactly 176x480.
    // blackBitmap / redBitmap must each be _bufSize (10560) bytes, PROGMEM,
    // "Horizontal - 1 bit per pixel" format, 1 = white/no-red, 0 = black/red.
    // Pass nullptr for either plane to leave it untouched.
    void writeFullScreenBitmap(const uint8_t* blackBitmap, const uint8_t* redBitmap);

    // Pushes both buffers (black + red) to the panel and triggers refresh.
    void display();

    // Puts the panel into deep sleep. Call after display(); required by
    // the datasheet — do not skip this.
    void hibernate();

  private:
    int8_t _cs, _dc, _rst, _busy;
    static const uint16_t _bufSize = (WIDTH / 8) * HEIGHT; // 22 * 480 = 10560 bytes
    uint8_t _black[_bufSize];
    uint8_t _red[_bufSize];

    void _writeCmd(uint8_t c);
    void _writeData(uint8_t d);
    void _waitBusy();
    void _hwReset();
    void _initDisplay();

    void _delay_us(unsigned int xus);
    void _delay_xms(unsigned long xms);   
    void _SPI_Delay(unsigned char xrate);
    void _SPI_Write(unsigned char value) ;
};

#endif
