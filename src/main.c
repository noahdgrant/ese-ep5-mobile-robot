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
	
	int RCServoModifier = 0;

    // Startup menu
//    USART3_printf("--- Engineering Project 5 Mobile Robot ---\n");
//    USART3_printf("Robot initialized...\n");

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

            // DC motors
            case '0':{  // ljs N
                DCMotorLeftDir = DCMOTOR_FWD;
                DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '1':{  // ljs NW
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '2':{  // ljs NE
                DCMotorLeftDir = DCMOTOR_FWD;
                DCMotorRightDir = DCMOTOR_STOP;
                break;
            }
            case '3':{  // ljs W
                DCMotorLeftDir = DCMOTOR_BWD;
                DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '4':{  // ljs E
                DCMotorLeftDir = DCMOTOR_FWD;
                DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '5':{  // ljs S
                DCMotorLeftDir = DCMOTOR_BWD;
                DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '6':{  // ljs SW
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '7':{  // ljs SE
                DCMotorLeftDir = DCMOTOR_BWD;
                DCMotorRightDir = DCMOTOR_STOP;
                break;
            }
            case 'A':{ // ljs _
                DCMotorLeftDir = DCMOTOR_STOP;
                DCMotorRightDir = DCMOTOR_STOP;
                break;
            }

            // Speed
            case '8': { // ZR
                leftEncoderSetpoint += DCMOTOR_SPEED_INC;
                rightEncoderSetpoint += DCMOTOR_SPEED_INC;
                break;
            }
            case '9': { // ZL
                leftEncoderSetpoint -= DCMOTOR_SPEED_DEC;
                rightEncoderSetpoint -= DCMOTOR_SPEED_DEC;
                break;
            }

            // Servo
            case 'B':{  // R3
                RCServoAngle = SERVO_HOME;
                break;
            }
            case 'C':{  // rjs N
				RCServoModifier = 1;
                break;
            }
            case 'D':{  // rjs S
			RCServoModifier = -1;
                break;
            }

            // Stepper
            case 'E':{  // rjs E
				if(!LimitSwitch_PressCheck(RIGHT)){
					StepperStep = STEPPER_CW_FULL_STEP;
				}
                break;
            }
            case 'F':{  // rjs W
				if(!LimitSwitch_PressCheck(LEFT)){
					StepperStep = STEPPER_CCW_FULL_STEP;
				)
                break;
            }
            case 'G':{ // rjs _
                StepperStep = STEPPER_STOP;
                break;
            }

            // Ultrasonic
            case 'H':{
                Ultra_StartTrigger();
                while(!Ultra_EchoRx());
                USART3_printf("User Input: 5");
                USART3_printf("\nUltrasonic: %dcm", Ultra_ReadSensor());
                break;
            }
    
            // Invalid command
            default: {
                break;
            }
        }

		RCServoAngle+=RCServoModifier;
		
		if(RCServoAngle<-45){
			RCServoAngle = -45;
		}
		else if(RCServoAngle>45){
			RCServoAngle = 45;
		}
		
        DCMotor_SetDirs(DCMotorLeftDir, DCMotorRightDir);
        Stepper_Step(StepperStep);
        RCServo_SetAngle(RCServoAngle);
        Delay_ms(5);
    }
}
