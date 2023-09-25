/*******************************************************************************
* Name: PushButton.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 17, 2023
* Description: Push button control for mobile robot.
*******************************************************************************/

#ifndef __PushButton_H
#define __PushButton_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

void PushButton_Init(void);
uint8_t PushButton_PressCheck(void);

#endif
