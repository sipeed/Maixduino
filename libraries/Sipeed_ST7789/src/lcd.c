/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <unistd.h>
#include "lcd.h"

static lcd_ctl_t lcd_ctl;

uint16_t g_lcd_display_buff[LCD_Y_MAX * LCD_X_MAX];

static uint16_t gray2rgb565[64] = {
    0x0000,
    0x2000,
    0x4108,
    0x6108,
    0x8210,
    0xa210,
    0xc318,
    0xe318,
    0x0421,
    0x2421,
    0x4529,
    0x6529,
    0x8631,
    0xa631,
    0xc739,
    0xe739,
    0x0842,
    0x2842,
    0x494a,
    0x694a,
    0x8a52,
    0xaa52,
    0xcb5a,
    0xeb5a,
    0x0c63,
    0x2c63,
    0x4d6b,
    0x6d6b,
    0x8e73,
    0xae73,
    0xcf7b,
    0xef7b,
    0x1084,
    0x3084,
    0x518c,
    0x718c,
    0x9294,
    0xb294,
    0xd39c,
    0xf39c,
    0x14a5,
    0x34a5,
    0x55ad,
    0x75ad,
    0x96b5,
    0xb6b5,
    0xd7bd,
    0xf7bd,
    0x18c6,
    0x38c6,
    0x59ce,
    0x79ce,
    0x9ad6,
    0xbad6,
    0xdbde,
    0xfbde,
    0x1ce7,
    0x3ce7,
    0x5def,
    0x7def,
    0x9ef7,
    0xbef7,
    0xdfff,
    0xffff,
};

void lcd_polling_enable(void)
{
    lcd_ctl.mode = 0;
}

void lcd_interrupt_enable(void)
{
    lcd_ctl.mode = 1;
}

void lcd_init(uint8_t spi, uint8_t ss, uint8_t rst, uint8_t dcx, uint32_t freq, int8_t rst_pin, int8_t dcx_pin, uint8_t dma_ch)
{
    uint8_t data = 0;

    tft_hard_init(spi, ss, rst, dcx, freq, rst_pin, dcx_pin, dma_ch);
    /*soft reset*/
    tft_write_command(SOFTWARE_RESET);
    usleep(100000);
    /*exit sleep*/
    tft_write_command(SLEEP_OFF);
    usleep(100000);
    /*pixel format*/
    tft_write_command(PIXEL_FORMAT_SET);
    data = 0x55;
    tft_write_byte(&data, 1);
    lcd_set_direction(DIR_YX_RLDU);

    /*display on*/
    tft_write_command(DISPALY_ON);
    lcd_polling_enable();
}

void lcd_set_direction(lcd_dir_t dir)
{
    //dir |= 0x08;  //excahnge RGB
    lcd_ctl.dir = dir;
    if (dir & DIR_XY_MASK)
    {
        lcd_ctl.width = LCD_Y_MAX - 1;
        lcd_ctl.height = LCD_X_MAX - 1;
    }
    else
    {
        lcd_ctl.width = LCD_X_MAX - 1;
        lcd_ctl.height = LCD_Y_MAX - 1;
    }

    tft_write_command(MEMORY_ACCESS_CTL);
    tft_write_byte((uint8_t *)&dir, 1);
}

static uint32_t lcd_freq = 20000000; // default to 20MHz
void lcd_set_freq(uint32_t freq)
{
    tft_set_clk_freq(freq);
    lcd_freq = freq;
}

uint32_t lcd_get_freq()
{
    return lcd_freq;
}

void lcd_set_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t data[4] = {0};

    data[0] = (uint8_t)(x1 >> 8);
    data[1] = (uint8_t)(x1);
    data[2] = (uint8_t)(x2 >> 8);
    data[3] = (uint8_t)(x2);
    tft_write_command(HORIZONTAL_ADDRESS_SET);
    tft_write_byte(data, 4);

    data[0] = (uint8_t)(y1 >> 8);
    data[1] = (uint8_t)(y1);
    data[2] = (uint8_t)(y2 >> 8);
    data[3] = (uint8_t)(y2);
    tft_write_command(VERTICAL_ADDRESS_SET);
    tft_write_byte(data, 4);

    tft_write_command(MEMORY_WRITE);
}

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_area(x, y, x, y);
    tft_write_half(&color, 1);
}

