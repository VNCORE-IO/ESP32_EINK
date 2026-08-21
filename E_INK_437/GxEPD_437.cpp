#include "GxEPD_437.h"
static const uint16_t _bufSize = (WIDTH / 8) * HEIGHT; // 22 * 480 = 10560 bytes
static uint8_t _black[_bufSize];
static uint8_t _red[_bufSize];
GxEPD_437::GxEPD_437(int8_t cs, int8_t dc, int8_t rst, int8_t busy)
  : Adafruit_GFX(WIDTH, HEIGHT), _cs(cs), _dc(dc), _rst(rst), _busy(busy) {
}
void GxEPD_437::_SPI_Write(unsigned char value) {
  unsigned char i;
  _SPI_Delay(1);
  for (i = 0; i < 8; i++) {
    EPD_CLK_0;
    _SPI_Delay(1);
    if (value & 0x80)
      EPD_MOSI_1;
    else
      EPD_MOSI_0;
    value = (value << 1);
    _SPI_Delay(1);
    _delay_us(1);
    EPD_CLK_1;
    _SPI_Delay(1);
  }
}

/////////////////////delay//////////////////////////////////////
void GxEPD_437::_delay_us(unsigned int xus)  //1us
{
  for (; xus > 1; xus--)
    ;
}
void GxEPD_437::_delay_xms(unsigned long xms)  //1ms
{
  unsigned long i = 0, j = 0;

  for (j = 0; j < xms; j++) {
    for (i = 0; i < 256; i++)
      ;
  }
}
//////////////////////SPI///////////////////////////////////
void GxEPD_437::_SPI_Delay(unsigned char xrate) {
  unsigned char i;
  while (xrate) {
    for (i = 0; i < 2; i++)
      ;
    xrate--;
  }
}
void GxEPD_437::_writeCmd(uint8_t c) {
  _SPI_Delay(1);
  digitalWrite(_cs, LOW);
  digitalWrite(_dc, LOW);  // command
  _SPI_Write(c);
  digitalWrite(_cs, HIGH);

  // digitalWrite(_cs, LOW);
  // digitalWrite(_dc, LOW);   // command
  // SPI.transfer(c);
  // digitalWrite(_cs, HIGH);
}

void GxEPD_437::_writeData(uint8_t d) {
  // digitalWrite(_cs, LOW);
  // digitalWrite(_dc, HIGH);  // data
  // SPI.transfer(d);
  // digitalWrite(_cs, HIGH);
  _SPI_Delay(1);
  digitalWrite(_cs, LOW);
  digitalWrite(_dc, HIGH);  // data
  _SPI_Write(d);
  digitalWrite(_cs, HIGH);
}

void GxEPD_437::_waitBusy() {
  unsigned char busy;
  do
  {
    _writeCmd(0x71);
    busy = isEPD_BUSY;
    busy =!(busy & 0x01);        
  }
  while(busy);   
  _delay_xms(200);    
//   _writeCmd(0x71);
//   // BUSY pin reads LOW while busy on this panel family (same as Z39c).
//   // If your panel is active-HIGH busy instead, flip this condition.
//   while (digitalRead(_busy) == LOW) {
//     delay(1);
// #if defined(ESP8266) || defined(ESP32)
//     yield();  // feed the watchdog
// #endif
  // }
}

void GxEPD_437::_hwReset() {
  digitalWrite(_rst, LOW);
  delay(100);  // at least 10ms per datasheet
  digitalWrite(_rst, HIGH);
  delay(100);
}
void GxEPD_437::init(uint32_t serial_diag_bitrate) {
  pinMode(BUSY_PIN, INPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(DC_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(SDI_PIN, OUTPUT);
  // SPI.begin();
  // SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  // //SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  memset(_black, 0xFF, _bufSize);  // 0xFF = white
  memset(_red, 0xFF, _bufSize);    // 0xFF = no red

  _initDisplay();
}

void GxEPD_437::_initDisplay() {

  EPD_RST_0;   // Module reset
  delay(100);  //At least 10ms
  EPD_RST_1;
  delay(100);
  _writeCmd(0x06);   //boost soft start
  _writeData(0x17);  //A
  _writeData(0x17);  //B
  _writeData(0x17);  //C

  _writeCmd(0x04);  //Power on
  _waitBusy();  //waiting for the electronic paper IC to release the idle signal

  _writeCmd(0x00);   //panel setting
  _writeData(0x0f);  //LUT from OTP, 176x480
  _writeData(0x0d);  //VCOM to 0V fast


  _writeCmd(0x61);  //resolution setting
  _writeData(0xB0);
  _writeData(0x01);
  _writeData(0xE0);

  _writeCmd(0X50);   //VCOM AND DATA INTERVAL SETTING
  _writeData(0x77);  //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}

void GxEPD_437::fillScreen(uint16_t color) {
  uint8_t blackVal = (color == GxEPD_BLACK) ? 0x00 : 0xFF;
  uint8_t redVal = (color == GxEPD_RED) ? 0x00 : 0xFF;
  memset(_black, blackVal, _bufSize);
  memset(_red, redVal, _bufSize);
}

void GxEPD_437::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  uint16_t byteIdx = x / 8 + (uint32_t)y * (WIDTH / 8);
  uint8_t bitMask = 0x80 >> (x % 8);

  switch (color) {
    case GxEPD_BLACK:
      _black[byteIdx] &= ~bitMask;  // 0 = black
      _red[byteIdx] |= bitMask;     // make sure it's not also red
      break;
    case GxEPD_RED:
      _black[byteIdx] |= bitMask;
      _red[byteIdx] &= ~bitMask;  // 0 = red
      break;
    default:  // GxEPD_WHITE or anything else
      _black[byteIdx] |= bitMask;
      _red[byteIdx] |= bitMask;
      break;
  }
}

void GxEPD_437::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {
  int16_t byteWidth = (w + 7) / 8;  // horizontal 1bpp: row bytes, MSB first
  uint8_t byte = 0;

  for (int16_t j = 0; j < h; j++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7) byte <<= 1;
      else byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);

      if (byte & 0x80) {
        drawPixel(x + i, y + j, color);
      }
    }
  }
}

void GxEPD_437::writeFullScreenBitmap(const uint8_t* blackBitmap, const uint8_t* redBitmap) {
  if (blackBitmap != nullptr) {
    for (uint16_t i = 0; i < _bufSize; i++) _black[i] = pgm_read_byte(&blackBitmap[i]);
  }
  if (redBitmap != nullptr) {
    for (uint16_t i = 0; i < _bufSize; i++) _red[i] = pgm_read_byte(&redBitmap[i]);
  }
}

void GxEPD_437::display() {

  _writeCmd(0x10);  // transfer black/white plane
  for (uint16_t i = 0; i < _bufSize; i++) _writeData(_black[i]);

  _writeCmd(0x13);  // transfer red plane
  for (uint16_t i = 0; i < _bufSize; i++) _writeData(_red[i]);

  _writeCmd(0x12);  // display refresh
  _delay_xms(100);      // datasheet: at least 200us, keep generous margin
  _waitBusy();
}

void GxEPD_437::hibernate() {
  _writeCmd(0x50);  // VCOM and data interval setting
  _writeData(0xf7);

  _writeCmd(0x02);  // power off
  _waitBusy();

  _writeCmd(0x07);  // deep sleep — required, do not skip
  _writeData(0xA5);
}
