/********************************************************************************
* Name: KeyPad.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: January 25, 2023
* Description: Keypad Pad functions to initialize, and check status.
********************************************************************************/

#ifndef __KeyPad_H
#define __KeyPad_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

void KeyPad_Init(void);
uint8_t KeyPad_MatrixScan(void);
uint8_t KeyPad_GetKey(void);

#endif
