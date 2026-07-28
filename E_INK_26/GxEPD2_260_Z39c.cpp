#include "GxEPD2_260_Z39c.h"

GxEPD2_260_Z39c::GxEPD2_260_Z39c(int8_t cs, int8_t dc, int8_t rst, int8_t busy)
  : Adafruit_GFX(WIDTH, HEIGHT), _cs(cs), _dc(dc), _rst(rst), _busy(busy)
{
}

void GxEPD2_260_Z39c::_writeCmd(uint8_t c)
{
  digitalWrite(_cs, LOW);
  digitalWrite(_dc, LOW);   // command
  SPI.transfer(c);
  digitalWrite(_cs, HIGH);
}

void GxEPD2_260_Z39c::_writeData(uint8_t d)
{
  digitalWrite(_cs, LOW);
  digitalWrite(_dc, HIGH);  // data
  SPI.transfer(d);
  digitalWrite(_cs, HIGH);
}

void GxEPD2_260_Z39c::_waitBusy()
{
  _writeCmd(0x71);
  // BUSY pin reads LOW while busy on this panel (same as original driver)
  while (digitalRead(_busy) == LOW)
  {
    delay(1);
#if defined(ESP8266) || defined(ESP32)
    yield(); // feed the watchdog, replaces ESP.wdtFeed() from the original code
#endif
  }
}

void GxEPD2_260_Z39c::_hwReset()
{
  digitalWrite(_rst, LOW);
  delay(100); // at least 10ms per datasheet
  digitalWrite(_rst, HIGH);
  delay(100);
}

void GxEPD2_260_Z39c::init(uint32_t serial_diag_bitrate)
{
  if (serial_diag_bitrate > 0)
  {
    Serial.begin(serial_diag_bitrate);
  }

  pinMode(_busy, INPUT);
  pinMode(_rst, OUTPUT);
  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  memset(_black, 0xFF, _bufSize); // 0xFF = white
  memset(_red, 0xFF, _bufSize);   // 0xFF = no red

  _initDisplay();
}

void GxEPD2_260_Z39c::_initDisplay()
{
  _hwReset();

  _writeCmd(0x06);      // boost soft start
  _writeData(0x17);     // A
  _writeData(0x17);     // B
  _writeData(0x17);     // C

  _writeCmd(0x04);      // power on
  _waitBusy();           // wait for the panel to release the idle signal

  _writeCmd(0x00);      // panel setting
  _writeData(0x0f);     // LUT from OTP, 128x296
  _writeData(0x0d);     // VCOM to 0V fast

  _writeCmd(0x61);      // resolution setting
  _writeData(0x98);     // HRES = 152
  _writeData(0x01);     // VRES high byte
  _writeData(0x28);     // VRES low byte -> 0x0128 = 296

  _writeCmd(0x50);      // VCOM and data interval setting
  _writeData(0x77);     // WBmode: VBDF 17 | D7 VBDW 97 VBDB 57
}

void GxEPD2_260_Z39c::fillScreen(uint16_t color)
{
  uint8_t blackVal = (color == GxEPD_BLACK) ? 0x00 : 0xFF;
  uint8_t redVal   = (color == GxEPD_RED)   ? 0x00 : 0xFF;
  memset(_black, blackVal, _bufSize);
  memset(_red, redVal, _bufSize);
}

void GxEPD2_260_Z39c::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  uint16_t byteIdx = x / 8 + (uint32_t)y * (WIDTH / 8);
  uint8_t bitMask = 0x80 >> (x % 8);

  switch (color)
  {
    case GxEPD_BLACK:
      _black[byteIdx] &= ~bitMask; // 0 = black
      _red[byteIdx]   |= bitMask;  // make sure it's not also red
      break;
    case GxEPD_RED:
      _black[byteIdx] |= bitMask;
      _red[byteIdx]   &= ~bitMask; // 0 = red
      break;
    default: // GxEPD_WHITE or anything else
      _black[byteIdx] |= bitMask;
      _red[byteIdx]   |= bitMask;
      break;
  }
}

void GxEPD2_260_Z39c::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color)
{
  int16_t byteWidth = (w + 7) / 8; // horizontal 1bpp: row bytes, MSB first
  uint8_t byte = 0;

  for (int16_t j = 0; j < h; j++)
  {
    for (int16_t i = 0; i < w; i++)
    {
      if (i & 7) byte <<= 1;
      else       byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);

      if (byte & 0x80)
      {
        drawPixel(x + i, y + j, color);
      }
      // 0 bits are left as-is (transparent), matching Adafruit_GFX drawBitmap behaviour
    }
  }
}

void GxEPD2_260_Z39c::writeFullScreenBitmap(const uint8_t* blackBitmap, const uint8_t* redBitmap)
{
  if (blackBitmap != nullptr)
  {
    for (uint16_t i = 0; i < _bufSize; i++) _black[i] = pgm_read_byte(&blackBitmap[i]);
  }
  if (redBitmap != nullptr)
  {
    for (uint16_t i = 0; i < _bufSize; i++) _red[i] = pgm_read_byte(&redBitmap[i]);
  }
}

void GxEPD2_260_Z39c::display()
{
  _writeCmd(0x10);      // transfer black/white plane
  for (uint16_t i = 0; i < _bufSize; i++) _writeData(_black[i]);

  _writeCmd(0x13);      // transfer red plane
  for (uint16_t i = 0; i < _bufSize; i++) _writeData(_red[i]);

  _writeCmd(0x12);      // display refresh
  delay(100);            // datasheet: at least 200us, keep generous margin
  _waitBusy();
}

void GxEPD2_260_Z39c::hibernate()
{
  _writeCmd(0x50);      // VCOM and data interval setting
  _writeData(0xf7);

  _writeCmd(0x02);      // power off
  _waitBusy();

  _writeCmd(0x07);      // deep sleep — required, do not skip
  _writeData(0xA5);
}
