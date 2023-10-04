/*******************************************************************************
* Name: Ultrasonic.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: October 4, 2023
* Description: Ultrasonic sensor functions.
*******************************************************************************/

#ifndef __Ultrasonic_H
#define __Ultrasonic_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

void Ultra_Init(void);
void Ultra_StartTrigger(void);
uint32_t Ultra_ReadSensor(void);

#endif
