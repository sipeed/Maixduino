#include "SPI.h"


void setup()
{
    SPI.setFrequency(10000000);
    SPI.begin(27, 26, 28, 29);
    SPI.transfer(1);
    SPI.transfer(2);
    SPI.transfer(3);
    SPI.transfer(0xEE);
}

void loop()
{

}
