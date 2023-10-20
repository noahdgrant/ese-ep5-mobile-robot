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
#include "PushButton.h"
#include "KeyPad.h"
#include "Ultrasonic.h"
#include "DCMotor.h"
#include "LCD.h"
#include "Encoder.h"
#include "LimitSwitch.h"
volatile uint8_t StepperLastStep = 0;	// The last step the stepper took

int main(void) {	
	// INITIALIZE

	int8_t RCServoAngle = 0;			// Servo angle
	uint8_t StepperMode = 0;			// Stepper mode (continuous or single output)

	
	System_Clock_Init();				// Scale clock speed to 72MHz
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
    PushButton_Init();
	LimitSwitch_Init();

	Stepper_Range();
    RCServo_SetAngle(-30);
	
	// Print menu
	USART3_printf("--- Engineering Project 5 Mobile Robot ---\n");
	USART3_printf("Press a key on the keypad\n");



    DCMotor_SetMotors(1, 50, 1, 50);


	// PROGRAM LOOP
	while(1){
		switch(USART3_dequeue()){
			// STEPPER
			// OFF
			case '0':{
				USART3_printf("User Input: 0\n");
				USART3_printf("Input new value for I gain");
                I_GAIN[0] = USART3_dequeue();
                I_GAIN[1] = I_GAIN[0];
				break;
			}
			// Full step CW
			case '1':{
				USART3_printf("User Input: 1");
                USART3_printf("Input new value for P gain");
                P_GAIN[0] = USART3_dequeue();
                P_GAIN[1] = P_GAIN[0];
                break;
			}
			// Full step CCW
			case '2':{
				USART3_printf("User Input: 2");
                USART3_printf("Input new value for I gain");
                GAIN_DIVISOR[0] = USART3_dequeue();
                GAIN_DIVISOR[1] = GAIN_DIVISOR[0];
				break;
			}
			// Half step CW
			case '3':{
				USART3_printf("User Input: 3");
                USART3_printf("Desired Velocity (cm/s)");
                int vel = USART3_dequeue();
                DCMotor_SetMotors(1, vel, 1, vel);
				break;
			}
			// Half step CCW
			case '4':{
				USART3_printf("User Input: 4");
				USART3_printf("\nHalf Step CCW");
				StepperLastStep = 4;
				Stepper_Step(4);
				break;
			}
			// ping ultrasonic
			case '5':{
				Ultra_StartTrigger();
				while(!Ultra_EchoRx());
				USART3_printf("User Input: 5");
				USART3_printf("\nUltrasonic: %dcm", Ultra_ReadSensor());
				break;
			}
			// 
			case '6':{
				USART3_printf("User Input: 6");
				USART3_printf("\nIt's a button.");
				break;
			}
			// SERVO
			// Decrease servo angle
			case '7':{
				USART3_printf("User Input: 7");
				USART3_printf("\nDec Servo Angle");
				RCServoAngle -= 5;
				RCServo_SetAngle(RCServoAngle);
				break;
			}
			// Centre servo
			case '8':{
				USART3_printf("User Input: 8");
				USART3_printf("\nCentre Servo");
				RCServoAngle = -30;
				RCServo_SetAngle(RCServoAngle);
				break;
			}
			// Increase servo angle
			case '9':{
				USART3_printf("User Input: 9");
				USART3_printf("\nInc Servo Angle");
				RCServoAngle += 5;
				RCServo_SetAngle(RCServoAngle);
				break;
			}
			// Toggle between continuous output mode or single output mode
			case '#':{
				USART3_printf("User Input: #");
				USART3_printf("\nToggle Mode");
				if(StepperMode == 1){
					StepperMode = 0;
				}
				else{
					StepperMode = 1;
				}
				break;
			}
			// LED
			case '*':{
				USART3_printf("User Input: *");
				USART3_printf("\nToggle LED");
				LED_Toggle();
				break;
			}
			// dc motors forward
			case 'A':{
				USART3_printf("User Input: A");
				USART3_printf("\nDC Forward");
				DCMotor_Forward(100);
				break;
			}
			// dc motors off
			case 'B':{
				USART3_printf("User Input: B");
				USART3_printf("\nDC Stop");
				DCMotor_Stop();
				break;
			}
			// dc motors backwards 
			case 'C':{
				USART3_printf("User Input: C");
				USART3_printf("\nDC Backward");
				DCMotor_Backward(100);
				break;
			}
			// check encoder values
			case 'D':{
				Encoder_CalculateSpeed();
				USART3_printf("User Input: D");
				USART3_printf("\nL: %d R: %d", Global_EncoderPeriod[0], Global_EncoderPeriod[1]);
				break;
			}
		}
		
		// Stepper continuous output mode
		if(StepperMode){
			Stepper_Step(StepperLastStep);
		}
	}
}
