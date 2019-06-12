#ifndef _WIRING_DIGITAL_
#define _WIRING_DIGITAL_

#include "Arduino.h"
#include <stdint.h>
#include "platform.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#define PortValue_t uint32_t
#define PortAddr_t uint32_t
#define PortRegister_t volatile uint32_t *

#define digitalPinToPort(pin)    ((PortAddr_t)     GPIOHS_BASE_ADDR)
#define digitalPinToPortIn(pin)  ((PortAddr_t)     GPIOHS_BASE_ADDR)
#define digitalPinToBitMask(pin) ((PortValue_t)    (1<<pin))
#define portOutputRegister(port) ((PortRegister_t) (GPIOHS_BASE_ADDR + GPIOHS_OUTPUT_VAL))
#define portInputRegister(port)  ((PortRegister_t) (GPIOHS_BASE_ADDR + GPIOHS_INPUT_VAL))

/**
 * \brief Configures the specified pin to behave either as an input or an output. See the description of digital pins for details.
 *
 * \param ulPin The number of the pin whose mode you wish to set
 * \param ulMode Either INPUT or OUTPUT
 */
extern void pinMode( uint8_t dwPin, uint8_t dwMode ) ;

/**
 * \brief Write a HIGH or a LOW value to a digital pin.
 *
 * If the pin has been configured as an OUTPUT with pinMode(), its voltage will be set to the
 * corresponding value: 5V (or 3.3V on 3.3V boards) for HIGH, 0V (ground) for LOW.
 *
 * \note Digital pin PIN_LED is harder to use as a digital input than the other digital pins because it has an LED
 * and resistor attached to it that's soldered to the board on most boards. If you enable its internal 20k pull-up
 * resistor, it will hang at around 1.7 V instead of the expected 5V because the onboard LED and series resistor
 * pull the voltage level down, meaning it always returns LOW. If you must use pin PIN_LED as a digital input, use an
 * external pull down resistor.
 *
 * \param dwPin the pin number
 * \param dwVal HIGH or LOW
 */
extern void digitalWrite( uint8_t dwPin, uint8_t dwVal ) ;

/**
 * \brief Reads the value from a specified digital pin, either HIGH or LOW.
 *
 * \param dwPin The number of the digital pin you want to read (int)
 *
 * \return HIGH or LOW
 */
extern int digitalRead( uint8_t dwPin ) ;

int get_gpio(uint8_t fpio_pin) ;
fpioa_function_t fpioa_get_function_by_io(uint8_t fpioPin) ;
int find_unused_gpiohs_io(void) ;
int read_fpio_to_gpio_table(int number);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* _WIRING_DIGITAL_ */