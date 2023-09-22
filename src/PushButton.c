/********************************************************************************
* Name: PushButton.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 17, 2023
* Description: Push button control for mobile robot.
********************************************************************************/

#include "PushButton.h"
#include "Utility.h"
#include "stm32f303xe.h"
#include "LED.h"

/******************************************************************
*						PUBLIC FUNCTIONS													*
******************************************************************/

/********************************************************
* PushButton_Init() - Initialize limit switch settings.
* No inputs.
* No return value.
********************************************************/
void PushButton_Init(void){
	// Enable GPIO Port C
	ENABLE_GPIO_CLOCK(C);
	
	//Set PC13 to INPUT mode (00)
	GPIO_MODER_SET(C, 13, GPIO_MODE_IN);
	
	// Set PUPD to no-pull (00 for pin 13)
	GPIO_PUPDR_SET(C, 13, GPIO_PUPD_NO);


    EXTI->IMR |= EXTI_IMR_IM13; // Enable interrupt

    // Connect External Line to the GPI
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // Falling Edge trigger selection
    EXTI->FTSR |= EXTI_RTSR1_FT13;

    // Configure NVIC for EXTI events on pin 10-15
    // Set its priority to 0 (next highest to NMIs)
    NVIC_EnableIRQ( EXTI15_10_IRQn );
    NVIC_SetPriority( EXTI15_10_IRQn, 0 );
}

/**********************************************************************
* PushButton_PressCheck() - Check if the push button has been pressed.
* No inputs.
* Returns result depending on button status.
**********************************************************************/
uint8_t PushButton_PressCheck(void){
	// Check if ODR of PC13 is set
	if(IS_BIT_SET(GPIOC->IDR, GPIO_IDR_13)){
		// If set, button is not pressed because of ACTIVE-LOW.
		return(0);
	}
	else{
		// If cleared, button is pressed.
		return(1);
	}
}


void EXTI15_10_IRQHandler(void) {
    if ((EXTI->PR & EXTI_PR_PIF13) != 0) {
        // Toggle LED
        LED_Toggle();
        // Cleared flag by writing 1
        EXTI->PR |= EXTI_PR_PIF13;
    }
}