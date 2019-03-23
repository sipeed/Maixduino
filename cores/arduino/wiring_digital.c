
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

void pinMode(uint8_t dwPin, uint8_t dwMode){ 
    int gpionum = getGpio(dwPin);
    if(gpionum >= 0){
        fpioa_function_t function = FUNC_GPIOHS0 + gpionum;
        fpioa_set_function(dwPin, function);
        gpiohs_set_drive_mode((uint8_t)gpionum, (gpio_drive_mode_t)dwMode);
        gpiohs_set_pin((uint8_t)gpionum, GPIO_PV_LOW);
    }
    return ;
}

void digitalWrite(uint8_t dwPin, uint8_t dwVal){
    int gpionum = getGpio_s(dwPin);
    if(gpionum >= 0){
        gpiohs_set_pin((uint8_t)gpionum, (gpio_pin_value_t)dwVal);
    }
    return ;
}

int digitalRead(uint8_t dwPin){
    int gpionum = getGpio_s(dwPin);
    if(gpionum >= 0){
        return (int)gpiohs_get_pin((uint8_t)gpionum);
    }
    return -1;
}

int getGpio(uint8_t fpioPin) //分配一个gpio给fpio 输入 fpio 返回 gpiohs号
{
    fpioa_function_t function = fpioa_get_function_buy_io(fpioPin);
    if(function <= FUNC_GPIOHS31 && function >= FUNC_GPIOHS0 )
    {
        return (int)(function - FUNC_GPIOHS0);
    }else{
        return find_unused_gpiohs_io();        
    }
}

int getGpio_s(uint8_t fpioPin) 
{
    fpioa_function_t function = fpioa_get_function_buy_io(fpioPin);
    if(function <= FUNC_GPIOHS31 && function >= FUNC_GPIOHS0 )
    {
        return (int)(function - FUNC_GPIOHS0);
    }else{
        return -1;        
    }
}

fpioa_function_t fpioa_get_function_buy_io(uint8_t fpioPin)
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

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus