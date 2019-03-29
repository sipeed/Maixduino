/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

// some libraries and sketches depend on this
// AVR stuff, assuming Arduino.h or WProgram.h
// automatically includes it...
#include <avr/pgmspace.h>
#include <avr/dtostrf.h>
//#include <avr/interrupt.h>

#include "binary.h"
#include "pins_arduino.h"

#include "fpioa.h"
#include "gpiohs.h"
#include "gpio.h"
#include "gpio_common.h"
#include "plic.h"
#include "uarths.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
#include "wiring.h"
#include "wiring_constants.h"

#define	SystemCoreClock F_CPU

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( (a) * (SystemCoreClock / 1000000L) )

void yield(void);

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

typedef void (*voidFuncPtr)( void ) ;

/* Define attribute */
#define WEAK __attribute__ ((weak))

#ifdef __cplusplus
} // extern "C"

#include "WCharacter.h"
#include "WString.h"
#include "WMath.h"
#include "HardwareSerial.h"
#include "wiring_pulse.h"
#include "Client.h"
#include "HardwareI2C.h"
#include "IPAddress.h"
#include "Server.h"
#include "Udp.h"

// Tone function prototypes
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0);
void noTone(uint8_t _pin);

#endif // __cplusplus

#include "wiring_digital.h"
#include "wiring_analog.h"
#include "wiring_shift.h"
#include "WInterrupts.h"

#endif // Arduino_h
