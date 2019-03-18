#ifndef _WIRING_SHIFT_
#define _WIRING_SHIFT_

#include <stdint.h>
#include "Arduino.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
/*
 * \brief
 */
extern uint8_t shiftIn( uint8_t ulDataPin, uint8_t ulClockPin, uint8_t ulBitOrder ) ;


/*
 * \brief
 */
extern void shiftOut( uint8_t ulDataPin, uint8_t ulClockPin, uint8_t ulBitOrder, uint8_t ulVal ) ;

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* _WIRING_SHIFT_ */