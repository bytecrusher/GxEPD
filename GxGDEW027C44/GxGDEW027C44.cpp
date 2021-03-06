/************************************************************************************
   class GxGDEW027C44 : Display class example for GDEW027C44 e-Paper from Dalian Good Display Co., Ltd.: www.good-display.com

   based on Demo Example from Good Display, now available on http://www.good-display.com/download_list/downloadcategoryid=34&isMode=false.html

   Author : J-M Zingg

   modified by :

   Version : 2.0

   Support: minimal, provided as example only, as is, no claim to be fit for serious use

   connection to the e-Paper display is through DESTM32-S2 connection board, available from GoodDisplay

   DESTM32-S2 pinout (top, component side view):
       |-------------------------------------------------
       |  VCC  |o o| VCC 5V
       |  GND  |o o| GND
       |  3.3  |o o| 3.3V
       |  nc   |o o| nc
       |  nc   |o o| nc
       |  nc   |o o| nc
       |  MOSI |o o| CLK
       |  DC   |o o| D/C
       |  RST  |o o| BUSY
       |  nc   |o o| BS
       |-------------------------------------------------

   note: for correct red color jumper J3 must be set on 0.47 side (towards FCP connector)

*/
#include "GxGDEW027C44.h"

const uint8_t lut_vcomDC[] =
{
  0x00	, 0x00,
  0x00	, 0x1A	, 0x1A	, 0x00	, 0x00	, 0x01,
  0x00	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x00	, 0x0E	, 0x01	, 0x0E	, 0x01	, 0x10,
  0x00	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x00	, 0x04	, 0x10	, 0x00	, 0x00	, 0x05,
  0x00	, 0x03	, 0x0E	, 0x00	, 0x00	, 0x0A,
  0x00	, 0x23	, 0x00	, 0x00	, 0x00	, 0x01
};
//R21H
const uint8_t lut_ww[] = {
  0x90	, 0x1A	, 0x1A	, 0x00	, 0x00	, 0x01,
  0x40	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x84	, 0x0E	, 0x01	, 0x0E	, 0x01	, 0x10,
  0x80	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x00	, 0x04	, 0x10	, 0x00	, 0x00	, 0x05,
  0x00	, 0x03	, 0x0E	, 0x00	, 0x00	, 0x0A,
  0x00	, 0x23	, 0x00	, 0x00	, 0x00	, 0x01
};
//R22H	r
const uint8_t lut_bw[] = {
  0xA0	, 0x1A	, 0x1A	, 0x00	, 0x00	, 0x01,
  0x00	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x84	, 0x0E	, 0x01	, 0x0E	, 0x01	, 0x10,
  0x90	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0xB0	, 0x04	, 0x10	, 0x00	, 0x00	, 0x05,
  0xB0	, 0x03	, 0x0E	, 0x00	, 0x00	, 0x0A,
  0xC0	, 0x23	, 0x00	, 0x00	, 0x00	, 0x01
};
//R23H	w
const uint8_t lut_bb[] = {
  0x90	, 0x1A	, 0x1A	, 0x00	, 0x00	, 0x01,
  0x40	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x84	, 0x0E	, 0x01	, 0x0E	, 0x01	, 0x10,
  0x80	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x00	, 0x04	, 0x10	, 0x00	, 0x00	, 0x05,
  0x00	, 0x03	, 0x0E	, 0x00	, 0x00	, 0x0A,
  0x00	, 0x23	, 0x00	, 0x00	, 0x00	, 0x01
};
//R24H	b
const uint8_t lut_wb[] = {
  0x90	, 0x1A	, 0x1A	, 0x00	, 0x00	, 0x01,
  0x20	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x84	, 0x0E	, 0x01	, 0x0E	, 0x01	, 0x10,
  0x10	, 0x0A	, 0x0A	, 0x00	, 0x00	, 0x08,
  0x00	, 0x04	, 0x10	, 0x00	, 0x00	, 0x05,
  0x00	, 0x03	, 0x0E	, 0x00	, 0x00	, 0x0A,
  0x00	, 0x23	, 0x00	, 0x00	, 0x00	, 0x01
};

GxGDEW027C44::GxGDEW027C44(GxIO& io, uint8_t rst, uint8_t busy)
  : GxEPD(GxGDEW027C44_WIDTH, GxGDEW027C44_HEIGHT),
    IO(io), _rst(rst), _busy(busy)
{
}

template <typename T> static inline void
swap(T& a, T& b)
{
  T t = a;
  a = b;
  b = t;
}

