/*******************************************************************************
* Name: Ultrasonic.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: October 4, 2023
* Description: Ultrasonic sensor functions. Ultrasonic sensor is in polling mode.
*******************************************************************************/

#include "Ultrasonic.h"
#include "Utility.h"
	
/*******************************************************************************
*								STATIC VARIABLES				  			   *
*******************************************************************************/
	
uint32_t Global_UltraEcho;


/*******************************************************************************
*								PRIVATE FUNCTIONS							   *
*******************************************************************************/

/*******************************************************************************
* Ultra_InitTrigger() - Initialize ultrasonic trigger timer.
* No inputs.
* No return value.
*******************************************************************************/
static void Ultra_InitTrigger(void){
	// Configure GPIO Pin
	ENABLE_GPIO_CLOCK(A);						    // Enable GPIO Port A
	GPIO_MODER_SET(A, 12, GPIO_MODE_AF);		    // Set MODE to AF
	GPIO_AFR_SET(A, 12, 1);						    // TIM16 CH1
	GPIO_OTYPER_SET(A, 12, GPIO_OTYPE_PP);	        // Set OTYPE to Push-pull
	GPIO_PUPDR_SET(A, 12, GPIO_PUPD_NO);		    // Set PUPD to No pull-up/pull-down
	
	// Configure TIM16 CH1
	SET_BITS(RCC->APB2ENR, RCC_APB2ENR_TIM16EN);	// Turn on TIM16
	SET_BITS(TIM16->PSC, 71UL);					    // Set PSC so it counts in 1us (1-10us)
		// Timer Period = (Prescaler + 1) / SystemClockFreq
		// 1us = (Prescaler + 1) / 72MHz
		// (Prescaler + 1) = 72
		// Prescaler = 71
	FORCE_BITS(TIM16->ARR, 0xFFFFUL, 99999UL);		// Set ARR to 100ms (100-500ms)
		// ARR = Repeating Counter Period - 1
	SET_BITS(TIM16->CR1, TIM_CR1_ARPE);				// Enable ARR preload (ARPE) in CR1
	SET_BITS(TIM16->BDTR, TIM_BDTR_MOE);			// Set main output enabled (MOE) in BDTR
	
	// Configure TIM16 CH1 for OCM with PWM
	SET_BITS(TIM16->CCMR1, TIM_CCMR1_OC1M);			// Set TIM16 to PWM mode
	SET_BITS(TIM16->CCMR1, TIM_CCMR1_OC1PE);		// Enable Output Compare Preload (OC1PE)
	SET_BITS(TIM16->CCER, TIM_CCER_CC1E);			// Enable Regular Output Channel for CH1
	CLEAR_BITS(TIM16->CCER, TIM_CCER_CC1P);			// Make CH1 active HI
	FORCE_BITS(TIM16->CCR1, 0xFFFFUL, 10UL);		// Set CH1 CCR1 initial output waveform on-time to 10us PWM pulse width
	
	// Configure TIM16 CH1 for PWM (repeating mode)
	SET_BITS(TIM16->CR1, TIM_CR1_OPM);				// Set TIM16 to one-shot mode
	SET_BITS(TIM16->EGR, TIM_EGR_UG);				// Force an update event to prelaod all the registers
}

/*******************************************************************************
* Ultra_InitEcho() - Initialize ultrasonic echo timer.
* No inputs.
* No return value.
*******************************************************************************/
static void Ultra_InitEcho(void){
	// Configure GPIO Pin
	ENABLE_GPIO_CLOCK(C);							// Enable GPIO Port C
	GPIO_MODER_SET(C, 7, GPIO_MODE_AF);			    // Set MODE to AF
	GPIO_AFR_SET(C, 7, 2);							// TIM3 CH2
	GPIO_PUPDR_SET(C, 7, GPIO_PUPD_NO);			    // Set PUPD to No pull-up/pull-down
	
	// Configure TIM3 CH2 (TI2)
	SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);		// Turn on clock for TIM3																			
	SET_BITS(TIM3->PSC, 71UL);						// Set PSC so it counts in 1us
		// 1us = (Prescaler + 1) / 72MHz
		// (Prescaler + 1) = 72
		// Prescaler = 71
	CLEAR_BITS(TIM3->CR1, TIM_CR1_DIR);				// Set TIM3 counting direction to upcounting
	FORCE_BITS(TIM3->ARR, 0xFFFFUL, 0xFFFFUL);		// Set ARR to max value
	CLEAR_BITS(TIM3->CCMR1, TIM_CCMR1_IC2F);		// Set TIM3 TI2 to No Input Filtering
	CLEAR_BITS(TIM3->CCMR1, TIM_CCMR1_IC2PSC);		// Set TI2 Prescaler to zero to capture every valid transition event		

	// Configure TIM3 CCR1 for PW measurement on TI2
	SET_BITS(TIM3->CCMR1, TIM_CCMR1_CC1S_1);		// Internally connect TIM3 TI2 to CCR1
	SET_BITS(TIM3->CCER, TIM_CCER_CC1P);			// Set TIM3 CCR1 to capture TI2 falling edge
	CLEAR_BITS(TIM3->CCER, TIM_CCER_CC1NP);			// Set TIM3 CCR1 to capture TI2 falling edge

	// Configure TIM3 to Slave Reset on TI2 Rising Edge
	SET_BITS(TIM3->SMCR, 6UL << TIM_SMCR_TS_Pos);	// Select Filtered Input Timer 2 (TI2) as the trigger source of the Timer Reset
	SET_BITS(TIM3->SMCR, 4UL << TIM_SMCR_SMS_Pos);	// Select RESET slave mode on TI2 Rising Edge
	
	// Enable Counter Capture
	SET_BITS(TIM3->CCER, TIM_CCER_CC1E);
	SET_BITS(TIM3->CR1, TIM_CR1_CEN);				// Enable TIM3 main counter

    // Configure interrupts for Echo pin
    NVIC_SetPriority( EXTI9_5_IRQn, 0 );
    NVIC_EnableIRQ( EXTI9_5_IRQn );
}


/*******************************************************************************
*								PUBLIC FUNCTIONS						       *
*******************************************************************************/

/*******************************************************************************
* Ultra_Init() - Call the ultrasonic trigger and echo initialization functions.
* No inputs.
* No return value.
*******************************************************************************/	
void Ultra_Init(void){
	Ultra_InitTrigger();
	Ultra_InitEcho();
}

/*******************************************************************************
* Ultra_StartTrigger() - Starts the trigger timer.
* No inputs.
* No return value.
*******************************************************************************/
void Ultra_StartTrigger(void){
	SET_BITS(TIM16->CR1, TIM_CR1_CEN);	// Enable TIM16
}

/*******************************************************************************
* Ultra_ReadSensor() - Calculates the distance to the object infront of the sensor.
* No inputs.
* Returns the distance in cm to the object infront of the sensor.
*******************************************************************************/
uint32_t Ultra_ReadSensor(void){
	return(Global_UltraEcho / 59);		// Equation from ESS W7 slides (#6)
}

