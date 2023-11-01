/*******************************************************************************
* Name: PID.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: October 31, 2023
* Description: PID control.
*******************************************************************************/

#ifndef PID_H
#define PID_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"
#include "Utility.h"
#include "DCMotor.h"
#include "Encoder.h"

typedef struct {
	// Controller gains
	int Kp;
	int Ki;

	// Output limits
	int limMin;
	int limMax;
	
	// Integrator limits
	int limMinInt;
	int limMaxInt;

	// Time between encoder interrupts (deltaT)
	int deltaT;

	// Controller "memory"
	int integrator;
	int prevError;

	// Controller output
	int out;

} PIDController;

extern PIDController PIDLeftEncoder;
extern PIDController PIDRightEncoder;

void PID_Init(void);
int PID_Update(PIDController *pid, int setpoint, int measurement, int deltaT);

#endif
