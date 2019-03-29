#ifndef _SIPEED_ST7789_H
#define _SIPEED_ST7789_H


#include <SPI.h>
#include <Adafruit_GFX.h>


#if (defined(BOARD_SIPEED_MAIX_GO) || defined(BOARD_SIPEED_MAIX_ONE_DOCK) )
#define SIPEED_ST7789_RST_PIN    37
#define SIPEED_ST7789_DCX_PIN    38
#define SIPEED_ST7789_SS_PIN     36
#define SIPEED_ST7789_SCLK_PIN   39
#else
#define SIPEED_ST7789_RST_PIN    37
#define SIPEED_ST7789_DCX_PIN    38
#define SIPEED_ST7789_SS_PIN     36
#define SIPEED_ST7789_SCLK_PIN   39
#endif

// default peripheral
#define SIPEED_ST7789_RST_GPIONUM  6
#define SIPEED_ST7789_DCX_GPIONUM  7
#define SIPEED_ST7789_SS           3


#define COLOR_BLACK       0x0000
#define COLOR_NAVY        0x000F
#define COLOR_DARKGREEN   0x03E0
#define COLOR_DARKCYAN    0x03EF
#define COLOR_MAROON      0x7800
#define COLOR_PURPLE      0x780F
#define COLOR_OLIVE       0x7BE0
#define COLOR_LIGHTGREY   0xC618
#define COLOR_DARKGREY    0x7BEF
#define COLOR_BLUE        0x001F
#define COLOR_GREEN       0x07E0
#define COLOR_CYAN        0x07FF
#define COLOR_RED         0xF800
#define COLOR_MAGENTA     0xF81F
#define COLOR_YELLOW      0xFFE0
#define COLOR_WHITE       0xFFFF
#define COLOR_ORANGE      0xFD20
#define COLOR_GREENYELLOW 0xAFE5
#define COLOR_PINK        0xF81F


class Sipeed_ST7789 : public Adafruit_GFX{
public:
    /**
     * @param rst_pin -1: not use reset pin
     */
    Sipeed_ST7789(uint16_t w, uint16_t h, SPIClass& spi, int8_t dc_pin = SIPEED_ST7789_DCX_PIN, int8_t rst_pin = SIPEED_ST7789_RST_PIN , uint8_t dma_ch = 3);

    ~Sipeed_ST7789(void);

    boolean      begin( uint32_t freq = 15000000, uint16_t color = 0xffff );

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void writePixel(int16_t x, int16_t y, uint16_t color);
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

    virtual void
    drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
    drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
    fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
    fillScreen(uint16_t color),
    drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t lineWidth);

    virtual void setRotation(uint8_t r);
    virtual void invertDisplay(boolean invert);

    void drawImage(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint16_t* img);

private:
    SPIClass& _spi;
    int8_t    _dcxPin;
    int8_t    _rstPin;
    uint8_t   _dmaCh;
    uint32_t  _freq;
    uint16_t  _screenDir;


};

#endif

