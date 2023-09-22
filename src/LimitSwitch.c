/********************************************************************************
* Name: LimitSwitch.h (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: September 15, 2023
* Description: Camera Module Limit Switch Implementation.
********************************************************************************/

#include "LimitSwitch.h"
#include "Utility.h"
#include "stm32f303xe.h"

/******************************************************************
*						PUBLIC FUNCTIONS						    *
******************************************************************/

/********************************************************
* LimitSwitch_Init() - Initialize push button setting.
* No inputs.
* No return value.
********************************************************/
void LimitSwitch_Init(void){
    // Enable GPIO Port C
    ENABLE_GPIO_CLOCK(C);
    // left  -> PC5
    // right -> pc6

    //Set PC5 to INPUT mode (00)
    GPIO_MODER_SET(C, 5, GPIO_MODE_IN);

    // Set PUPD to no-pull (00 for pin 5)
    GPIO_PUPDR_SET(C, 5, GPIO_PUPD_NO);

    //Set PC6 to INPUT mode (00)
    GPIO_MODER_SET(C, 6, GPIO_MODE_IN);

    // Set PUPD to no-pull (00 for pin 6)
    GPIO_PUPDR_SET(C, 6, GPIO_PUPD_NO);

}

/**********************************************************************
* LimitSwitch_PressCheck() - Check if the push button has been pressed.
* No inputs.
* Returns result depending on button status.
**********************************************************************/
uint8_t LimitSwitch_PressCheck(uint8_t direction){
    if(direction) { // Clockwise, Right
        // Check if ODR of PC6 is set
        if(IS_BIT_SET(GPIOC->IDR, GPIO_IDR_6)){
            // If set, button is not pressed because of ACTIVE-LOW.
            return(0);
        }
        else{
            // If cleared, button is pressed.
            return(1);
        }
    }
    else { // Counter-Clockwise, Left
        // Check if ODR of PC5 is set
        if(IS_BIT_SET(GPIOC->IDR, GPIO_IDR_5)){
            // If set, button is not pressed because of ACTIVE-LOW.
            return(0);
        }
        else{
            // If cleared, button is pressed.
            return(1);
        }
    }
}
