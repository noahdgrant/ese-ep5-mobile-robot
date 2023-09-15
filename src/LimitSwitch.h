/********************************************************************************
* Name: LimitSwitch.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: September 15, 2023
* Description: Camera Module Limit Switch functions.
********************************************************************************/

#ifndef LIMITSWITCH_H
#define LIMITSWITCH_H

#include "stm32f303xe.h"
void LimitSwitch_Init(void);
uint8_t LimitSwitch_PressCheck(void);

#endif //LIMITSWITCH_H