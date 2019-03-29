
#define ARDUINO_MAIN
#include "Arduino.h"
#include "encoding.h"


#define cmb() __asm__ __volatile__ ("" ::: "memory")
extern "C"{ void * __dso_handle = 0 ;}

/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
  pll_init();
  plic_init();
  uarths_init();
  gpio_init();
  sysctl_set_power_mode(SYSCTL_POWER_BANK6,SYSCTL_POWER_V18);
	sysctl_set_power_mode(SYSCTL_POWER_BANK7,SYSCTL_POWER_V18);
  sysctl_enable_irq();
  setup();

  do {
    loop();
    //if (serialEventRun)
    //  serialEventRun();
  } while (1);

  return 0;
}
