/*******************************************************************************
* Name: Encoder.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: April 14, 2023
* Description: Encoder functions for mobile robot.
*******************************************************************************/

#ifndef __MOTOR_ENCODER_H
#define __MOTOR_ENCODER_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"
#include "Utility.h"

#define ENCODER_PRIORITY    9
#define MAX_TIME_US         65536   // 2^16 us
#define UM_PER_VANE         274     // um/encoder vane 

void Encoder_Init(void);
void Encoder_CalculateSpeed(void);

extern volatile uint32_t Global_EncoderPeriod[2];
extern volatile uint32_t leftEncoderSpeed;
extern volatile uint32_t rightEncoderSpeed;
extern int leftEncoderSetpoint;
extern int rightEncoderSetpoint;

#endif
