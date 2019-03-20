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

#ifndef _UART_CLASS_
#define _UART_CLASS_

#include "platform.h"
#include "HardwareSerial.h"
#include "uarths.h"
#include "uart.h"

class UARTClass : public HardwareSerial
{
  public:
    UARTClass(uint8_t rxPin, uint8_t txPin);
    UARTClass(void);
    void begin(uint32_t dwBaudRate);
    void end(void);
    int available(void);
    int availableForWrite(void);
    int peek(void);
    int read(void);
    void flush(void);
    size_t write(const uint8_t c);
    using Print::write; // pull in write(str) and write(buf, size) from Print

    operator bool() {return (true);}; // UART always active

  protected:
    uint8_t _rxPin;
    uint8_t _txPin;
    
};
extern int uarths_putchar(char c);
extern int uarths_getc(void);

#endif // _UART_CLASS_
