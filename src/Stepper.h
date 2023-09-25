/*******************************************************************************
* Name: Stepper.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 3, 2023
* Description: Stepper motor control.
*******************************************************************************/

#ifndef __Stepper_H
#define __Stepper_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

void Stepper_Init(void);
void Stepper_Step(uint8_t stepType);

#endif
