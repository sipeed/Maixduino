#include "SPI.h"

SPIClass spi0(SPI0);

void setup()
{
    spi0.setFrequency(10000000);
    spi0.begin(27, 26, 28, 29);
    spi0.transfer(1);
    spi0.transfer(2);
    spi0.transfer(3);
    spi0.transfer(0xEE);
}

void loop()
{

}


