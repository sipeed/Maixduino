#ifndef _VARIANT_SIPEED_MAIX_AMIGO
#define _VARIANT_SIPEED_MAIX_AMIGO

#include <stdint.h>

#define RISCV
#include "platform.h"

#include "Arduino.h"
#include "pwm.h"

#ifdef __cplusplus
#include "UARTClass.h"
extern class UARTHSClass Serial;
extern class UARTClass Serial1;
extern class UARTClass Serial2;
extern class UARTClass Serial3;
#endif

/* BOARD  PIN DEFINE */
/* UARTHS CH552T */
#define RX0                   4
#define TX0                   5
/* UART Grove */
#define RX1                   9 //Grove
#define TX1                   7 //Grove
/* 8P Connector */
// #define PB0                   8 //8P-12
// #define PB1                  12 //8P-3
/* SPI0 */
#define SPI0_MISO             6 //8P-11
#define SPI0_MOSI            10 //8P-2
#define SPI0_SCLK            11 //8P-1
#define SPI0_CS0             26 //TF
/* KEYs */
#define KEY2                 20 //Grove
#define KEY1                 23 //Grove      
/* LEDs */
#define PIN_LED_RED          14
#define PIN_LED_GREEN        15
#define PIN_LED_BLUE         17
#define PIN_LED              15
#define LED_RED              14
#define LED_GREEN            15
#define LED_BLUE             17
#define LED_BUILTIN          15
/* UART3 */
#define TX2                  22 //8P-4
#define RX2                  25 //8P-5
/* I2C1 - AXP173, MSA301, TOUCH */
#define SCL                  24
#define SDA                  27
/* USB_SBU */
#define USB_DP               28 //8P-8
#define USB_DM               29 //8P-9
/* I2S AUDIO CODEC ES8374 */
#define I2S2_MCLK            13
#define I2S2_WS              18
#define I2S2_SCLK            21
#define I2S2_DOUT            34
#define I2S2_DIN             35
/* Flash LED */
#define WLED_EN              32
/* TouchScreen */
#define CAP_TOUCH_IRQ        33
/* LCD */
#define LCD_TEN              19
#define LCD_CS               36
#define LCD_RST              37
#define LCD_DC               38
#define LCD_WR               39


#define MD_PIN_MAP(fpio)   (fpio)
#define ORG_PIN_MAP(org_pin)    (org_pin)

static const uint8_t SS   = SPI0_CS0 ;
static const uint8_t MOSI = SPI0_MOSI;
static const uint8_t MISO = SPI0_MISO;
static const uint8_t SCK  = SPI0_SCLK;

typedef struct _pwm_fpio_set_t{
    pwm_channel_number_t channel;
    pwm_device_number_t device;
    uint8_t inuse;
} pwm_fpio_set_t;


#define VARIANT_NUM_GPIOHS (32)
#define VARIANT_NUM_GPIO   ( 8)
#define VARIANT_NUM_PWM    (12)
#define VARIANT_NUM_I2C    ( 3)
#define VARIANT_NUM_SPI    ( 3)
#define VARIANT_NUM_UART   ( 3)

#endif
