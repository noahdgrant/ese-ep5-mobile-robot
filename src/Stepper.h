/*******************************************************************************
* Name: Stepper.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 3, 2023
* Description: Stepper motor control.
*******************************************************************************/

#ifndef __Stepper_H
#define __Stepper_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"
#include "Utility.h"
#include "UART.h"
#include "LimitSwitch.h"
#include "Ultrasonic.h"

#define STEPPER_STOP 0
#define STEPPER_CW_FULL_STEP 1
#define STEPPER_CCW_FULL_STEP 2
#define STEPPER_CW_HALF_STEP 3
#define STEPPER_CCW_HALF_STEP 4

extern volatile uint8_t G_StepperStep;

void Stepper_Init(void);
void Stepper_Step(uint8_t stepType);
uint8_t Stepper_Range(void);

#endif
