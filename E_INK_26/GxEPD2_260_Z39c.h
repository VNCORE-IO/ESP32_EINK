#ifndef _GxEPD2_260_Z39c_H_
#define _GxEPD2_260_Z39c_H_

// Custom Adafruit_GFX-based driver for the GDEW026Z39 e-paper panel
// (2.6", 152 x 296, 3-color black/white/red, UC8151 / IL0373 controller).
//
// GxEPD2 does NOT ship a native class for this exact panel (the closest
// match, GxEPD2_266c, is for a different panel/controller: GDEY0266Z90 /
// SSD1680, and will NOT work correctly with this hardware).
//
// This class follows the same public-API style as GxEPD2 (Adafruit_GFX
// based, init()/display()/hibernate()) but the init/refresh/sleep command
// sequence is taken directly from the original vendor demo code for
// GDEW026Z39, so the panel-specific behaviour is preserved.

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

#define GxEPD_BLACK 0x0000
#define GxEPD_WHITE 0xFFFF
#define GxEPD_RED   0xF800

class GxEPD2_260_Z39c : public Adafruit_GFX
{
  public:
    static const uint16_t WIDTH  = 152;
    static const uint16_t HEIGHT = 296;
    static const bool hasColor = true;
    static const bool hasPartialUpdate = false; // this panel/driver only supports full refresh

    GxEPD2_260_Z39c(int8_t cs, int8_t dc, int8_t rst, int8_t busy);

    // Initializes pins, SPI and the panel. Call once in setup().
    void init(uint32_t serial_diag_bitrate = 0);

    // Adafruit_GFX overrides — these only touch the in-memory buffers,
    // call display() afterwards to actually push pixels to the screen.
    void fillScreen(uint16_t color) override;
    void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    // Draws a bitmap at (x, y) into the in-memory buffer. `bitmap` must be
    // PROGMEM data in "Horizontal - 1 bit per pixel" format (MSB first,
    // row width padded to a whole byte) — the standard export format from
    // tools like image2cpp / LCD Assistant. `color` selects which plane
    // the set bits (1) are drawn into: GxEPD_BLACK or GxEPD_RED.
    // Call display() afterwards to push it to the panel.
    void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    // Fast path for full-screen images exported at exactly 152x296
    // (e.g. gImage_black1 / gImage_red1 style arrays from the original
    // vendor demo code). Copies both planes directly, no per-pixel loop.
    // blackBitmap / redBitmap must each be _bufSize (5624) bytes, PROGMEM,
    // "Horizontal - 1 bit per pixel" format, 1 = white/no-red, 0 = black/red
    // (same convention as the original GDEW026Z39 demo bitmaps).
    // Pass nullptr for either plane to leave it untouched.
    void writeFullScreenBitmap(const uint8_t* blackBitmap, const uint8_t* redBitmap);

    // Pushes both buffers (black + red) to the panel and triggers refresh.
    void display();

    // Puts the panel into deep sleep. Call after display(); required by
    // the datasheet — do not skip this.
    void hibernate();

  private:
    int8_t _cs, _dc, _rst, _busy;
    static const uint16_t _bufSize = (WIDTH / 8) * HEIGHT; // 5624 bytes
    uint8_t _black[_bufSize];
    uint8_t _red[_bufSize];

    void _writeCmd(uint8_t c);
    void _writeData(uint8_t d);
    void _waitBusy();
    void _hwReset();
    void _initDisplay();
};

#endif
