/********************************************************************************
* Name: Ultrasonic.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: March 17, 2023
* Description: Ultrasonic sensor functions.
********************************************************************************/

#ifndef __Ultrasonic_H
#define __Ultrasonic_H

#include <stdint.h>
#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

void Ultra_Init(void);
void Ultra_StartTrigger(void);
uint8_t Ultra_EchoRx(void);
uint32_t Ultra_ReadSensor(void);

#endif
