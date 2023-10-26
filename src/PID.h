#ifndef PID_H
#define PID_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"
#include "Utility.h"
#include "DCMotor.h"

#define PID_PRIORITY 9

#define PID_L_KP 10
#define PID_L_KI 0
#define PID_R_KP 10
#define PID_R_KI 0
#define PID_LIM_MIN 0          // PWM
#define PID_LIM_MAX 100
#define PID_LIM_MIN_INT 0
#define PID_LIM_MAX_INT 100

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

void PID_Init(void);
int PID_Update(PIDController *pid, int setpoint, int measurement, int deltaT);

#endif
