
#include "Sipeed_ST7789.h"
#include "lcd.h"
#include "sysctl.h"
#include "utils.h"

Sipeed_ST7789::Sipeed_ST7789(uint16_t w, uint16_t h, SPIClass& spi, int8_t dc_pin, int8_t rst_pin, uint8_t dma_ch )
:Adafruit_GFX(w,h),
 _spi(spi), _dcxPin(dc_pin), _rstPin(rst_pin), 
 _dmaCh(dma_ch)
{
    configASSERT(_spi.busId()==SPI0);
}

Sipeed_ST7789::~Sipeed_ST7789(void)
{

}

boolean Sipeed_ST7789::begin( uint32_t freq, uint16_t color )
{
    uint8_t spiNum = _spi.busId();
    if( (spi_id_t)spiNum == SPI0)
    {
        fpioa_set_function(SIPEED_ST7789_SS_PIN  , (fpioa_function_t)(FUNC_SPI0_SS0 + SIPEED_ST7789_SS));
        fpioa_set_function(SIPEED_ST7789_SCLK_PIN, (fpioa_function_t)FUNC_SPI0_SCLK);
    }
    else if((spi_id_t)spiNum == SPI1)
    {
        fpioa_set_function(SIPEED_ST7789_SS_PIN  , (fpioa_function_t)(FUNC_SPI1_SS0 + SIPEED_ST7789_SS));
        fpioa_set_function(SIPEED_ST7789_SCLK_PIN, (fpioa_function_t)FUNC_SPI1_SCLK);
    }
    sysctl_set_spi0_dvp_data(1);
    _freq = freq;
    lcd_init(spiNum, SIPEED_ST7789_SS, SIPEED_ST7789_RST_GPIONUM, SIPEED_ST7789_DCX_GPIONUM, _freq, _rstPin,  _dcxPin, _dmaCh);
    lcd_clear(color);
    return true;
}


void Sipeed_ST7789::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    lcd_draw_point(x, y, color);
}
void Sipeed_ST7789::writePixel(int16_t x, int16_t y, uint16_t color)
{
    lcd_draw_point(x, y, color);
}

void Sipeed_ST7789::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    lcd_fill_rectangle(x, y , x+w-1, y+h-1, color);
}

void Sipeed_ST7789::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    lcd_fill_rectangle(x, y, x, y+h-1, color);
}

void Sipeed_ST7789::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    lcd_fill_rectangle(x, y, x+w-1, y, color);
}

void Sipeed_ST7789::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    lcd_fill_rectangle(x, y, x, y+h-1, color);
}

void Sipeed_ST7789::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    lcd_fill_rectangle(x, y, x+w-1, y, color);
}

void Sipeed_ST7789::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    lcd_fill_rectangle(x, y , x+w-1, y+h-1, color);
}

void Sipeed_ST7789::fillScreen(uint16_t color)
{
    lcd_clear(color);
}

void Sipeed_ST7789::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    lcd_draw_rectangle(x, y, x+w-1, y+h-1, 1, color);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t lineWidth)
{
    lcd_draw_rectangle(x, y, x+w-1, y+h-1, lineWidth, color);
}




