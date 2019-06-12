#ifndef _VARIANT_SIPEED_M1_DOCK
#define _VARIANT_SIPEED_M1_DOCK

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
/* LEDs */
#define PIN_LED_GREEN        13
#define PIN_LED_BLUE         12
#define PIN_LED_RED          14
#define PIN_LED              13
#define LED_BUILTIN          13
#define LED_GREEN            13
#define LED_BLUE             12
#define LED_RED              14
/* KEY */
#define KEY0                 16
/* MIC ARRAY */
#define MIC_BCK              18
#define MIC_WS               19
#define MIC_DAT3             20
#define MIC_DAT2             21
#define MIC_DAT1             22
#define MIC_DAT0             23
#define MIC_LED_DAT          24
/* SPI0 */
#define SPI0_CS1             25
#define SPI0_MISO            26
#define SPI0_SCLK            27
#define SPI0_MOSI            28
#define SPI0_CS0             29
/* I2S */
#define MIC0_WS              30
#define MIC0_DATA            31
#define MIC0_BCK             32
#define I2S_WS               33
#define I2S_DA               34
#define I2S_BCK              35
/* LCD */
#define LCD_CS               36
#define LCD_RST              37
#define LCD_DC               38
#define LCD_WR               39

#define RX0                   4
#define TX0                   5

#define RX1                   6
#define TX1                   7

#define SDA                  31
#define SCL                  30

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
}pwm_fpio_set_t;



#define VARIANT_NUM_GPIOHS (32)
#define VARIANT_NUM_GPIO   ( 8)
#define VARIANT_NUM_PWM    (12)
#define VARIANT_NUM_I2C    ( 3)
#define VARIANT_NUM_SPI    ( 3)
#define VARIANT_NUM_UART   ( 3)

#endif
