/*******************************************************************************
* Name: DCMotor.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: March 24, 2023
* Description: DC motor functions.
*******************************************************************************/

#include "DCMotor.h"
#include "Utility.h"
#include "stm32f303xe.h"

// Drive Motor Configuration Parameters
// - Motor Speed Control Pins:
//    Left Motor	PC10
//    Right Motor   PC11
// - Motor Direction Control Pins:
//    Left Motor Forward (A)	PC12
//    Left Motor Reverse (B)    PC13
//    Right Motor Forward (A)   PC8
//    Right Motor Reverse (B)   PC9
//
// - Direction Control Truth Table
//     STOP   FWD	RWD   UNDEFINED
// (A)  0      1     0      1     
// (B)  0      0     1      1

/*******************************************************************************
* DCMotor_Init() - Initiate and configure DC motors.
* No inputs.
* No return value.
*******************************************************************************/
void DCMotor_Init(void){
	ENABLE_GPIO_CLOCK(C);		// Enable GPIO Clock
	
	// Configure Pins 
	// Mode = Output
	GPIO_MODER_SET(C, 8, GPIO_MODE_OUT);
	GPIO_MODER_SET(C, 9, GPIO_MODE_OUT);
	GPIO_MODER_SET(C, 12, GPIO_MODE_OUT);
	GPIO_MODER_SET(C, 13, GPIO_MODE_OUT);
	
	// Output Type = Push Pull
	GPIO_OTYPER_SET(C, 8, GPIO_OTYPE_PP);
	GPIO_OTYPER_SET(C, 9, GPIO_OTYPE_PP);
	GPIO_OTYPER_SET(C, 12, GPIO_OTYPE_PP);
	GPIO_OTYPER_SET(C, 13, GPIO_OTYPE_PP);
	
	// Pull-up / Pull-down = No Pull
	GPIO_PUPDR_SET(C, 8, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(C, 9, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(C, 12, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(C, 13, GPIO_PUPD_NO);
	
	// Initial Output Value should be set to 0 (STOP by default)
	CLEAR_BITS(GPIOC->ODR, GPIO_ODR_8);
	CLEAR_BITS(GPIOC->ODR, GPIO_ODR_9);
	CLEAR_BITS(GPIOC->ODR, GPIO_ODR_12);
	CLEAR_BITS(GPIOC->ODR, GPIO_ODR_13);
	
	
	// Speed Control
	// Mode = Alternative Function 4
	GPIO_MODER_SET(C, 10, GPIO_MODE_AF);
	GPIO_MODER_SET(C, 11, GPIO_MODE_AF);
	
	GPIO_AFR_SET(C, 10, 4);
	GPIO_AFR_SET(C, 11, 4);
	
	// Output Type = Push Pull
	GPIO_OTYPER_SET(C, 10, GPIO_OTYPE_PP);
	GPIO_OTYPER_SET(C, 11, GPIO_OTYPE_PP);
	
	// Pull-up / Pull-down = No Pull
	GPIO_PUPDR_SET(C, 10, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(C, 11, GPIO_PUPD_NO);
	
	
	// Configure TIM8 for CH1N and CH2N
	SET_BITS(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);		// Turn on Timer 8
	SET_BITS(TIM8->PSC, 71UL);						// Set PSC so it counts in 1us
		// Timer Period = (Prescaler + 1) / SystemClockFreq
		// 1us = (Prescaler + 1) / 72MHz
		// (Prescaler + 1) = 72
		// Prescaler = 71
	CLEAR_BITS(TIM8->CR1, TIM_CR1_DIR);				// Set TIM8 counting direction to upcounting
	FORCE_BITS(TIM8->ARR, 0xFFFFUL, 999UL);			// Set ARR to 999us
		// ARR = Repeating Counter Period - 1
		// ARR = 1000us - 1
		// ARR = 999us
	SET_BITS(TIM8->CR1, TIM_CR1_ARPE);				// Enable ARR preload (ARPE) in CR1
	SET_BITS(TIM8->BDTR, TIM_BDTR_MOE);				// Set main output enabled (MOE) in BDTR
	
	
	// Configure CH1N of TIM8 for Left Wheel PWM output compare mode
	FORCE_BITS(TIM8->CCMR1, TIM_CCMR1_OC1M_Msk, 0x6UL << TIM_CCMR1_OC1M_Pos);				// Set TIM8 to PWM mode
	SET_BITS(TIM8->CCMR1, TIM_CCMR1_OC1PE); 																				// Enable output compare preload on channel 1
	SET_BITS(TIM8->CCER, TIM_CCER_CC1NE);																						// Enable the COMPLEMENTARY output channel (CH1N)
	CLEAR_BITS(TIM8->CCER, TIM_CCER_CC1NP);																					// Make CH1N active HI
	CLEAR_BITS(TIM8->CCR1, TIM_CCR1_CCR1);																					// Set the CH1N initial PWM ON-time to 0us
	
	
	// Configure CH2N of TIM8 for Right Wheel PWM output compare mode
	FORCE_BITS(TIM8->CCMR1, TIM_CCMR1_OC2M_Msk, 0x6UL << TIM_CCMR1_OC2M_Pos);				// Set TIM8 to PWM mode
	SET_BITS(TIM8->CCMR1, TIM_CCMR1_OC2PE); 																				// Enable output compare preload on channel 2
	SET_BITS(TIM8->CCER, TIM_CCER_CC2NE);																						// Enable the COMPLEMENTARY output channel (CH2N)
	CLEAR_BITS(TIM8->CCER, TIM_CCER_CC2NP);																					// Make CH2N active HI
	CLEAR_BITS(TIM8->CCR1, TIM_CCR1_CCR1);																					// Set the CH2N initial PWM ON-time to 0us
	
	
	// Start TIM8 CH1N and CH2N Outputs
	SET_BITS(TIM8->EGR, TIM_EGR_UG);				// Force an update event to preload all the registers
	SET_BITS(TIM8->CR1, TIM_CR1_CEN);				// Enable TIM8 to start counting
}

/*******************************************************************************
* DCMotor_SetSpeed() - Sets the speed for a DC motor.
* motor				- The motor to set the speed for.
* dutyCycle		- The desired % of duty cycle for ON-time.
* No return value.
*******************************************************************************/
void DCMotor_SetSpeed(uint8_t motor, uint16_t dutyCycle){	
	// Cap duty cycle %
	if(dutyCycle > 100){
		dutyCycle = 100;
	}
	else if(dutyCycle < 50){
		dutyCycle = 50;     // The DC motors do not start spinning if the duty cycle is < 50%
	}
	
	// Convert to ms ON-time
	dutyCycle *= 10;        // dutyCycle = (dutyCycle * 1000) / 100
	
	// Output PW duty cycle
	if(motor == DCMOTOR_LEFT){
		FORCE_BITS(TIM8->CCR1, 0xFFFFUL, dutyCycle);
	}
	else if(motor == DCMOTOR_RIGHT){
		FORCE_BITS(TIM8->CCR2, 0xFFFFUL, dutyCycle);
	}
}	

/*******************************************************************************
* DCMotor_SetDir() - Sets the direction of a DC motor.
* motor		- The motor to set the direction of.
* dir			- The direction the DC motor should spin.
* No return value.
*******************************************************************************/
void DCMotor_SetDir(uint8_t motor, uint8_t dir){
	// - Motor Direction Control Pins:
	//	Left Motor Forward (A)	PC12
	//	Left Motor Reverse (B)	PC13
	//	Right Motor Forward (A)	PC8
	//	Right Motor Reverse (B)	PC9
	// - Direction Control Truth Table
	//     STOP   FWD	RWD   UNDEFINED
	// (A)  0      1     0      1     
	// (B)  0      0     1      1

	// POSSIBLE INPUTS
	// motor:		0 - left
	//					1 - right
	//
	// dir:			0 - stop
	//					1 - forward
	//					2 - backwards

	// Left motor
	if(motor == DCMOTOR_LEFT){
		// Left motor stop
		CLEAR_BITS(GPIOC->ODR, GPIO_ODR_12);
		CLEAR_BITS(GPIOC->ODR, GPIO_ODR_13);
		Delay_ms(5);

		// Left motor fwd
		if(dir == DCMOTOR_FWD){
			SET_BITS(GPIOC->ODR, GPIO_ODR_12);
			CLEAR_BITS(GPIOC->ODR, GPIO_ODR_13);
		}
		// Left motor bwd
		else if(dir == DCMOTOR_BWD){
			CLEAR_BITS(GPIOC->ODR, GPIO_ODR_12);
			SET_BITS(GPIOC->ODR, GPIO_ODR_13);
		}
	}
	// Right motor
	else if (motor == DCMOTOR_RIGHT){
		// Right motor stop
		CLEAR_BITS(GPIOC->ODR, GPIO_ODR_8);
		CLEAR_BITS(GPIOC->ODR, GPIO_ODR_9);
		Delay_ms(5);
		
		// Right motor fwd
		if(dir == DCMOTOR_FWD){
			SET_BITS(GPIOC->ODR, GPIO_ODR_8);
			CLEAR_BITS(GPIOC->ODR, GPIO_ODR_9);
		}
		// Right motor bwd
		else if(dir == DCMOTOR_BWD){
			CLEAR_BITS(GPIOC->ODR, GPIO_ODR_8);
			SET_BITS(GPIOC->ODR, GPIO_ODR_9);
		}
	}
}

/*******************************************************************************
* DCMotor_SetMotor() - Set the speed and direction of one motor.
* dir						- motor direction.
* dutyCycle			- motor duty cycle.
* No return value.
*******************************************************************************/
void DCMotor_SetMotor(uint8_t motor, uint8_t dir, uint16_t dutyCycle){
	DCMotor_SetDir(motor, dir);
	DCMotor_SetSpeed(motor, dutyCycle);
}

/*******************************************************************************
* DCMotor_SetMotors() - Sets the speed and direction of both motors.
* leftDir						- Left motor direction.
* leftDutyCycle			- Left motor duty cycle.
* rightDir					- Right motor direction.
* rightDutyCycle		- Right motor duty cycle.
* No return value.
*******************************************************************************/
void DCMotor_SetMotors(uint8_t leftDir, uint16_t leftDutyCycle, uint8_t rightDir, uint16_t rightDutyCycle){
	DCMotor_SetMotor(DCMOTOR_LEFT, leftDir, leftDutyCycle);
	DCMotor_SetMotor(DCMOTOR_RIGHT, rightDir, rightDutyCycle);
}

/*******************************************************************************
* DCMotor_Stop() - Stops both motors.
* No inputs.
* No return value.
*******************************************************************************/
void DCMotor_Stop(void){
	DCMotor_SetMotors(DCMOTOR_STOP, 0, DCMOTOR_STOP, 0);
}

/*******************************************************************************
* DCMotor_Forward() - Both motors spin forwards.
* dutyCycle		- The desired % of duty cycle for ON-time.
* No return value.
*******************************************************************************/
void DCMotor_Forward(uint16_t dutyCycle){
	DCMotor_SetMotors(DCMOTOR_FWD, dutyCycle, DCMOTOR_FWD, dutyCycle);
}

/*******************************************************************************
* DCMotor_Backward() - Both motors spin backwards.
* dutyCycle		- The desired % of duty cycle for ON-time.
* No return value.
*******************************************************************************/
void DCMotor_Backward(uint16_t dutyCycle){
	DCMotor_SetMotors(DCMOTOR_BWD, dutyCycle, DCMOTOR_BWD, dutyCycle);
}
