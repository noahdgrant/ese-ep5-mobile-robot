/*******************************************************************************
* Name: DCMotor.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: March 24, 2023
* Description: DC motor functions.
*******************************************************************************/

#ifndef DCMOTOR_H
#define	DCMOTOR_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

#define DCMOTOR_LEFT    0UL
#define DCMOTOR_RIGHT	1UL
#define DCMOTOR_STOP    0UL
#define DCMOTOR_FWD     1UL
#define DCMOTOR_BWD     2UL

//Control Law Values
#define DCMOTOR_PRIORITY        9
#define I_GAIN                  1
#define P_GAIN                  1
#define GAIN_DIVISOR            1
#define FEEDBACK_SCALE_FACTOR   65536
#define MAX_DRIVE_VALUE         100
#define MIN_DRIVE_VALUE         50
#define STUPID_SPEED_ERROR      1
//needs one gain per wheel

extern uint32_t Global_EncoderPeriod[2];

void DCMotor_SetDir(uint8_t motor, uint8_t dir);
void DCMotor_SetPWM(uint8_t motor, uint16_t dutyCycle);

void DCMotor_Init(void);
void DCMotor_SetMotor(uint8_t motor, uint8_t dir, uint16_t velocity);
void DCMotor_SetMotors(uint8_t leftDir, uint16_t leftVelocity, uint8_t rightDir, uint16_t rightVelocity);

void DCMotor_Stop(void);
void DCMotor_Forward(uint16_t velocity);
void DCMotor_Backward(uint16_t velocity);

// Not implemented
void DCMotor_TurnLeft(void);
void DCMotor_TurnRight(void);
void DCMotor_ZeroTurnLeft(void);
void DCMotor_ZeroTurnRight(void);

#endif
