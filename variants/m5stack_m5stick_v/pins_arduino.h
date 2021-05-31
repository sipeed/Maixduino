#ifndef _VARIANT_M5STACK_M5STICK_V
#define _VARIANT_M5STACK_M5STICK_V

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
/* UART */
#define RX0                   4
#define TX0                   5
/* LEDs */
#define PIN_LED_RED           6
#define PIN_LED_WHITE         7
#define PIN_LED_BLUE          8
#define PIN_LED_GREEN         9
#define PIN_LED               9
#define LED_RED               6
#define LED_WHITE             7
#define LED_BLUE              8
#define LED_GREEN             9
#define LED_BUILTIN           9
/* MIC ARRAY */
#define MIC_WS               10
#define MIC_DAT3             12
#define MIC_BCK              13
/* SPK MAX98357 */
#define I2S_SD               11
#define I2S_LRCLK            14
#define I2S_BCLK             15
#define I2S_DIN              17
/* LCD ST7789 135x240 */
#define LCD_SDA              18
#define LCD_SCL              19
#define LCD_DC               20
#define LCD_RST              21
#define LCD_CS               22
/* AXP192 INT PIN */
#define INTL_INT             23
/* INTL I2C - AXP192, MPU6886 */
#define SCL                  28
#define SDA                  29
/* SPI0 */
#define SPI0_SCLK            30
#define SPI0_MISO            31
#define SPI0_CS0             32
#define SPI0_MOSI            33
/* GROVE */
#define RX1                  34
#define TX1                  35
#define EXTL_SCL             34
#define EXTL_SDA             35
/* BUTTONS */
#define BTN_A                36
#define BTN_B                37
#define KEY0                 36
#define KEY1                 37

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
