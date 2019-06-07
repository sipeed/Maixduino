#include "Arduino.h"
#include "WInterrupts.h"
#include "wiring_constants.h"
#include "plic.h"

void attachInterrupt(uint8_t intnum, voidFuncPtr user_callback, uint8_t mode)
{
    int gpionum = get_gpio(MD_PIN_MAP(intnum));
    if(gpionum >= 0){
        fpioa_function_t function = FUNC_GPIOHS0 + gpionum;
        fpioa_set_function(intnum, function);
        gpiohs_set_drive_mode(gpionum, GPIO_DM_INPUT_PULL_UP);
        switch(mode){
            case LOW : 
                gpiohs_set_pin_edge((uint8_t)gpionum, GPIO_PE_LOW);
                break;
            case HIGH :
                gpiohs_set_pin_edge((uint8_t)gpionum, GPIO_PE_HIGH);
                break;
            case FALLING :
                gpiohs_set_pin_edge((uint8_t)gpionum, GPIO_PE_FALLING);
                break; 
            case CHANGE :
                gpiohs_set_pin_edge((uint8_t)gpionum, GPIO_PE_BOTH);
                break;
            case RISING :
                gpiohs_set_pin_edge((uint8_t)gpionum, GPIO_PE_RISING);
                break;
        }
        gpiohs_irq_register((uint8_t)gpionum, 10, gpiohs_callback, user_callback);
        sysctl_enable_irq();
    }
}

void detachInterrupt(uint8_t intnum)
{
    int gpionum = get_gpio(MD_PIN_MAP(intnum));
    if(gpionum >= 0){
        gpiohs_irq_unregister((uint8_t)gpionum);
    }
}

int gpiohs_callback(void *ctx)
{
    voidFuncPtr user_callback = ctx;
    user_callback();
    return 0;
}
