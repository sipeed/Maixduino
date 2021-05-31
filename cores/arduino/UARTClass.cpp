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

#include "Arduino.h"
#include "uarths.h"
#include "fpioa.h"
#include "sysctl.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
UARTHSClass Serial;
UARTClass Serial1(UART_DEVICE_1);
UARTClass Serial2(UART_DEVICE_2);
UARTClass Serial3(UART_DEVICE_3);
#endif

static int uart_rec_callback(void *ctx);
static int uarths_rec_callback(void *ctx);

// Public Methods //////////////////////////////////////////////////////////////
UARTClass::UARTClass()
{

}

UARTClass::UARTClass(uart_device_number_t device_select)
{
  this->_uart = device_select;
  switch(this->_uart){
    case UART_DEVICE_1:
      this->_rxfunc = FUNC_UART1_RX;
      this->_txfunc = FUNC_UART1_TX;
      break;
    case UART_DEVICE_2:
      this->_rxfunc = FUNC_UART2_RX;
      this->_txfunc = FUNC_UART2_TX;
      break;
    case UART_DEVICE_3:
      this->_rxfunc = FUNC_UART3_RX;
      this->_txfunc = FUNC_UART3_TX;
      break;
    case UART_DEVICE_MAX:
      break;
  }
}

void 
UARTClass::begin(uint32_t dwBaudRate)
{
  begin(dwBaudRate, RX1, TX1);
}

void
UARTClass::begin(uint32_t dwBaudRate, uint8_t _rx, uint8_t _tx)
{
  fpioa_set_function((int)MD_PIN_MAP(_rx), this->_rxfunc);
  fpioa_set_function((int)MD_PIN_MAP(_tx), this->_txfunc);
  uart_init(this->_uart);
  uart_configure(this->_uart, dwBaudRate, UART_BITWIDTH_8BIT, UART_STOP_1, UART_PARITY_NONE);
  this->_buff = new RingBuffer();
  uart_set_receive_trigger(this->_uart, UART_RECEIVE_FIFO_1);
  uart_irq_register(this->_uart, UART_RECEIVE, uart_rec_callback, this, 5);
  sysctl_enable_irq();
}

void
UARTClass::end(void)                       
{
  delete this->_buff;
  uart_irq_unregister(this->_uart, UART_RECEIVE);
}

int
UARTClass::available(void)                  
{
  return this->_buff->available();
}

int
UARTClass::availableForWrite(void)          //TODO Get the number of bytes (characters) available for writing from the serial port. 
{
  return (1);
}

int
UARTClass::peek(void)                        
{
    return this->_buff->peek();
}

int
UARTClass::read(void)
{
  while(this->_buff->available()){
    return this->_buff->read_char();
  }
  return -1;
}

void
UARTClass::flush(void)                       
{
  this->_buff->clear();
}

size_t
UARTClass::write(const uint8_t c)
{
  while (uart[this->_uart]->LSR & (1u << 5))
        continue;
  uart[this->_uart]->THR = c;
  return 1;
}

static int 
uart_rec_callback(void *ctx)
{
  char data;
  auto &driver = *reinterpret_cast<UARTClass *>(ctx); 
  while(uart_receive_data(driver._uart, &data, 1)){
    driver._buff->store_char(data);
  }
  return 0;
}

UARTHSClass::UARTHSClass()
{

}

void 
UARTHSClass::begin(uint32_t dwBaudRate)
{
  begin(dwBaudRate, RX0, TX0);
}

void 
UARTHSClass::begin(uint32_t dwBaudRate, uint8_t _rx, uint8_t _tx)
{
  fpioa_set_function((int)MD_PIN_MAP(_rx), FUNC_UARTHS_RX);
  fpioa_set_function((int)MD_PIN_MAP(_tx), FUNC_UARTHS_TX);
  uarths_init();
  uarths_config(dwBaudRate, UARTHS_STOP_1);
  this->_buff = new RingBuffer();
  this->_buff->clear();
  uarths_set_interrupt_cnt(UARTHS_RECEIVE, 0);
  uarths_set_irq(UARTHS_RECEIVE, uarths_rec_callback, this,1);
  sysctl_enable_irq();
}

void
UARTHSClass::end()
{
  delete _buff;
  plic_irq_unregister(IRQN_UARTHS_INTERRUPT);
}

size_t
UARTHSClass::write(const uint8_t uc_data)
{
  if (uarths_putchar(uc_data) == uc_data) {
    return 1;
  }

  return 0;
}

static int
uarths_rec_callback(void *ctx)
{
  int data;
  auto &driver = *reinterpret_cast<UARTHSClass *>(ctx);
  data = uarths_getchar();
  if(data != EOF){
    driver._buff->store_char((char)data);
  }
  return 0;
}
