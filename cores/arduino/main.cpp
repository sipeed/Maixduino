
#define ARDUINO_MAIN
#include "Arduino.h"
#include "encoding.h"


#define cmb() __asm__ __volatile__ ("" ::: "memory")


/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
  pll_init();
  plic_init();
  setup();

  do {
    loop();
    //if (serialEventRun)
    //  serialEventRun();
  } while (1);

  return 0;
}
