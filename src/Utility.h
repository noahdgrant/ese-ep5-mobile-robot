/******************************************************************************
* Name: Utility.h (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 10, 2023
* Description: Utility functions for mobile robot.
******************************************************************************/

#ifndef __Utility_H
#define __Utility_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

/******************************************************************
*														MACROS																*
******************************************************************/
#define SET_BITS(port, bits) ((port) |= (bits))
#define CLEAR_BITS(port, bits) ((port) &= (~(bits)))
#define FLIP_BITS(port, bits) ((port) ^= (bits))
#define FORCE_BITS(port, mask, value) ((port) = ((port) & (~(mask))) | ((value) & (mask)))

// Logic Bit Tests
#define IS_BIT_SET(port, bits) ((port) & (bits))
#define IS_BIT_CLEAR(port, bits) ((port) & (~(bits)))

// Nibble Selection
#define LO_NYBBLE(value)	((value) & 0x0FUL)
#define HI_NYBBLE(value)	((value >> 4UL) & 0x0FUL)

// Port Name Macro
#define GPIO(port) GPIOx(port)
#define GPIOx(port) GPIO ## port

// GPIO Register Setting Macros
#define GPIO_MODER_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->MODER), (3UL << ((pin) * 2)), ((mode) << ((pin) * 2)))
#define GPIO_MODE_IN 0UL		// 00: Input mode (reset state)
#define GPIO_MODE_OUT 1UL		// 01: General purpose output mode
#define GPIO_MODE_AF 2UL		// 10: Alternate Function mode
#define GPIO_MODE_ANLG 3UL	// 11: Analog mode

#define GPIO_OTYPER_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->OTYPER), (1UL << ((pin) * 1)), ((mode) << ((pin) * 1)))
#define GPIO_OTYPE_PP 0UL		// 0: Output push-pull (reset state)
#define GPIO_OTYPE_OD 1UL		// 1: Output open-drain

#define GPIO_PUPDR_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->PUPDR), (3UL << ((pin) * 2)), ((mode) << ((pin) * 2)))
#define GPIO_PUPD_NO 0UL		// 00: No pull-up/pull-down
#define GPIO_PUPD_PU 1UL		// 01: Pull-up
#define GPIO_PUPD_PD 2UL		// 10: Pull-down

#define GPIO_AFR_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->AFR[pin >> 3UL]), (15UL << ((pin & 7UL) * 4)), ((mode) << ((pin & 7UL) * 4)))

#define GPIO_OSPEEDR_SET(port, pin, mode) FORCE_BITS ((GPIOx( port ) -> OSPEEDR), (3UL<<((pin)*2)), ((mode)<<((pin)*2)))
#define GPIO_OSPEED_LOW 0UL		// x0: Low speed
#define GPIO_OSPEED_MED 1UL		// 01: Medium speed
#define GPIO_OSPEED_HIGH 3UL	// 11: High speed

#define GPIO_ODR_SET(port, pin, state) FORCE_BITS(GPIO(port) -> ODR, (1UL << ((pin) * 1)), ( (state) << ((pin) * 1)))
#define GPIO_ODR_BIT_CLEAR	0UL
#define GPIO_ODR_BIT_SET		1UL

#define ENABLE_GPIO_CLOCK(port)	ENABLE_GPIO_CLOCKx(port)
#define ENABLE_GPIO_CLOCKx(port) RCC -> AHBENR |= RCC_AHBENR_GPIO ## port ## EN


/******************************************************************
*												PUBLIC FUNCTIONS													*
******************************************************************/

void Delay_ms(uint32_t msec);

#endif