void lcd_clear(uint16_t color)
{
    uint32_t data = ((uint32_t)color << 16) | (uint32_t)color;

    lcd_set_area(0, 0, lcd_ctl.width, lcd_ctl.height);
    tft_fill_data(&data, LCD_X_MAX * LCD_Y_MAX / 2);
}

void lcd_fill_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint32_t data = ((uint32_t)color << 16) | (uint32_t)color;
    lcd_set_area(x1, y1, x2, y2);
    tft_fill_data(&data, (y2+1-y1)*(x2+1-x1) );
}

void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t width, uint16_t color)
{
    uint32_t data_buf[640] = {0};
    uint32_t *p = data_buf;
    uint32_t data = color;
    uint32_t index = 0;

    data = (data << 16) | data;
    for (index = 0; index < 160 * width; index++)
        *p++ = data;

    lcd_set_area(x1, y1, x2, y1 + width - 1);
    tft_write_word(data_buf, ((x2 - x1 + 1) * width + 1) / 2);
    lcd_set_area(x1, y2 - width + 1, x2, y2);
    tft_write_word(data_buf, ((x2 - x1 + 1) * width + 1) / 2);
    lcd_set_area(x1, y1, x1 + width - 1, y2);
    tft_write_word(data_buf, ((y2 - y1 + 1) * width + 1) / 2);
    lcd_set_area(x2 - width + 1, y1, x2, y2);
    tft_write_word(data_buf, ((y2 - y1 + 1) * width + 1) / 2);
}

void lcd_draw_picture(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint16_t *ptr)
{
    uint32_t i;
    uint16_t *p = ptr;
    lcd_set_area(x1, y1, x1 + width - 1, y1 + height - 1);
    for (i = 0; i < LCD_MAX_PIXELS; i += 2)
    {
        g_lcd_display_buff[i] = *(p + 1);
        g_lcd_display_buff[i + 1] = *(p);
        p += 2;
    }
    tft_write_word((uint32_t*)g_lcd_display_buff, width * height / 2);
}

//draw pic's roi on (x,y)
//x,y of LCD, w,h is pic; rx,ry,rw,rh is roi
void lcd_draw_pic_roi(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rx, uint16_t ry, uint16_t rw, uint16_t rh, uint32_t *ptr)
{
    int y_oft;
    uint8_t *p;
    for (y_oft = 0; y_oft < rh; y_oft++)
    { //draw line by line
        p = (uint8_t *)(ptr) + w * 2 * (y_oft + ry) + 2 * rx;
        lcd_set_area(x, y + y_oft, x + rw - 1, y + y_oft);
        tft_write_byte(p, rw * 2); //, lcd_ctl.mode ? 2 : 0);
    }
    return;
}

static uint16_t line_buf[320]; //TODO: optimize
void lcd_draw_pic_gray(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint8_t *ptr)
{
    int x, y;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            line_buf[x] = gray2rgb565[(ptr[y * width + x]) >> 2];
        }
        lcd_set_area(x1, y1 + y, x1 + width - 1, y1 + y);
        tft_write_byte((uint8_t*)line_buf, width * 2);
    }
    return;
}

void lcd_draw_pic_grayroi(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rx, uint16_t ry, uint16_t rw, uint16_t rh, uint8_t *ptr)
{
    int y_oft;
    uint8_t *p;
    for (y_oft = 0; y_oft < rh; y_oft++)
    { //draw line by line
        p = (uint8_t *)(ptr) + w * (y_oft + ry) + rx;
        lcd_draw_pic_gray(x, y + y_oft, rw, 1, p);
    }
    return;
}

void lcd_ram_cpyimg(char *lcd, int lcdw, char *img, int imgw, int imgh, int x, int y)
{
    int i;
    for (i = 0; i < imgh; i++)
    {
        memcpy(lcd + lcdw * 2 * (y + i) + x * 2, img + imgw * 2 * i, imgw * 2);
    }
    return;
}
