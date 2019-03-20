/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UARTClass.h"

#include "pins_arduino.h"
#include "uarths.h"
#include "fpioa.h"

UARTClass Serial;

// Public Methods //////////////////////////////////////////////////////////////
UARTClass::UARTClass(void)
{
    _rxPin = RX;
    _txPin = TX;
}

UARTClass::UARTClass(uint8_t rxPin, uint8_t txPin)
{
  _rxPin = rxPin;
  _txPin = txPin;
}


void
UARTClass::begin(uint32_t bauds)
{
  fpioa_set_function((int)_rxPin, FUNC_UARTHS_RX);
  fpioa_set_function((int)_txPin, FUNC_UARTHS_TX);
  uarths_config(bauds, UARTHS_STOP_1);
}



void
UARTClass::end(void)                       //TODO k210 sdk not have api
{
  
}


int
UARTClass::available(void)                  //TODO Get the number of bytes (characters) available for reading from the serial port. 
{
  return(1);
}


int
UARTClass::availableForWrite(void)          //TODO Get the number of bytes (characters) available for writing from the serial port. 
{
  return (1);
}


int
UARTClass::peek(void)                        //TODO Returns the next byte (character) of incoming serial data without removing it from the internal serial buffer. 
{
    return (-1);
}


int
UARTClass::read(void)
{

  return (uarths_getc());
}


void
UARTClass::flush(void)                       //TODO Waits for the transmission of outgoing serial data to complete. 
{
}


size_t
UARTClass::write(const uint8_t uc_data)
{

  uarths_putchar(uc_data);
  return (1);
}
