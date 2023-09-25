/********************************************************************************
* Name: LED.h (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: January 13, 2023
* Description: Nucleo board LED functions.
********************************************************************************/

#include "stm32f303xe.h"
#include "LED.h"
#include "Utility.h"


/******************************************************************
*						PUBLIC FUNCTIONS													*
******************************************************************/

/******************************************
* LED_Init() - Initialize LED setting.
* No inputs.
* No return value.
******************************************/
void LED_Init(void){
	// 1. Turn on AHB so GPIOs are ON
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// 2. GPIO Mode Register -> set to OUTPUT
	GPIOA->MODER &= ~(3UL << (2*5)); 	// Reset two bits for PA5
																		//3UL is 11 in binary
																		// 2 bits per pin for MODER and need to move 5 pin down
	GPIOA->MODER |= (1UL << (2*5));		// set the two bits to OUTPUT(01) for PA5
																		// 1UL is 01 in binary
	
	// 3. Set GPIO output TYPE to push-pull (logic 0)
	GPIOA->OTYPER &= ~(1UL << (1*5));		// OTYPER only have 1 bit per pin
																			// Only need to shift 5 bits
	
	// 4. (optional) set GPIO pull-up/pull-down register to default (no pull)
	GPIOA->PUPDR &= ~(3UL << (2*5));	// PUPDR has 2 bits per pin
	
	// 5. Write logic 1 to GPIOA ODR bit 5 (PA5 to controlling LED)
	// Initialize LED ON
	GPIOA->ODR |= (1UL << (1*5));		// ODR is 1 bit per pin	
}

/******************************************
* LED_Toggle() - Toggle LED ON and OFF.
* No inputs.
* No return value.
******************************************/
void LED_Toggle(void){
	GPIOA->ODR ^= (1UL << (1*5));	// XOR to flip bit
}

/******************************************
* LED_Flash - Turn LED on for a given number of seconds.
* number_of_seconds		- The number of seconds to turn the LED ON.
* No return value.
******************************************/
void LED_Flash(uint32_t number_of_seconds){
		Delay_ms(number_of_seconds * 1000);
		LED_Toggle();
}
