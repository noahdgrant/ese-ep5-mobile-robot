/*******************************************************************************
* Name: main.c (client)
* Author(s): Noah Grant, Wyatt Richard
* Date: September 15, 2023
* Description: Encoder control for mobile robot.
*******************************************************************************/

#include "SysClock.h"
#include "Utility.h"
#include "UART.h"
#include "Stepper.h"
#include "RCServo.h"
#include "LED.h"
#include "KeyPad.h"
#include "Ultrasonic.h"
#include "DCMotor.h"
#include "LCD.h"
#include "Encoder.h"
#include "LimitSwitch.h"
#include "PID.h"

int main(void) {	
	// INITIALIZE
	System_Clock_Init();
	SystemCoreClockUpdate();
	
	USART3_Init();
	Stepper_Init();
	RCServo_Init();
	LED_Init();
	KeyPad_Init();
	Ultra_Init();
	DCMotor_Init();
	LCD_Init();
	Encoder_Init();
	LimitSwitch_Init();
    PID_Init();

	Stepper_Range();
    RCServo_SetAngle(SERVO_HOME);
	
	// Startup menu
	USART3_printf("--- Engineering Project 5 Mobile Robot ---\n");
	USART3_printf("Robot initialized...\n");


	// PROGRAM LOOP
	while(1) {
        switch(USART3_dequeue()){
            // Stop robot
            case 'S':{
                StepperStep = STEPPER_STOP;
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_STOP;
                leftEncoderSetpoint = 0;
                rightEncoderSetpoint = 0;
                break;
            }

            // Stepper
            case '0':{
                StepperStep = STEPPER_CW_FULL_STEP;
                break;
            }
            case '1':{
                StepperStep = STEPPER_CCW_FULL_STEP;
                break;
            }
            case '2':{
                StepperStep = STEPPER_STOP;
                break;
            }

            // Servo
            case '3':{
                RCServoAngle -= SERVO_DECREASE;
                break;
            }
            case '4':{
                RCServoAngle += SERVO_INCREASE;
                break;
            }
            case '5':{
                RCServoAngle = SERVO_HOME;
                break;
            }
            
            // DC motors
            case '6':{
                DCMotorLeftDir = DCMOTOR_FWD;
                DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '7':{
                DCMotorLeftDir = DCMOTOR_BWD;
                DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '8':{
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '9':{
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case 'q':{
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_STOP;
                break;
            }

            // Speed
            case 'w': {
                leftEncoderSetpoint += DCMOTOR_SPEED_INC;
                break;
            }
            case 'e': {
                leftEncoderSetpoint -= DCMOTOR_SPEED_DEC;
                break;
            }
            case 'r': {
                rightEncoderSetpoint += DCMOTOR_SPEED_INC;
                break;
            }
            case 't': {
                rightEncoderSetpoint -= DCMOTOR_SPEED_DEC;
                break;
            }

            // Ultrasonic
            case 'A':{
                Ultra_StartTrigger();
                while(!Ultra_EchoRx());
                USART3_printf("User Input: 5");
                USART3_printf("\nUltrasonic: %dcm", Ultra_ReadSensor());
                break;
            }
        }

        DCMotor_SetDirs(DCMotorLeftDir, DCMotorRightDir);
        Stepper_Step(StepperStep);
        RCServo_SetAngle(RCServoAngle);

        USART3_printf("Left: s = %d, m= %d, p = %d, pwm = %d\n", leftEncoderSetpoint, leftEncoderSpeed, Global_EncoderPeriod[LEFT], PIDLeftEncoder.out);
        USART3_printf("Right: s = %d, m = %d, p = %d, pwm = %d\n", rightEncoderSetpoint, rightEncoderSpeed, Global_EncoderPeriod[RIGHT], PIDRightEncoder.out);
	}
}
	
