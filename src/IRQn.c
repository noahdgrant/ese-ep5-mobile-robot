/*******************************************************************************
* Name: IRQn.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: October 4, 2023
* Description: Interrupt Handler Implementation.
*******************************************************************************/

#include "IRQn.h"
#include "Utility.h"

void EXTI9_5_IRQHandler(void) {
    extern volatile uint8_t StepperLastStep;	// The last step the servo took
    extern volatile uint32_t Global_UltraEcho;

    // Check whether a limit switch was triggered
    if ((EXTI->PR & EXTI_PR_PIF5) != 0) { // left limit
        StepperLastStep = 0;
        // Clear flag by writing 1
        SET_BITS(EXTI->PR, EXTI_PR_PIF5);
    }
    else if ((EXTI->PR & EXTI_PR_PIF6) != 0) { // right limit
        StepperLastStep = 0;
        // Clear flag by writing 1
        SET_BITS(EXTI->PR, EXTI_PR_PIF6);
    }
	
    // Check whether ultrasonic echo was recieved
	if(IS_BIT_SET(TIM3->SR, TIM_SR_CC1IF)){
		Global_UltraEcho = TIM3->CCR1;	// Record TIM3 CCR1 value in a global variable for further processing
	}
}