void GxGDEW027C44::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  // check rotation, move pixel around if necessary
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      x = GxGDEW027C44_WIDTH - x - 1;
      break;
    case 2:
      x = GxGDEW027C44_WIDTH - x - 1;
      y = GxGDEW027C44_HEIGHT - y - 1;
      break;
    case 3:
      swap(x, y);
      y = GxGDEW027C44_HEIGHT - y - 1;
      break;
  }
  uint16_t i = x / 8 + y * GxGDEW027C44_WIDTH / 8;
  if (i >= GxGDEW027C44_BUFFER_SIZE) return; // for reduced buffer size
  _black_buffer[i] = (_black_buffer[i] & (0xFF ^ (1 << (7 - x % 8)))); // white
  _red_buffer[i] = (_red_buffer[i] & (0xFF ^ (1 << (7 - x % 8)))); // white
  if (color == GxEPD_WHITE) return;
  else if (color == GxEPD_BLACK) _black_buffer[i] = (_black_buffer[i] | (1 << (7 - x % 8)));
  else if (color == GxEPD_RED) _red_buffer[i] = (_red_buffer[i] | (1 << (7 - x % 8)));
  else
  {
    if ((color & 0xF100) > (0xF100 / 2)) _red_buffer[i] = (_red_buffer[i] | (1 << (7 - x % 8)));
    else if ((((color & 0xF100) >> 11) + ((color & 0x07E0) >> 5) & (color & 0x001F)) < 3 * 255 / 2)
    {
      _black_buffer[i] = (_black_buffer[i] | (1 << (7 - x % 8)));
    }
  }
}


void GxGDEW027C44::init(void)
{
  IO.init();
  IO.setFrequency(4000000); // 4MHz : 250ns > 150ns min RD cycle
  digitalWrite(_rst, 1);
  pinMode(_rst, OUTPUT);
  pinMode(_busy, INPUT);
  fillScreen(GxEPD_WHITE);
}

void GxGDEW027C44::fillScreen(uint16_t color)
{
  uint8_t black = 0x00;
  uint8_t red = 0x00;
  if (color == GxEPD_WHITE);
  else if (color == GxEPD_BLACK) black = 0xFF;
  else if (color == GxEPD_RED) red = 0xFF;
  else if ((color & 0xF100) > (0xF100 / 2))  red = 0xFF;
  else if ((((color & 0xF100) >> 11) + ((color & 0x07E0) >> 5) & (color & 0x001F)) < 3 * 255 / 2) black = 0xFF;
  for (uint16_t x = 0; x < GxGDEW027C44_BUFFER_SIZE; x++)
  {
    _black_buffer[x] = black;
    _red_buffer[x] = red;
  }
}

void GxGDEW027C44::update(void)
{
  drawPicture(_black_buffer, _red_buffer, GxGDEW027C44_BUFFER_SIZE);
}

void  GxGDEW027C44::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  drawBitmap(bitmap, x, y, w, h, color);
}

void  GxGDEW027C44::drawBitmap(const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, bool mirror)
{
  if (mirror)
  {
    for (uint16_t x1 = x; x1 < x + w; x1++)
    {
      for (uint16_t y1 = y; y1 < y + h; y1++)
      {
        uint32_t i = (w - (x1 - x) - 1) / 8 + uint32_t(y1 - y) * uint32_t(w) / 8;
        uint16_t pixelcolor = (bitmap[i] & (0x01 << (x1 - x) % 8)) ? color : GxEPD_WHITE;
        drawPixel(x1, y1, pixelcolor);
      }
    }
  }
  else
  {
    for (uint16_t x1 = x; x1 < x + w; x1++)
    {
      for (uint16_t y1 = y; y1 < y + h; y1++)
      {
        uint32_t i = (x1 - x) / 8 + uint32_t(y1 - y) * uint32_t(w) / 8;
        uint16_t pixelcolor = (bitmap[i] & (0x80 >> (x1 - x) % 8)) ? color : GxEPD_WHITE;
        drawPixel(x1, y1, pixelcolor);
      }
    }
  }
}

void GxGDEW027C44::drawPicture(const uint8_t* black_bitmap, const uint8_t* red_bitmap, uint32_t size)
{
  _wakeUp();
  _writeCommand(0x10);
  for (uint32_t i = 0; i < GxGDEW027C44_BUFFER_SIZE; i++)
  {
    _writeData((i < size) ? black_bitmap[i] : 0x00);
  }
  _writeCommand(0x13);
  for (uint32_t i = 0; i < GxGDEW027C44_BUFFER_SIZE; i++)
  {
    _writeData((i < size) ? red_bitmap[i] : 0x00);
  }
  _writeCommand(0x12);      //display refresh
  _waitWhileBusy("update display refresh");
  _sleep();
}

