/*******************************************************************************
* Name: Utility.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 10, 2023
* Description: Utility functions for mobile robot.
*******************************************************************************/

#include "Utility.h"

/*******************************************************************************
*                               PUBLIC FUNCTIONS                                *
*******************************************************************************/

/*******************************************************************************
* UART2_init() - Initialize UART2 setting.
* No inputs.
* No return value.
*******************************************************************************/
void Delay_ms(uint32_t msec){
    // Stop SysTick
    SysTick->CTRL = 0;

    // Write appropriate RELOAD value to SysTick LOAD
    SysTick->LOAD = ((SystemCoreClock / 8) / 1000UL) * msec;

    // !! Check if loaded value exceeds max allowable value

    // Reset SysTick and prepare for reload
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

    SysTick->CTRL = 0;
}
