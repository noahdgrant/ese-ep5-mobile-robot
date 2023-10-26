#include "stdlib.h"

#include "PID.h"
#include "DCMotor.h"
#include "Encoder.h"

// Code based on: https://github.com/pms67/PID
// https://github.com/curiores/ArduinoTutorials/blob/main/SpeedControl/SpeedControl/SpeedControl.ino

PIDController PIDLeftEncoder, PIDRightEncoder;

static void PID_ControllerInit(PIDController *pid) {
	// Clear controller variables
	pid->integrator = 0;
	pid->prevError  = 0;
    pid->deltaT = 0;
	pid->out = 0;
}

void PID_Init(void) {
    // Initialize PID controllers
    PIDController PIDLeftEncoder = {PID_L_KP, PID_L_KI, PID_LIM_MIN, PID_LIM_MAX,
                                    PID_LIM_MIN_INT, PID_LIM_MAX_INT};

    PIDController PIDRightEncoder = {PID_R_KP, PID_R_KI, PID_LIM_MIN, PID_LIM_MAX,
                                    PID_LIM_MIN_INT, PID_LIM_MAX_INT};

    PID_ControllerInit(&PIDLeftEncoder);
    PID_ControllerInit(&PIDRightEncoder);
    
    // Configure TIM4 for PI control law
    SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM4EN);         // Turn on Timer 4
    SET_BITS(TIM4->PSC, 71UL);                          // Set PSC so it counts in 1us
    CLEAR_BITS(TIM4->CR1, TIM_CR1_DIR);                 // Set TIM4 counting direction to upcounting
    FORCE_BITS(TIM4->ARR, 0xFFFFUL, 33332UL);           // Set ARR to 33333us
    SET_BITS(TIM4->CR1, TIM_CR1_ARPE);                  // Enable ARR preload (ARPE) in CR1
    //SET_BITS(TIM4->BDTR, TIM_BDTR_MOE);               // Set main output enabled (MOE) in BDTR
    CLEAR_BITS(TIM4->CR1, TIM_CR1_UDIS);                // Set update request source
    CLEAR_BITS(TIM4->CR1, TIM_CR1_URS);                 // Enable update
    SET_BITS(TIM4->DIER, TIM_DIER_UIE);                 // Enable timer overflow to trigger IRQ
    NVIC_EnableIRQ(TIM4_IRQn);                          // Enable TIM4 IRQ (TIM4_IRQn) in NVIC
    NVIC_SetPriority(TIM4_IRQn, PID_PRIORITY);          // Set NVIC priority
    SET_BITS(TIM4->EGR, TIM_EGR_UG);                    // Force an update event to preload all the registers
    SET_BITS(TIM4->CR1, TIM_CR1_CEN);                   // Enable TIM4 to start counting
}

int PID_Update(PIDController *pid, int setpoint, int measurement, int deltaT) {
    // Error
    int error = setpoint - measurement;

    
    // Proportional
    int proportional = error;

    
    // Integrator
    pid->integrator = pid->integrator + error * deltaT; // (pid->Ki * pid->deltaT * (error + pid->prevError)) / 2;

	// Anti-wind-up via integrator clamping
    if (pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    }
    else if (pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }

	
    // Compute output and apply limits
    pid->out = pid->Kp * proportional + pid->Ki * pid->integrator;

    if (pid->out > pid->limMax) {
        pid->out = pid->limMax;
    }
    else if (pid->out < pid->limMin) {
        pid->out = pid->limMin;
    }

    
    // Store error and measurement for later use
    //pid->prevError = error;

	
    // Return controller output
    return pid->out;
}

void TIM4_IRQHandler(void) {
    // Update PWM outputs
    PID_Update(&PIDLeftEncoder, leftEncoderSetpoint, leftEncoderSpeed, Global_EncoderPeriod[0]);
    DCMotor_SetPWM(DCMOTOR_LEFT, abs(PIDLeftEncoder.out));

    PID_Update(&PIDRightEncoder, rightEncoderSetpoint, rightEncoderSpeed, Global_EncoderPeriod[1]);
    DCMotor_SetPWM(DCMOTOR_RIGHT, abs(PIDRightEncoder.out));

    // Clear interrupt flag
    CLEAR_BITS(TIM4->SR, TIM_SR_UIF);
}

