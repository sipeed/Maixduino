#ifndef _VARIANT_SIPEED_MAIX_BIT
#define _VARIANT_SIPEED_MAIX_BIT

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
/* UARTHS */
#define RX0                   4
#define TX0                   5
/* UART */
#define RX1                   6
#define TX1                   7

// #define IO_8                  8
// #define IO_9                  9
// #define IO_10                10
/* LEDs */
#define PIN_LED_GREEN        12
#define PIN_LED_RED          13
#define PIN_LED_BLUE         14
#define PIN_LED              12
#define LED_RED              13
#define LED_GREEN            12
#define LED_BLUE             14
#define LED_BUILTIN          12
// #define IO_15                15
// #define IO_17                17

/* I2S MIC */
#define MIC_BCK              18
#define MIC_WS               19
#define MIC_DAT3             20

// #define IO_21                21
// #define IO_22                22
// #define IO_23                23
// #define IO_24                24
// #define IO_25                25
/* SPI0 */
#define SPI0_MISO            26
#define SPI0_SCLK            27
#define SPI0_MOSI            28
#define SPI0_CS0             29

// #define SCL                  30
// #define SDA                  31
// #define IO_30                30
// #define IO_31                31
// #define IO_32                32
// #define IO_33                33
// #define IO_34                34
// #define IO_35                35
/* LCD */
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