void GxGDEW027C44::drawBitmap(const uint8_t* bitmap, uint32_t size)
{
  _wakeUp();
  _writeCommand(0x10);
  for (uint32_t i = 0; i < GxGDEW027C44_BUFFER_SIZE; i++)
  {
    _writeData((i < size) ? bitmap[i] : 0x00);
  }
  _writeCommand(0x13);
  for (uint32_t i = 0; i < GxGDEW027C44_BUFFER_SIZE; i++)
  {
    _writeData(0);
  }
  _writeCommand(0x12);      //display refresh
  _waitWhileBusy("update display refresh");
  _sleep();
}

void GxGDEW027C44::_writeCommand(uint8_t command)
{
  //while (!digitalRead(_busy));
  if (!digitalRead(_busy))
  {
    String str = String("command 0x") + String(command, HEX);
    _waitWhileBusy(str.c_str());
  }
  IO.writeCommandTransaction(command);
}

void GxGDEW027C44::_writeData(uint8_t data)
{
  IO.writeDataTransaction(data);
}

void GxGDEW027C44::_waitWhileBusy(const char* comment)
{
  unsigned long start = micros();
  while (1)
  {
    if (digitalRead(_busy) == 1) break;
    delay(1);
  }
  if (comment)
  {
    unsigned long elapsed = micros() - start;
    //    Serial.print(comment);
    //    Serial.print(" : ");
    //    Serial.println(elapsed);
  }
}

void GxGDEW027C44::_wakeUp()
{
  // reset required for wakeup
  digitalWrite(_rst, 0);
  delay(10);
  digitalWrite(_rst, 1);
  delay(10);

  _writeCommand(0x01);      // define by OTP
  _writeData (0x03);       //���øߵ͵�ѹ
  _writeData (0x00);
  _writeData (0x2b);
  _writeData (0x2b);
  _writeData (0x09);

  _writeCommand(0x06);         //boost�趨
  _writeData (0x07);
  _writeData (0x07);
  _writeData (0x17);       //

  _writeCommand(0xF8);         //boost�趨
  _writeData (0x60);
  _writeData (0xA5);

  _writeCommand(0xF8);         //boost�趨
  _writeData (0x89);
  _writeData (0xA5);

  _writeCommand(0xF8);         //boost�趨
  _writeData (0x90);
  _writeData (0x00);

  _writeCommand(0xF8);         //boost�趨
  _writeData (0x93);
  _writeData (0x2A);

  _writeCommand(0xF8);         //boost�趨
  _writeData (0x73);
  _writeData (0x41);

  _writeCommand(0x16);
  _writeData(0x00);

  _writeCommand(0x04);
  _waitWhileBusy("_wakeUp Power On");

  _writeCommand(0x00);
  _writeData(0xaf);   //KW-BF   KWR-AF  BWROTP 0f

  _writeCommand(0x30);      //PLL�趨 // define by OTP
  _writeData (0x3a);       //3A 100HZ   29 150Hz 39 200HZ 31 171HZ

  _writeCommand(0x61);      //�����趨 // define by OTP
  _writeData (0x00);
  _writeData (0xb0);       //176
  _writeData (0x01);
  _writeData (0x08);    //264

  _writeCommand(0x82);      //vcom�趨 // define by OTP
  _writeData (0x12);

  _writeCommand(0X50);      // define by OTP
  _writeData(0x87);   // define by OTP
  _writeLUT();              //д��lut
}

void GxGDEW027C44::_sleep(void)
{
  _writeCommand(0X07);   //power off
  _writeData (0xa5);
}

void GxGDEW027C44::_writeLUT(void)
{
  unsigned int count;
  {
    _writeCommand(0x20);							//vcom
    for (count = 0; count < 44; count++)
    {
      _writeData(lut_vcomDC[count]);
    }

    _writeCommand(0x21);							//ww --
    for (count = 0; count < 42; count++)
    {
      _writeData(lut_ww[count]);
    }

    _writeCommand(0x22);							//bw r
    for (count = 0; count < 42; count++)
    {
      _writeData(lut_bw[count]);
    }

    _writeCommand(0x23);							//wb w
    for (count = 0; count < 42; count++)
    {
      _writeData(lut_bb[count]);
    }

    _writeCommand(0x24);							//bb b
    for (count = 0; count < 42; count++)
    {
      _writeData(lut_wb[count]);
    }
  }
}

