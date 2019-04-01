#include "st7789.h"


static uint8_t g_gpio_dcx;
static int8_t g_gpio_rst;
static spi_device_num_t g_spi_num;
static dmac_channel_number_t g_dma_ch;
static uint8_t g_ss;

static void init_dcx(uint8_t dcx, int8_t dcx_pin)
{
    fpioa_set_function(dcx_pin , FUNC_GPIO0 + dcx);
    g_gpio_dcx = dcx;
    gpio_set_drive_mode(g_gpio_dcx, GPIO_DM_OUTPUT);
    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);
}

static void set_dcx_control(void)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW);
}

static void set_dcx_data(void)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);
}

static void init_rst(uint8_t rst, int8_t rst_pin)
{
    g_gpio_rst = rst;
    if( rst_pin < 0)
        return ;
    fpioa_set_function(rst_pin , FUNC_GPIO0 + rst);
    gpio_set_drive_mode(g_gpio_rst, GPIO_DM_OUTPUT);
    gpio_set_pin(g_gpio_rst, GPIO_PV_HIGH);
}

static void set_rst(uint8_t val)
{
    if(g_gpio_rst < 0)
        return ;
    gpio_set_pin(g_gpio_rst, val);
}

void tft_set_clk_freq(uint32_t freq)
{
    spi_set_clk_rate(g_spi_num, freq);
}

void tft_hard_init(uint8_t spi, uint8_t ss, uint8_t rst, uint8_t dcx, uint32_t freq, int8_t rst_pin, int8_t dcx_pin, uint8_t dma_ch)
{
    g_spi_num = spi;
    g_dma_ch = dma_ch;
    g_ss = ss;
    init_dcx(dcx, dcx_pin);
    init_rst(rst, rst_pin);
    set_rst(0);
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    tft_set_clk_freq(freq);
    set_rst(1);
}

void tft_write_command(uint8_t cmd)
{
    set_dcx_control();
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, (uint8_t *)(&cmd), 1, SPI_TRANS_CHAR);
}

void tft_write_byte(uint8_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length, SPI_TRANS_CHAR);
}

void tft_write_half(uint16_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
    spi_init_non_standard(g_spi_num, 16 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length, SPI_TRANS_SHORT);
}

void tft_write_word(uint32_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);

    spi_init_non_standard(g_spi_num, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length, SPI_TRANS_INT);
}

void tft_fill_data(uint32_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
    spi_init_non_standard(g_spi_num, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_fill_data_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length);
}
