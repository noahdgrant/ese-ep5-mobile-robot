/*******************************************************************************
* Name: Stepper.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 3, 2023
* Description: Stepper motor control.
*******************************************************************************/

#include "stm32f303xe.h"
#include "Stepper.h"
#include "Utility.h"
#include "UART.h"
#include "LimitSwitch.h"


/*******************************************************************************
*					    LOCAL CONSTANTS AND VARIABLES                          *
*******************************************************************************/

static uint8_t stepCounter = 0xFF;		// Stepper motor pattern counter (only care about the 3 LSBs)
static int8_t lastStep = 0;				// Last valid step incrament
static uint8_t lastStepType = 0;		// Last valid step type

/*******************************************************************************
*					        	PRIVATE FUNCTIONS							   *
*******************************************************************************/

/*******************************************************************************
* stepper_output() - Updates the output of GPIOC pins PC0-PC3.
* stepPattern		- Binary step pattern to ouput.
* No return value.
*******************************************************************************/
static void Stepper_Ouput(uint8_t stepPattern){
	// PC0 - A
	if (stepPattern & 0x8){
		GPIOC->ODR |= (1UL << (1*0));
	}
	else{
		GPIOC->ODR &= ~(1UL << (1*0));
	}
	// PC1 - A/
	if (stepPattern & 0x4){
		GPIOC->ODR |= (1UL << (1*1));
	}
	else{
		GPIOC->ODR &= ~(1UL << (1*1));
	}
	// PC2 - B
	if (stepPattern & 0x2){
		GPIOC->ODR |= (1UL << (1*2));
	}
	else{
		GPIOC->ODR &= ~(1UL << (1*2));
	}
	// PC3 - B/
	if (stepPattern & 0x1){
		GPIOC->ODR |= (1UL << (1*3));
	}
	else{
		GPIOC->ODR &= ~(1UL << (1*3));
	}
}


/*******************************************************************************
*						        PUBLIC FUNCTIONS							   *
*******************************************************************************/

/*******************************************************************************
* stepper_init() - Initialize GPIOC pins PC0-PC3.
* No inputs.
* No return value.
*******************************************************************************/
void Stepper_Init(void){
	uint8_t PCx;	// GPIOC pin number (PC0-PC3)
	
	// 1. Turn on AHB so GPIOs are ON
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	// PC0-PC3
	for(PCx = 0; PCx < 4; PCx++){
		// 2. GPIO Mode Register -> set to OUTPUT
		// Reset PC0-PC3
		GPIOC->MODER &= ~(3UL << (2*PCx));

		// Set PC0-PC3 to output (01)
		GPIOC->MODER |= (1UL << (2*PCx));
		
		// 3. Set GPIO output TYPE to push-pull (logic 0)
		GPIOC->OTYPER &= ~(1UL << (1*PCx));
		
		// 4. (optional) set GPIO pull-up/pull-down register to default (no pull)
		GPIOC->PUPDR &= ~(3UL << (2*PCx));
		
		// 5. Initialize to OFF (0)
		GPIOC->ODR &= ~(1UL << (1*PCx));
	}		
}

/*******************************************************************************
* stepper_step() - Updates stepCount based on stepType and passes binary output pattern to stepper_output().
* stepType		- The type of step (FS-CW, FS-CCW, HS-CW, HS-CCW) to take.
* No return value.
*******************************************************************************/
void Stepper_Step(uint8_t stepType){
	uint8_t stepPatterns[] = {0x8, 0xA, 0x2, 0x6, 0x4, 0x5, 0x1, 0x9};		// The different possible binary step patterns
	
	switch(stepType){
		// Turn motor OFF
		case 0: {
			Stepper_Ouput(stepPatterns[0x7 & stepCounter]);	
			lastStepType = 0;
			lastStep = 0;
			break;
		}
		// Full-step clockwise
		case 1:{
            stepCounter += 2;
    	    Stepper_Ouput(stepPatterns[0x7 & stepCounter]);		// & with 0x7 because we just want the lower 3 bits
            lastStepType = 1;
            lastStep = 2;
			break;
		}
		// Full-step counter-clockwise
		case 2:{
            stepCounter -= 2;
            Stepper_Ouput(stepPatterns[0x7 & stepCounter]);		// & with 0x7 because we just want the lower 3 bits
            lastStepType = 2;
            lastStep = -2;
			break;
		}
		// Half-step clockwise
		case 3:{
            stepCounter++;
            Stepper_Ouput(stepPatterns[0x7 & stepCounter]);		// & with 0x7 because we just want the lower 3 bits
            lastStepType = 3;
            lastStep = 1;
			break;
		}
		// Half-step counter-clockwise
		case 4:{
            stepCounter--;
            Stepper_Ouput(stepPatterns[0x7 & stepCounter]);		// & with 0x7 because we just want the lower 3 bits
            lastStepType = 4;
            lastStep = -1;
			break;
		}
		// Repeat last valid input if bad value is passed to the funciton
		default:{
			stepCounter += lastStep;
			Stepper_Ouput(stepPatterns[0x7 & stepCounter]);		// & with 0x7 because we just want the lower 3 bits
			break;
		}
	}
}
