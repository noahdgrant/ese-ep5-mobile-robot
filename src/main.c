/******************************************************************************
* Name: main.c (client)
* Author(s): Noah Grant, Wyatt Richard
* Date: April 14, 2023
* Description: Encoder control for mobile robot.
******************************************************************************/

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

int main(void){	
	// INITIALIZE
	uint8_t pressedKey = '\0';		// Key pressed by user
	int8_t RCServoAngle = 0;			// Servo angle
	uint8_t StepperMode = 0;			// Stepper mode (continuous or single output)
	uint8_t StepperLastStep = 0;	// The last step the servo took
	
	System_Clock_Init();					// Scale clock speed to 72MHz
	SystemCoreClockUpdate();
	
	UART2_Init();
	Stepper_Init();
	RCServo_Init();
	LED_Init();
	KeyPad_Init();
	Ultra_Init();
	DCMotor_Init();
	LCD_Init();
	Encoder_Init();
	
	// Print menu
	UART_printf("Embedded Systems Software Semester 4 Final Demonstration\n");
	UART_printf("Press a key on the keypad\n");

	// PROGRAM LOOP
	while(1){
		pressedKey = KeyPad_GetKey();

		switch(pressedKey){
			// STEPPER
			// OFF
			case '0':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 0");
				LCD_printf("\nStepper Off");

				StepperLastStep = 0;
				Stepper_Step(0);
				break;
			}
			// Full step CW
			case '1':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 1");
				LCD_printf("\nFull Step CW");

				StepperLastStep = 1;
				Stepper_Step(1);
				break;
			}
			// Full step CCW
			case '2':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 2");
				LCD_printf("\nFull Step CCW");
				StepperLastStep = 2;
				Stepper_Step(2);
				break;
			}
			// Half step CW
			case '3':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 3");
				LCD_printf("\nHalf Step CW");
				StepperLastStep = 3;
				Stepper_Step(3);
				break;
			}
			// Half step CCW
			case '4':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 4");
				LCD_printf("\nHalf Step CCW");
				StepperLastStep = 4;
				Stepper_Step(4);
				break;
			}
			// ping ultrasonic
			case '5':{
				Ultra_StartTrigger();
				while(!Ultra_EchoRx());
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 5");
				LCD_printf("\nUltrasonic: %dcm", Ultra_ReadSensor());
				break;
			}
			// 
			case '6':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 6");
				LCD_printf("\nIt's a button.");
				break;
			}
			// SERVO
			// Decrease servo angle
			case '7':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 7");
				LCD_printf("\nDec Servo Angle");
				RCServoAngle -= 5;
				RCServo_SetAngle(RCServoAngle);
				break;
			}
			// Centre servo
			case '8':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 8");
				LCD_printf("\nCentre Servo");
				RCServoAngle = 0;
				RCServo_SetAngle(RCServoAngle);
				break;
			}
			// Increase servo angle
			case '9':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: 9");
				LCD_printf("\nInc Servo Angle");
				RCServoAngle += 5;
				RCServo_SetAngle(RCServoAngle);
				break;
			}
			// Toggle between continuous output mode or single output mode
			case '#':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: #");
				LCD_printf("\nToggle Mode");
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
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: *");
				LCD_printf("\nToggle LED");
				LED_Toggle();
				break;
			}
			// dc motors forward
			case 'A':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: A");
				LCD_printf("\nDC Forward");
				DCMotor_Forward(100);
				break;
			}
			// dc motors off
			case 'B':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: B");
				LCD_printf("\nDC Stop");
				DCMotor_Stop();
				break;
			}
			// dc motors backwards 
			case 'C':{
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: C");
				LCD_printf("\nDC Backward");
				DCMotor_Backward(100);
				break;
			}
			// check encoder values
			case 'D':{
				Encoder_CalculateSpeed();
				LCD_Clear();
				LCD_HomeCursor();
				LCD_printf("User Input: D");
				LCD_printf("\nL: %d R: %d", Global_LeftEncoderPeriod, Global_RightEncoderPeriod);
				break;
			}
		}
		
		// Stepper continuous output mode
		if(StepperMode){
			Stepper_Step(StepperLastStep);
		}
	}
}
