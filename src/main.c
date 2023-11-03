/*******************************************************************************
* Name: main.c (client)
* Author(s): Noah Grant, Wyatt Richard
* Date: November 3, 2023
* Description: Mobile robot main control loop.
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

    // PROGRAM LOOP
    while (1) {
        Ultra_StartTrigger();
        
        switch (USART3_dequeue()) {
            // Stop robot
            case 'S': {
                G_StepperStep = STEPPER_STOP;
                G_DCMotorLeftDir = DCMOTOR_STOP;
                G_DCMotorRightDir = DCMOTOR_STOP;
                G_leftEncoderSetpoint = DCMOTOR_SPEED_BASE;
                G_rightEncoderSetpoint = DCMOTOR_SPEED_BASE;
                break;
            }

            // DC motors
            case '0': {
                G_DCMotorLeftDir = DCMOTOR_FWD;
                G_DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '1': {
                G_DCMotorLeftDir = DCMOTOR_STOP;
                G_DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '2': {
                G_DCMotorLeftDir = DCMOTOR_FWD;
                G_DCMotorRightDir = DCMOTOR_STOP;
                break;
            }
            case '3': {
                G_DCMotorLeftDir = DCMOTOR_BWD;
                G_DCMotorRightDir = DCMOTOR_FWD;
                break;
            }
            case '4': {
                G_DCMotorLeftDir = DCMOTOR_FWD;
                G_DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '5': {
                G_DCMotorLeftDir = DCMOTOR_BWD;
                G_DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '6': {
                G_DCMotorLeftDir = DCMOTOR_STOP;
                G_DCMotorRightDir = DCMOTOR_BWD;
                break;
            }
            case '7': {
                G_DCMotorLeftDir = DCMOTOR_BWD;
                G_DCMotorRightDir = DCMOTOR_STOP;
                break;
            }
            case 'A':{
                G_DCMotorLeftDir = DCMOTOR_STOP;
                G_DCMotorRightDir = DCMOTOR_STOP;
                G_leftEncoderSetpoint = DCMOTOR_SPEED_BASE;
                G_rightEncoderSetpoint = DCMOTOR_SPEED_BASE;
                break;
            }

            // Speed
            case '8': {
                G_leftEncoderSetpoint += DCMOTOR_SPEED_INC;
                G_rightEncoderSetpoint += DCMOTOR_SPEED_INC;

                if (G_leftEncoderSetpoint > DCMOTOR_SPEED_MAX) {
                    G_leftEncoderSetpoint = DCMOTOR_SPEED_MAX;
                }
                
                if (G_rightEncoderSetpoint > DCMOTOR_SPEED_MAX) {
                    G_rightEncoderSetpoint = DCMOTOR_SPEED_MAX;
                }

                break;
            }
            case '9': {
                G_leftEncoderSetpoint -= DCMOTOR_SPEED_DEC;
                G_rightEncoderSetpoint -= DCMOTOR_SPEED_DEC;
                
                if (G_leftEncoderSetpoint < DCMOTOR_SPEED_MIN) {
                    G_leftEncoderSetpoint = DCMOTOR_SPEED_MIN;
                }
                
                if (G_rightEncoderSetpoint < DCMOTOR_SPEED_MIN) {
                    G_rightEncoderSetpoint = DCMOTOR_SPEED_MIN;
                }

                break;
            }

            // Servo
            case 'B': {
                G_RCServoAngle = SERVO_HOME;
                Stepper_Range();
                break;
            }
            case 'C': {
                G_RCServoModifier = SERVO_INCREASE;
                break;
            }
            case 'D': {
                G_RCServoModifier = SERVO_DECREASE;
                break;
            }

            // Stepper
            case 'E': {
                if (LimitSwitch_PressCheck(RIGHT)) {
                    G_StepperStep = STEPPER_CW_FULL_STEP;
                }

                break;
            }
            case 'F': {
                if (LimitSwitch_PressCheck(LEFT)) {
                    G_StepperStep = STEPPER_CCW_FULL_STEP;
                }

                break;
            }
            case 'G': {
                G_RCServoModifier = SERVO_STOP;
                break;
            }
            case 'H': {
                G_StepperStep = STEPPER_STOP;
                break;
            }


            // Ultrasonic
            case 'I': {
                USART3_printf("\nUltrasonic: %dcm", Ultra_ReadSensor());
                break;
            }
    
            // Invalid command
            default: {
                break;
            }
        }

        G_RCServoAngle += G_RCServoModifier;
        if (G_RCServoAngle < SERVO_MIN) {
            G_RCServoAngle = SERVO_MIN;
        }
        else if (G_RCServoAngle > SERVO_MAX) {
            G_RCServoAngle = SERVO_MAX;
        }

        DCMotor_SetDirs(G_DCMotorLeftDir, G_DCMotorRightDir);
        Stepper_Step(G_StepperStep);
        RCServo_SetAngle(G_RCServoAngle);
        Delay_ms(5);
    }
}

