
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

static int8_t _fpio_to_gpio_table[48]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

void pinMode(uint8_t dwPin, uint8_t dwMode){ 
    int gpionum = get_gpio(MD_PIN_MAP(dwPin));
    if(gpionum >= 0){
        fpioa_function_t function = FUNC_GPIOHS0 + gpionum;
        fpioa_set_function(MD_PIN_MAP(dwPin), function);
        gpiohs_set_drive_mode((uint8_t)gpionum, (gpio_drive_mode_t)dwMode);
    }
    return ;
}

void digitalWrite(uint8_t dwPin, uint8_t dwVal){
    int8_t gpio_pin = _fpio_to_gpio_table[MD_PIN_MAP(dwPin)];
    if( gpio_pin >= 0){
        gpiohs_set_pin((uint8_t)gpio_pin, (gpio_pin_value_t)dwVal);
    }
    return ;
}

int digitalRead(uint8_t dwPin){
    int8_t gpio_pin = _fpio_to_gpio_table[MD_PIN_MAP(dwPin)];
    if(gpio_pin >= 0){
        return (int)gpiohs_get_pin((uint8_t)gpio_pin);
    }
    return -1;
}

int get_gpio(uint8_t fpio_pin)
{
    if(_fpio_to_gpio_table[fpio_pin] >= 0){
        return (int)_fpio_to_gpio_table[fpio_pin];
    }else{
        _fpio_to_gpio_table[fpio_pin] = (int8_t)find_unused_gpiohs_io();
        return (int)_fpio_to_gpio_table[fpio_pin];        
    }
}

fpioa_function_t fpioa_get_function_by_io(uint8_t fpioPin)
{
    return (fpioa_function_t)fpioa->io[fpioPin].ch_sel ;
}

int find_unused_gpiohs_io(void) //返回一个未使用的gpio ，失败返回-1
{
    static fpioa_function_t function = FUNC_GPIOHS0;
    int _io ;
    if (function <= FUNC_GPIOHS31)
    {
        _io = (int)(function - FUNC_GPIOHS0);
        function++;
        return _io;
    }
    return -1;
}

int read_fpio_to_gpio_table(int number)
{
    return _fpio_to_gpio_table[number];
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus