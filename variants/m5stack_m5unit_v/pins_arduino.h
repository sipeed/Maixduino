#ifndef _VARIANT_M5STACK_M5UNIT_V
#define _VARIANT_M5STACK_M5UNIT_V

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
/* WS2812 LEDs */
#define PIN_LED_RGB           8

/* MIC ARRAY */
#define MIC_WS               10
#define MIC_DAT3             12
#define MIC_BCK              13
/* SPK MAX98357 */
#define SPK_LRCLK            14
#define SPK_BCLK             15
#define SPK_DIN              17
#define SPK_SD               25
/* BUTTONS */
#define BTN_A                18
#define BTN_B                19
#define KEY0                 18
#define KEY1                 19
/* AXP192 INT PIN */
#define AXP_INT              23
/* INTL I2C */
#define SCL                  28
#define SDA                  29
/* SPI0 */
#define SPI0_SCLK            30
#define SPI0_MISO            31
#define SPI0_CS0             32
#define SPI0_MOSI            33
/* EXTL I2C */
#define EXTL_SCL             34
#define EXTL_SDA             35


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
