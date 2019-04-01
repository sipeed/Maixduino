#ifndef __SPI__fafefef_H
#define __SPI__fafefef_H

#include "stdint.h"

typedef enum{
    SPI_SOFT = -1,
    SPI0 = 0,
    SPI1,
    SPI2,
    SPI3,
    SPI_MAX
} spi_id_t;


#define SPI_LSBFIRST 0
#define SPI_MSBFIRST 1

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3


class SPISettings
{
public:
    SPISettings() :_freq(1000000), _bitOrder(SPI_MSBFIRST), _dataMode(SPI_MODE0) {}
    SPISettings(uint32_t freq, uint8_t bitOrder, uint8_t dataMode);
    uint32_t _freq;
    uint8_t  _bitOrder;
    uint8_t  _dataMode;
};

class SPIClass
{
private:
    spi_id_t _spiNum;
    uint32_t _freq;
    int8_t   _sck;
    int8_t   _miso;
    int8_t   _mosi;
    int8_t   _ss;        //pin
    int8_t   _ssPeriph;  //peripheral
    uint8_t  _dataMode;
    uint8_t  _bitOrder;
    bool     _inTransaction;
    bool     _initPinsInConstruct;

public:
    SPIClass(spi_id_t spi_bus = SPI1);
    /*
     * API just for k210
     */
    SPIClass(spi_id_t spi_bus, int8_t sck, int8_t miso, int8_t mosi, int8_t ss, uint32_t freq = 1000000);
    void begin(int8_t sck=27, int8_t miso=26, int8_t mosi=28, int8_t ss=-1);
    void end();

    void setBitOrder(uint8_t bitOrder);
    void setDataMode(uint8_t dataMode);
    void setFrequency(uint32_t freq);

    void beginTransaction(SPISettings settings);
    void endTransaction(void);
    void transfer(uint8_t * data, uint32_t size);
    uint8_t transfer(uint8_t data);
  
    void transferBytes(uint8_t * data, uint8_t * out, uint32_t size);

    // special
    spi_id_t busId(){ return _spiNum; }

};

extern SPIClass SPI;


#endif //__SPI_H
