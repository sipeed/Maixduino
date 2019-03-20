#include "Arduino.h"
#include "WInterrupts.h"
#include "wiring_constants.h"
#include "plic.h"

void attachInterrupt(uint8_t intnum, voidFuncPtr callback, uint8_t mode)
{
    plic_irq_callback_t _callback = (plic_irq_callback_t)callback;
    switch(mode){
        case LOW : 
            gpiohs_set_pin_edge(intnum, GPIO_PE_LOW);
            break;
        case HIGH :
            gpiohs_set_pin_edge(intnum, GPIO_PE_HIGH);
            break;
        case FALLING :
            gpiohs_set_pin_edge(intnum, GPIO_PE_FALLING);
            break; 
        case CHANGE :
            gpiohs_set_pin_edge(intnum, GPIO_PE_BOTH);
            break;
        case RISING :
            gpiohs_set_pin_edge(intnum, GPIO_PE_RISING);
            break;
    }
    gpiohs_irq_register(intnum, 1, _callback, NULL);
    sysctl_enable_irq();

}

void detachInterrupt(uint8_t intnum)
{
    gpiohs_irq_unregister(intnum);

}