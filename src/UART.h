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
void UART3_Init(void);

// UART I/O
void UART2_putc(char c);
void UART2_puts(char *str);
char UART2_getc(void);
char UART2_getcNB(void);
void UART2_printf(char *format, ...);

void UART3_putc(char c);
void UART3_puts(char *str);
char UART3_getc(void);
char UART3_getcNB(void);
void UART3_printf(char *format, ...);
#endif
