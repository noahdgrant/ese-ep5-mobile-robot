/********************************************************************************
* Name: PushButton.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 17, 2023
* Description: Push button control for mobile robot.
********************************************************************************/

#include "PushButton.h"
#include "Utility.h"
#include "stm32f303xe.h"


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
