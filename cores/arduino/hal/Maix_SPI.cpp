
#include "../../../libraries/SPI/src/SPI.h"

#include "stdio.h"
#include "utils.h"
#include "fpioa.h"
#include "stdint.h"
#include "stdbool.h"
#include "../kendryte-standalone-sdk/lib/drivers/include/spi.h"
#include "SPI_hal.h"
#include "sysctl.h"

SPIClass SPI;

extern volatile spi_t *const spi[4];

static spi_transfer_width_t sipeed_spi_get_frame_size(size_t data_bit_length)
{
    if (data_bit_length < 8)
        return SPI_TRANS_CHAR;
    else if (data_bit_length < 16)
        return SPI_TRANS_SHORT;
    return SPI_TRANS_INT;
}

static void sipeed_spi_set_tmod(uint8_t spi_num, uint32_t tmod)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    volatile spi_t *spi_handle = spi[spi_num];
    uint8_t tmod_offset = 0;
    switch(spi_num)
    {
        case 0:
        case 1:
            tmod_offset = 8;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            tmod_offset = 10;
            break;
    }
    set_bit(&spi_handle->ctrlr0, 3 << tmod_offset, tmod << tmod_offset);
}

/**
 * 
 * @param chip_select -1: not use cs
 */
void sipeed_spi_transfer_data_standard(spi_device_num_t spi_num, int8_t chip_select, const uint8_t *tx_buff,uint8_t *rx_buff,  size_t len)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    configASSERT(len > 0);
    size_t index, fifo_len;
    size_t rx_len = len;
    size_t tx_len = rx_len;
    sipeed_spi_set_tmod(spi_num, SPI_TMOD_TRANS_RECV);

    volatile spi_t *spi_handle = spi[spi_num];

    uint8_t dfs_offset;
    switch(spi_num){
        case 0:
        case 1:
            dfs_offset = 16;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            dfs_offset = 0;
            break;
    }
    uint32_t data_bit_length = (spi_handle->ctrlr0 >> dfs_offset) & 0x1F;
    spi_transfer_width_t frame_width = sipeed_spi_get_frame_size(data_bit_length);
    spi_handle->ctrlr1 = (uint32_t)(tx_len/frame_width - 1);
    spi_handle->ssienr = 0x01;
    spi_handle->ser = 1U << chip_select;
    uint32_t i = 0;
    while (tx_len)
    {
        fifo_len = 32 - spi_handle->txflr;
        fifo_len = fifo_len < tx_len ? fifo_len : tx_len;
        switch(frame_width)
        {
            case SPI_TRANS_INT:
                fifo_len = fifo_len / 4 * 4;
                for (index = 0; index < fifo_len / 4; index++)
                    spi_handle->dr[0] = ((uint32_t *)tx_buff)[i++];
                break;
            case SPI_TRANS_SHORT:
                fifo_len = fifo_len / 2 * 2;
                for (index = 0; index < fifo_len / 2; index++)
                    spi_handle->dr[0] = ((uint16_t *)tx_buff)[i++];
                break;
            default:
                for (index = 0; index < fifo_len; index++)
                    spi_handle->dr[0] = tx_buff[i++];
                break;
        }
        tx_len -= fifo_len;
    }

    while ((spi_handle->sr & 0x05) != 0x04)
        ;
    if(rx_buff)
    {
        i = 0;
        while (rx_len)
        {
            fifo_len = spi_handle->rxflr;
            fifo_len = fifo_len < rx_len ? fifo_len : rx_len;
            switch(frame_width)
            {
                case SPI_TRANS_INT:
                    fifo_len = fifo_len / 4 * 4;
                    for (index = 0; index < fifo_len / 4; index++)
                    ((uint32_t *)rx_buff)[i++] = spi_handle->dr[0];
                    break;
                case SPI_TRANS_SHORT:
                    fifo_len = fifo_len / 2 * 2;
                    for (index = 0; index < fifo_len / 2; index++)
                    ((uint16_t *)rx_buff)[i++] = (uint16_t)spi_handle->dr[0];
                    break;
                default:
                    for (index = 0; index < fifo_len; index++)
                        rx_buff[i++] = (uint8_t)spi_handle->dr[0];
                    break;
            }

            rx_len -= fifo_len;
        }
    }
    spi_handle->ser = 0x00;
    spi_handle->ssienr = 0x00;
}

