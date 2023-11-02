/*******************************************************************************
* Name: Stepper.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 3, 2023
* Description: Stepper motor control.
*******************************************************************************/

#include "Stepper.h"

/*******************************************************************************
*					          GLOBAL VARIABLES                                 *
*******************************************************************************/
volatile uint8_t StepperStep = STEPPER_STOP;

/*******************************************************************************
*					    LOCAL CONSTANTS AND VARIABLES                          *
*******************************************************************************/
static uint8_t stepCounter = 0xFF;		// Stepper motor pattern counter (only care about the 3 LSBs)

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
		case STEPPER_STOP: {
			break;
		}
		case STEPPER_CW_FULL_STEP:{
            stepCounter += 2;
			break;
		}
		case STEPPER_CCW_FULL_STEP:{
            stepCounter -= 2;
			break;
		}
		case STEPPER_CW_HALF_STEP:{
            stepCounter++;
			break;
		}
		case STEPPER_CCW_HALF_STEP:{
            stepCounter--;
			break;
		}
		default:{
			return;
		}
	}

    Stepper_Ouput(stepPatterns[0x7 & stepCounter]);		// & with 0x7 because we just want the lower 3 bits	
}

uint8_t Stepper_Range(void) {
    uint8_t rangeCount = 0;
	
    StepperStep = STEPPER_CW_FULL_STEP;
    while(StepperStep != 0){
        Stepper_Step(STEPPER_CW_FULL_STEP);
		Delay_ms(5);
    }

	StepperStep = STEPPER_CCW_FULL_STEP;
    while(StepperStep != 0){
        rangeCount++;
        Stepper_Step(STEPPER_CCW_FULL_STEP);
		Delay_ms(5);
    }
    
	StepperStep = STEPPER_STOP;
    for(int i = 0; i < rangeCount/2; i++){
        Stepper_Step(STEPPER_CW_FULL_STEP);
		Delay_ms(5);
    }
    
    return rangeCount;
}
