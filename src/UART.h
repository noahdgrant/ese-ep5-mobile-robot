/********************************************************************************
* Name: UART.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: January 25, 2023
* Description: UART functions to initialize, configure, and Tx/Rx.
********************************************************************************/

#ifndef __UART_H
#define __UART_H

#include "stm32f303xe.h"

// UART setup
void UART2_Init(void);

// UART I/O
void UART_putc(char c);
void UART_puts(char *str);
char UART_getc(void);
char UART_getcNB(void);
void UART_printf(char *format, ...);

#endif
