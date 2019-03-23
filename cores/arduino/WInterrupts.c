#include "Arduino.h"
#include "WInterrupts.h"
#include "wiring_constants.h"
#include "plic.h"

void attachInterrupt(uint8_t intnum, voidFuncPtr callback, uint8_t mode)
{
    plic_irq_callback_t _callback = (plic_irq_callback_t)callback;
    int gpionum = getGpio(intnum);
    if(gpionum >= 0){
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
        gpiohs_irq_register((uint8_t)gpionum, 10, _callback, NULL);
        sysctl_enable_irq();
    }
}

void detachInterrupt(uint8_t intnum)
{
    int gpionum = getGpio(intnum);
    if(gpionum >= 0){
        gpiohs_irq_unregister((uint8_t)gpionum);
    }
}
