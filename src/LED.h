/*******************************************************************************
* Name: LED.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: January 13, 2023
* Description: Nucleo board LED functions.
*******************************************************************************/

#ifndef LED_H
#define LED_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

void LED_Init(void);
void LED_Flash(uint32_t number_of_seconds);
void LED_Toggle(void);

#endif
