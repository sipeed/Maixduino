
#include "Arduino.h"

#include <stdint.h>

#include "fpioa.h"
#include "gpiohs.h"
#include "gpio.h"
#include "wiring_constants.h"
#include "wiring_digital.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

void pinMode(uint8_t dwPin, uint8_t dwMode){           //48个fpio 对应32个gpiohs？
    fpioa_set_function(dwPin, FUNC_GPIOHS0 + dwPin);
    gpiohs_set_drive_mode(dwPin, (gpio_drive_mode_t)dwMode);
    return ;
}

void digitalWrite(uint8_t dwPin, uint8_t dwVal){
    gpiohs_set_pin(dwPin, (gpio_pin_value_t)dwVal);
    return ;
}

int digitalRead(uint8_t ulPin){
    return (int)gpiohs_get_pin(ulPin);
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus