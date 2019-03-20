#ifndef _WIRING_
#define _WIRING_

#include "sysctl.h"
#include "sleep.h"
/**
 * \brief Returns the number of milliseconds since the board began running the current program.
 *
 * \return Number of milliseconds since the program started (uint64_t)
 */
extern uint64_t millis(void);

/**
 * \brief Returns the number of microseconds since the board began running the current program.
 *
 * \note There are 1,000 microseconds in a millisecond and 1,000,000 microseconds in a second.
 */
extern uint64_t micros(void);

/**
 * \brief Pauses the program for the amount of time (in miliseconds) specified as parameter.
 * (There are 1000 milliseconds in a second.)
 *
 * \param dwMs the number of milliseconds to pause (uint64_t)
 */
extern void delay(uint64_t dwMs);

extern void delayMicroseconds(uint64_t dwUs);

void pll_init(void);



#endif