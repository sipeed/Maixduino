#include <Sipeed_ST7789.h>

SPIClass spi_(SPI0);
Sipeed_ST7789 lcd(320, 240, spi_);

void setup()
{
    lcd.begin(15000000, COLOR_BLACK);
    lcd.drawRect(20, 20, 50, 50, COLOR_WHITE);
    lcd.drawCircle(100, 100, 40, COLOR_WHITE);
    lcd.fillTriangle(10, 200, 300, 200, 300, 150, COLOR_WHITE);
}

void loop()
{

}