void sipeed_spi_deinit(spi_device_num_t spi_num)
{
    volatile spi_t *spi_handle = spi[spi_num];
    spi_handle->ssienr = 0x00;
    sysctl_clock_disable( sysctl_clock_t(SYSCTL_CLOCK_SPI0 + spi_num));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct{
    int8_t pin;
    bool   used;
} spi_ss_t;

static spi_ss_t g_ss_table[2][4]={
    {//SPI0
        {.pin = -1, .used = false},
        {.pin = -1, .used = false},
        {.pin = -1, .used = false},
        {.pin = -1, .used = false}
    },
    {//SPI1
        {.pin = -1, .used = false},
        {.pin = -1, .used = false},
        {.pin = -1, .used = false},
        {.pin = -1, .used = false}
    }
};

int8_t getSsByPin(uint8_t spi, int8_t pin)
{
    uint8_t i;

    if(pin<0)
        return -1;
    for(i=0; i<4; ++i)
    {
        if(g_ss_table[spi][i].used && g_ss_table[spi][i].pin == pin)
            return i;
    }
    return -1;
}

int8_t getSsNumLast(uint8_t spi)
{
    uint8_t i=0, count=0;
    for(i=0; i<4; ++i)
    {
        if(!g_ss_table[spi][i].used)
            ++count;
    }
    return count;
}

bool checkSs(uint8_t spi, int8_t ss)
{
    int8_t ssPeriph;
    if( ss < 0) // not use ss
        return true;
    ssPeriph = getSsByPin(spi, ss);
    if(ssPeriph >= 0)
        return true;
    if(getSsNumLast(spi) > 0)
        return true;
    return false;
}

int8_t setSs(uint8_t spi, int8_t pin)
{
    uint8_t i=0;
    for(i=0; i<4; ++i)
    {
        if(!g_ss_table[spi][i].used)
        {
            g_ss_table[spi][i].used = true;
            g_ss_table[spi][i].pin = pin;
            return i;
        }
    }
    return -1;
}

/**
 * @param miso -1: not use miso
 * @param mosi must >= 0
 * @param ss -1: not use hardware ss
 */
bool checkPinParam(uint8_t spi, int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    if(!checkSs(spi, ss))
        return false;
    //TODO:
    return true;
}

bool checkFreq(uint32_t freq)
{
    return true;
}
bool checkBitOder(uint8_t bitOrder)
{
    //TODO: support LSB
    if(bitOrder!=SPI_MSBFIRST)
        return false;
    return true;
}
bool checkDataMode(uint8_t dataMode)
{
    if(dataMode > SPI_MODE3)
        return false;
    return true;
}

SPIClass::SPIClass(spi_id_t spi_bus)
:_spiNum(spi_bus), _freq(1000000),
 _sck(-1), _miso(-1), _mosi(-1), _ss(-1),
 _inTransaction(false),
 _initPinsInConstruct(false)
{
    configASSERT(_spiNum==SPI_SOFT || _spiNum==SPI0 || _spiNum==SPI1);
}

SPIClass::SPIClass(spi_id_t spi_bus, int8_t sck, int8_t miso, int8_t mosi, int8_t ss, uint32_t freq)
:_spiNum(spi_bus), _freq(freq),
 _sck(sck), _miso(miso), _mosi(mosi), _ss(ss),
 _inTransaction(false),
 _initPinsInConstruct(true)
{

}


/**
 * 
 * @param ss -1: not use
 */
void SPIClass::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    if(_initPinsInConstruct)
    {
        sck = _sck;
        miso = _miso;
        mosi = _mosi;
        ss = _ss;
    }
    configASSERT(checkPinParam(_spiNum, sck, miso, mosi, ss));

    if(_spiNum == SPI_SOFT)
    {
        configASSERT(!"Soft SPI Not Support Yet!");
        return ;
    }
    //hardware SPI
    if(_spiNum == SPI0)
    {
        fpioa_set_function(sck, FUNC_SPI0_SCLK);
        if( ss >= 0)
        {
            fpioa_function_t a = (fpioa_function_t)(FUNC_SPI0_SS0+setSs(_spiNum, ss));
            fpioa_set_function(ss, a);
        }
        fpioa_set_function(mosi, FUNC_SPI0_D0);
        if(miso>=0)
            fpioa_set_function(miso, FUNC_SPI0_D1);
    }
    else if(_spiNum == SPI1)
    {
        fpioa_set_function(sck, FUNC_SPI1_SCLK);
        if( ss >= 0)
        {
            fpioa_set_function(ss, (fpioa_function_t)(FUNC_SPI1_SS0+setSs(_spiNum, ss)));
        }
        fpioa_set_function(mosi, FUNC_SPI1_D0);
        if(miso>=0)
            fpioa_set_function(miso, FUNC_SPI1_D1);
    }
    _mosi = mosi;
    _miso = miso;
    _sck = sck;
    _ss = ss;
    _ssPeriph = getSsByPin(_spiNum, ss);
    if(_ssPeriph<0)
        _ssPeriph = 0; // default to cs0 TODO: optimize? 
    spi_init(spi_device_num_t(_spiNum), spi_work_mode_t(_dataMode), SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spi_device_num_t(_spiNum), _freq);
}

