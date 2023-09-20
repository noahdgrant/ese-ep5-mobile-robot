/********************************************************************************
* Name: UART.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: January 25, 2023
* Description: UART functions to initialize, configure, and Tx/Rx.
********************************************************************************/

#ifndef __UART_H
#define __UART_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

// UART setup
void USART2_Init(void);
void USART3_Init(void);

// UART I/O
void USART2_putc(char c);
void USART2_puts(char *str);
char USART2_getc(void);
char USART2_getcNB(void);
void USART2_printf(char *format, ...);

void USART3_putc(char c);
void USART3_puts(char *str);
char USART3_getc(void);
char USART3_getcNB(void);
void USART3_printf(char *format, ...);
#endif
