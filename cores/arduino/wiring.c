#include "Arduino.h"
#include "sysctl.h"
#include "sleep.h"

uint64_t millis(){
    return sysctl_get_time_us()/1000;
}
uint64_t micros(){
    return sysctl_get_time_us();
}
void delay(uint64_t dwMs){
    msleep(dwMs);
    return;
}
void delayMicroseconds(uint64_t dwUs){
    usleep(dwUs);
    return;
}

void pll_init(){
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 300000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    return;
}