void SPIClass::end()
{
    sipeed_spi_deinit(spi_device_num_t(_spiNum));
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
    //TODO: support LSB
    configASSERT(bitOrder==SPI_MSBFIRST);
}

void SPIClass::setDataMode(uint8_t dataMode)
{
    _dataMode = dataMode;
    spi_init(spi_device_num_t(_spiNum), spi_work_mode_t(_dataMode), SPI_FF_STANDARD, 8, 0);
}

void SPIClass::setFrequency(uint32_t freq)
{
    _freq = freq;
    spi_set_clk_rate(spi_device_num_t(_spiNum), _freq);
}

void SPIClass::beginTransaction(SPISettings settings)
{
    SPI_MUTEX_LOCK();
    _inTransaction = true;
    if( settings._freq!=_freq || settings._dataMode!=_dataMode || settings._bitOrder!=_bitOrder)
    {
        _dataMode = settings._dataMode;
        _bitOrder = settings._bitOrder; //TODO: bit order
        _freq = settings._freq;
        spi_init(spi_device_num_t(_spiNum), spi_work_mode_t(_dataMode), SPI_FF_STANDARD, 8, 0);
        spi_set_clk_rate(spi_device_num_t(_spiNum), _freq);
    }

}

void SPIClass::endTransaction(void)
{
    _inTransaction = false;
    SPI_MUTEX_UNLOCK();
}

void SPIClass::transfer(uint8_t * data, uint32_t size)
{
    sipeed_spi_transfer_data_standard(spi_device_num_t(_spiNum), _ssPeriph, data, NULL, size);
}

uint8_t SPIClass::transfer(uint8_t data)
{
    uint8_t temp;
    sipeed_spi_transfer_data_standard(spi_device_num_t(_spiNum), _ssPeriph, &data, &temp, 1);
    return temp;
}

void SPIClass::transferBytes(uint8_t * data, uint8_t * out, uint32_t size)
{
    sipeed_spi_transfer_data_standard(spi_device_num_t(_spiNum), _ssPeriph, data, out, size);
}


SPISettings::SPISettings(uint32_t freq, uint8_t bitOrder, uint8_t dataMode)
:_freq(freq), _bitOrder(bitOrder), _dataMode(dataMode)
{
    configASSERT(checkFreq(freq));
    configASSERT(checkBitOder(bitOrder));
    configASSERT(checkDataMode(dataMode));
}

