
#ifndef _VARIANT_HBIRD_E200_
#define _VARIANT_HBIRD_E200_


#include <stdint.h>

#define LICHEETANG_HBIRD_E200_PLATFORM
#define HBIRD_E200
#define RISCV
#include "platform.h"


/*----------------------------------------------------------------------------
*        Headers
*----------------------------------------------------------------------------*/

#include "Arduino.h"
#ifdef __cplusplus
#include "UARTClass.h"
#endif


/* LEDs */
#define PIN_LED_13          13
#define PIN_LED             3
#define LED_BUILTIN         3

#ifdef __cplusplus
extern UARTClass Serial;
#endif

/*
 * SPI Interfaces
 */

#define SPI_INTERFACES_COUNT 1
#define SPI_REG(x) SPI1_REG(x)

#define UART_INTERFACES_COUNT 1
#define UART_REG(x) UART0_REG(x)
#define UART_REGP(i) _REG32P(UART0_CTRL_ADDR, (i))

// we only want to enable 3 peripheral managed SPI pins: SCK, MOSI, MISO
// CS pins can either be handled by hardware or bit banged as GPIOs

static const uint8_t SS   = PIN_SPI1_SS0;
static const uint8_t SS1  = PIN_SPI1_SS1;
static const uint8_t SS2  = PIN_SPI1_SS2;
static const uint8_t SS3  = PIN_SPI1_SS3;
static const uint8_t MOSI = PIN_SPI1_MOSI;
static const uint8_t MISO = PIN_SPI1_MISO;
static const uint8_t SCK  = PIN_SPI1_SCK;

static const uint32_t SPI_IOF_MASK = (1 << IOF_SPI1_SCK) | (1 << IOF_SPI1_MOSI) | (1 << IOF_SPI1_MISO);
static const uint32_t IOF_UART_MASK = IOF0_UART0_MASK;

#define VARIANT_DIGITAL_PIN_MAP  {{.io_port = 0, .bit_pos = 16, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 17, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 18, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 19, .pwm_num = 1,  .pwm_cmp_num = 1}, \
	{.io_port = 0, .bit_pos = 20, .pwm_num = 1,  .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 21, .pwm_num = 1,  .pwm_cmp_num = 2}, \
	{.io_port = 0, .bit_pos = 22, .pwm_num = 1,  .pwm_cmp_num = 3}, \
	{.io_port = 0, .bit_pos = 23, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 0,  .pwm_num = 0,  .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 1,  .pwm_num = 0,  .pwm_cmp_num = 1}, \
	{.io_port = 0, .bit_pos = 2,  .pwm_num = 0,  .pwm_cmp_num = 2}, \
	{.io_port = 0, .bit_pos = 3,  .pwm_num = 0, .pwm_cmp_num  = 3}, \
	{.io_port = 0, .bit_pos = 4,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 5,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 8,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 9,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 10, .pwm_num = 2,   .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 11, .pwm_num = 2,   .pwm_cmp_num = 1}, \
	{.io_port = 0, .bit_pos = 12, .pwm_num = 2,   .pwm_cmp_num = 2}, \
	{.io_port = 0, .bit_pos = 13, .pwm_num = 2,   .pwm_cmp_num = 3}}

#define VARIANT_NUM_PIN (20)

#define VARIANT_PWM_LIST {(volatile void *) PWM0_CTRL_ADDR, \
      (volatile void *) PWM1_CTRL_ADDR, \
      (volatile void *) PWM2_CTRL_ADDR}

#define VARIANT_NUM_PWM (3)
#define VARIANT_NUM_SPI (1)
// For interfacing with the onboard SPI Flash.
#define VARIANT_NUM_QSPI (1)
#define VARIANT_NUM_UART (1)

#endif 
