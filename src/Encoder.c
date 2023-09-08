/********************************************************************************
* Name: Encoder.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: April 14, 2023
* Description: Encoder functions for mobile robot.
********************************************************************************/

#include "Encoder.h"


/******************************************************************
*												GLOBAL VARIABLES									  			*
******************************************************************/	

uint32_t Global_LeftEncoderPeriod = 0;
uint32_t Global_RightEncoderPeriod = 0;


/******************************************************************
*												STATIC VARIABLES									  			*
******************************************************************/	

static uint32_t leftEncoder[2] = {0, 0};		// [0] = current, [1] = previous
static uint32_t rightEncoder[2] = {0, 0};		// [0] = current, [1] = previous


/******************************************************************
*												PUBLIC FUNCTIONS													*
******************************************************************/
/****************************************************************************
* Encoder_Init() - Initialize GPIOA PA0 (left) and PA1 (right) for encoders.
* No inputs.
* No return value.
****************************************************************************/
void Encoder_Init(void){
	// PA0 for Input Capture on Left Wheel
	// PA1 for Input Capture on Right Wheel

	
	// Configure GPIOA P0 and P1
	ENABLE_GPIO_CLOCK(A);									// Enable GPIO Port A	
	GPIO_MODER_SET(A, 0, GPIO_MODE_AF);
	GPIO_MODER_SET(A, 1, GPIO_MODE_AF);
	GPIO_PUPDR_SET(A, 0, GPIO_PUPD_NO);		
	GPIO_PUPDR_SET(A, 1, GPIO_PUPD_NO);
	GPIO_AFR_SET(A, 0, 1);								// TIM2 CH1
	GPIO_AFR_SET(A, 1, 1);								// TIM2 CH2
	
	
	// Configure TIM2 for Both CH1 and CH2 inputs
	SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);		// Enable TIM2 on APB1
	SET_BITS(TIM2->PSC, 71UL);										// Set prescaler to count in 1us
		// Timer Period = (Prescaler + 1) / SystemClockFreq
		// 1us = (Prescaler + 1) / 72MHz
		// (Prescaler + 1) = 72
		// Prescaler = 71
	CLEAR_BITS(TIM2->CR1, TIM_CR1_DIR);						// Set counting direction to upcounting
	
	
	// Configure TIM2 CH1 for input capture on Left Encoder
	SET_BITS(TIM2->CCMR1, TIM_CCMR1_CC1S_0);										// Input capture mode for CH1 (normal mode  0%01)
	SET_BITS(TIM2->CCER, TIM_CCER_CC1E);												// ENABLE_GPIO_CLOCK input capture for CH1
	CLEAR_BITS(TIM2->CCER, TIM_CCER_CC1P & TIM_CCER_CC1NP);			// Detect rising edges (by clearing both input capture mode bits)
	CLEAR_BITS(TIM2->CCR1, TIM_CCR1_CCR1);											// Clear garbage values from CCR1
	
	
	// Configure TIM2 CH2 for input capture on Right Encoder
	SET_BITS(TIM2->CCMR1, TIM_CCMR1_CC2S_0);										// Input capture mode for CH2 (normal mode  0%01)
	SET_BITS(TIM2->CCER, TIM_CCER_CC2E);												// ENABLE_GPIO_CLOCK input capture for CH2
	CLEAR_BITS(TIM2->CCER, TIM_CCER_CC2P & TIM_CCER_CC2NP);			// Detect rising edges (by clearing both input capture mode bits)
	CLEAR_BITS(TIM2->CCR2, TIM_CCR2_CCR2);											// Clear garbage values from CCR2

	
	// Configure TIM2 to generate interrupts and configure NVIC to respond
	SET_BITS(TIM2->DIER, TIM_DIER_CC1IE);						// Enable encoder CH1 to trigger IRQ
	SET_BITS(TIM2->DIER, TIM_DIER_CC2IE);						// Enable encoder CH2 to trigger IRQ
	NVIC_EnableIRQ(TIM2_IRQn);											// Enable TIM2 IRQ (TIM2_IRQn) in NVIC
	NVIC_SetPriority(TIM2_IRQn, ENCODER_PRIORITY);	// Set NVIC priority
	 
	// Start TIM2 CH1 and CH2 Input Captures
	SET_BITS(TIM2->EGR, TIM_EGR_UG);						// Force an update event to preload all the registers
	SET_BITS(TIM2->CR1, TIM_CR1_CEN);						// Enable TIM2 to start counting
}

/*********************************************************
* Encoder_IRQHandler() - Interrupt handler for encoders.
* No inputs.
* No return value.
*********************************************************/
void TIM2_IRQHandler(void){
	// Left wheel interrupt
	if(IS_BIT_SET(TIM2->SR, TIM_SR_CC1IF)){
		leftEncoder[1] = leftEncoder[0];
		leftEncoder[0] = TIM2->CCR1;
	}
	
	// Right wheel interrupt
	if(IS_BIT_SET(TIM2->SR, TIM_SR_CC2IF)){
		rightEncoder[1] = rightEncoder[0];
		rightEncoder[0] = TIM2->CCR2;
	}	
}

/****************************************************************************
* Encoder_CalculateSpeed() - Calculates the speed of each encoder in us/vane
* No inputs.
* No return value.
****************************************************************************/
void Encoder_CalculateSpeed(void){
	Global_LeftEncoderPeriod = leftEncoder[0] - leftEncoder[1];				// Calculate encoder period (current - previous)
	leftEncoder[1] = leftEncoder[0] = 0;															// Update previous measurement to current measurement
	
	Global_RightEncoderPeriod = rightEncoder[0] - rightEncoder[1];		// Calculate encoder period
	rightEncoder[1] = rightEncoder[0] = 0;														// Update previous measurement to current measurement

}
