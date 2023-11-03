/*******************************************************************************
* Name: LimitSwitch.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: September 15, 2023
* Description: Limit switch functions.
*******************************************************************************/

#ifndef LIMITSWITCH_H
#define LIMITSWITCH_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"
#include "Utility.h"

void LimitSwitch_Init(void);
uint8_t LimitSwitch_PressCheck(uint8_t direction);

#endif
