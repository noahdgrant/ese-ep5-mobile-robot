/******************************************************************************
* Name: RCservo.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 10, 2023
* Description: RC servo motor conrol for mobile robot.
******************************************************************************/

#include "stm32f303xe.h"

#ifndef __SERVO_H
#define __SERVO_H

void RCServo_Init(void);
int16_t RCServo_SetAngle(int16_t angle);

#endif
