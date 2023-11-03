/*******************************************************************************
* Name: RCservo.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 10, 2023
* Description: RC servo motor conrol for mobile robot.
*******************************************************************************/

#include "RCServo.h"

/*******************************************************************************
*                               GLOBAL VARIABLES                               *
*******************************************************************************/
int G_RCServoAngle = SERVO_HOME;
int G_RCServoModifier = SERVO_STOP;

/*******************************************************************************
*                       LOCAL CONSTANTS AND VARIABLES                          *
*******************************************************************************/
#define SERVO_CENTRE 1500       // Servo centre pulse width (us)
#define SERVO_NEG_LMT 1050      // Servo negative mechanical limit pulse width (us)
#define SERVO_POS_LMT 1950      // Servo positive mechanical limit pulse width (us)
#define US_PER_DEGREE 10        // Servo us/degree pulse width ratio

static int RCServoLastAngle = 0;

/*******************************************************************************
*                           PUBLIC FUNCTIONS                                   *
*******************************************************************************/
/*******************************************************************************
* RCServo_init() - Configure setting for servo motor.
* No inputs.
* No return value.
*******************************************************************************/
void RCServo_Init(void){
    // Set up PB15 to Output, AF1, PP, no Pull
    // 1. Enable clock to Port B
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // 2. Set the MODER to AF
    // 00: Input mode (reset state)
    // 01: General purpose output mode
    // 10: Alternate function mode
    // 11: Analog mode
    GPIO_MODER_SET(B, 15, 2UL);

    // 3. Set the AFR to AF1 for PB15
    // AFRL is 0-7 and AFRH is 8-15
    // Need AFRH which is AFR[1] since using pin 15 (PB15)
    SET_BITS(GPIOB->AFR[1], 1UL << (4*7));

    // 4. Set the OTYPER to Push-Pull
    // 0: Output push-pull (reset state)
    // 1: Output open-drain
    GPIO_OTYPER_SET(B, 15, 0UL);

    // 5. Set the PUPDR to no pull-up
    // 00: No pull-up, pull-down
    // 01: Pull-up
    // 10: Pull-down
    // 11: Reserved
    GPIO_PUPDR_SET(B, 15, 0UL);


    // Configure TIM15
    // 1. Turn on TIM15 by feeding in the APB clock.  (RCC_APB2ENR_TIM15EN)
    SET_BITS(RCC->APB2ENR, RCC_APB2ENR_TIM15EN);

    // 2. Program the prescaler (PSC) to ensure TIM15 counts at 1us
    // Timer Period = (Prescaler + 1) / SystemClockFreq
    // 1us = (Prescaler + 1) / 72MHz
    // (Prescaler + 1) = 72
    // Prescaler = 71
    SET_BITS(TIM15->PSC, 71UL);

    // 3. Set TIM15 to Upcounting
    // (no need to do it, because TIM15 only knows upcounting...)

    // 4. Set the ARR to 20000us period
    //    Repeating Counter Period = ARR + 1
    //    Then, what is ARR?
    FORCE_BITS(TIM15->ARR, 0xFFFFUL, 20001UL);

    // 5. Enable TIM15 ARR Preload (ARPE flag on CR1)
    SET_BITS(TIM15->CR1, TIM_CR1_ARPE);

    // 6. Enable TIM15 main output, so to make it avaiable to the PWM OC (MOE flag on BDTR)
    SET_BITS(TIM15->BDTR, TIM_BDTR_MOE);


    // Configure CH2 of TIM15 for PWM OC mode
    // 1. Select PWM mode 1 (low-count mode) for TIM15 CH2 (OC2M flags - 2 bits - on CCMR1)
    //    ** PAY VERY CLOSE ATTENTION ON THIS ITEM **
    SET_BITS(TIM15->CCMR1, TIM_CCMR1_OC2M);

    // 2. Enable the Output Compare Preload on CH2 (OC2PE flag on CCMR1)
    SET_BITS(TIM15->CCMR1, TIM_CCMR1_OC2PE);

    // 3. Enable output channel 2 (CH2 with no negate) - (CC2E flag on CCER)
    SET_BITS(TIM15->CCER, TIM_CCER_CC2E);

    // 4. Make CH2 Active High (no negate) - (CC2P flag on CCER)
    SET_BITS(TIM15->CCER, TIM_CCER_CC2P);

    // 5. Set CH2 PWM initial on-time to 0, so PWM will not output anything before preload is done (writing 0 into CCR2)
    CLEAR_BITS(TIM15->CCR2, TIM_CCR2_CCR2);


    // Set TIM15 off!
    // 1. Force and Update Event to ensure all preload operations are done in sync! (UG flag on EGR)
    SET_BITS(TIM15->EGR, TIM_EGR_UG);

    // 2. Enable TIM15 Counting! (CEN flag on CR1)
    SET_BITS(TIM15->CR1, TIM_CR1_CEN);
}

/*******************************************************************************
* RCServo_setAngle() - Sets angle of the servo motor by updating the pulse width.
* angle     - Servo motor angle.
* No return value
*******************************************************************************/
void RCServo_SetAngle(int16_t angle){
    if (angle != RCServoLastAngle) {
        int16_t PW = 0; // Pulse width
        
        // Pulse width is position set point
        // Pulse train needs to repeat every 20us
        
        // 1. Convert the target angle to the corresponding target Pulse Width
            // According to angle vs PW graph from slides: 1 degree = 10us
                // m = (y2 - y1) / (x2 - x1)
                // m = (90 - 0) / (2400 - 1500)
                // m = 0.1 degree/us (10us/degree)
        PW = SERVO_CENTRE + angle * US_PER_DEGREE;
        
        // 2. Check whether the PW has exceeded the mechanical (+45 ~ -45 degrees) & motor limit (+/- 90 degrees) and cap the target PW at the limits!
            // 600us    -90 degrees     (motor limit)
            // 900us    -60 degrees
            // 1050us   -45 degrees     (mechanical limit)
            // 1500us   0 degrees       (centre)
            // 1950us   +45 degrees     (mechanical limit)
            // 2100us   +60 degrees
            // 2400us   +90 degrees     (motor limit)
        if(PW > SERVO_POS_LMT){
            PW = SERVO_POS_LMT;
        }
        else if(PW < SERVO_NEG_LMT){
            PW = SERVO_NEG_LMT;
        }
        
        // 3. Write the new target PW into TIM15 CR2 
        FORCE_BITS(TIM15->CCR2, 0xFFFFUL, (uint16_t)PW);

        RCServoLastAngle = angle;
    }
}

