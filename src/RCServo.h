/*******************************************************************************
* Name: RCservo.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 10, 2023
* Description: RC servo motor conrol for mobile robot.
*******************************************************************************/

#ifndef __SERVO_H
#define __SERVO_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"
#include "Utility.h"

#define SERVO_HOME -30
#define SERVO_INCREASE 1
#define SERVO_DECREASE -1
#define SERVO_MAX 45
#define SERVO_MIN -45
#define SERVO_STOP 0

extern int G_RCServoAngle;
extern int G_RCServoModifier;

void RCServo_Init(void);
void RCServo_SetAngle(int16_t angle);

#endif
