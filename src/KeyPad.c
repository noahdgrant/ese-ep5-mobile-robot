/********************************************************************************
* Name: KeyPad.c (implementation)
* Author(s): Noah Grant, Wyatt Richard
* Date: February 17, 2023
* Description: Keypad Pad functions to initialize, and check status.
********************************************************************************/
/*
		_________________________
		|			|			|			|			|
R1	|	1		|	2		|	3		|	A		|
		|_____|_____|_____|_____|
		|			|			|			|			|
R2	|	4		|	5		|	6		|	B		|
		|_____|_____|_____|_____|
		|			|			|			|			|
R3	|	7		|	8		|	9		|	C		|
		|_____|_____|_____|_____|
		|			|			|			|			|
R4	|	*		|	0		|	#		|	D		|
		|_____|_____|_____|_____|
			C1		C2		C3		C4
*/

#include "KeyPad.h"
#include "Utility.h"

/******************************************************************
*												PUBLIC FUNCTIONS													*
******************************************************************/

/****************************************************
* KeyPad_Init() - Initialize key pad settings.
* No inputs.
* No return value.
****************************************************/
void KeyPad_Init(void){
	ENABLE_GPIO_CLOCK(B);
	
	//Input Pins
	GPIO_MODER_SET(B, 4, GPIO_MODE_IN);
	GPIO_MODER_SET(B, 5, GPIO_MODE_IN);
	GPIO_MODER_SET(B, 6, GPIO_MODE_IN);
	GPIO_MODER_SET(B, 7, GPIO_MODE_IN);

	GPIO_PUPDR_SET(B, 4, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(B, 5, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(B, 6, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(B, 7, GPIO_PUPD_NO);
	
	// Output pins
	GPIO_MODER_SET(B, 0, 1UL);
	GPIO_MODER_SET(B, 1, 1UL);
	GPIO_MODER_SET(B, 2, 1UL);
	GPIO_MODER_SET(B, 3, 1UL);
	
	GPIO_PUPDR_SET(B, 0, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(B, 1, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(B, 2, GPIO_PUPD_NO);
	GPIO_PUPDR_SET(B, 3, GPIO_PUPD_NO);
	
	GPIO_OTYPER_SET(B, 0, GPIO_OTYPE_OD);
	GPIO_OTYPER_SET(B, 1, GPIO_OTYPE_OD);
	GPIO_OTYPER_SET(B, 2, GPIO_OTYPE_OD);
	GPIO_OTYPER_SET(B, 3, GPIO_OTYPE_OD);	
}

/********************************************************************
* KeyPad_MatrixScan() - Checks what key is pressed on matrix key pad.
* No inputs.
* Returns the character that was pressed on the keypad.
********************************************************************/
uint8_t KeyPad_MatrixScan(void){
	//set row output to 0 (pb0-pb3)
	// [if] all column inputs are 1? (pb4-pb7)
	//	if no, button is pressed somewhere
	//		1. debounce
	//			option 1 - delay 10us before continue
	//			option 2 - deploy a 5-count 0's counter (check 5 times in a row)
	//		2. set R0 to 0 and the remaining three columns to 1
	//			check if all columns are 1
	//				if all columns are 1, no button is pressed in this row
	//				else, check which column is 0, and output the corresponding constants for R0C# (row 0 column #)
	//		3. repeate step 2 for the remaining three rows
	//	if yes no buttons are pressed, exit the scanner

	GPIOB -> ODR &= ~(1UL << (1*0));
	GPIOB -> ODR &= ~(1UL << (1*1));
	GPIOB -> ODR &= ~(1UL << (1*2));
	GPIOB -> ODR &= ~(1UL << (1*3));
	
	//debounce
	// check for 5 0s in a row
	for(int i = 0; i < 5; i++){
		if(IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7)){
			return('f'); // no button is pressed
		}
		Delay_ms(10);
	}
	
	// CHECKING WHAT BUTTON IS PRESSED
	GPIOB -> ODR &= ~(1UL << (1*0));
	GPIOB -> ODR |= (1UL << (1*1));
	GPIOB -> ODR |= (1UL << (1*2));
	GPIOB -> ODR |= (1UL << (1*3));
	if(!(IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7))){
		if(!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4)){
			return('1');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5)){
			return('2');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6)){
			return('3');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7)){
			return('A');
		}
		//button in this row is pressed
		//return something
	}
	
	GPIOB -> ODR |= (1UL << (1*0));
	GPIOB -> ODR &= ~(1UL << (1*1));
	GPIOB -> ODR |= (1UL << (1*2));
	GPIOB -> ODR |= (1UL << (1*3));
	if(!(IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7))){
		//button in this row is pressed
		//return something
		if(!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4)){
			return('4');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5)){
			return('5');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6)){
			return('6');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7)){
			return('B');
		}
		//button in this row is pressed
		//return something
	}
	
	GPIOB -> ODR |= (1UL << (1*0));
	GPIOB -> ODR |= (1UL << (1*1));
	GPIOB -> ODR &= ~(1UL << (1*2));
	GPIOB -> ODR |= (1UL << (1*3));
	if(!(IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7))){
		//button in this row is pressed
		//return something
		if(!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4)){
			return('7');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5)){
			return('8');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6)){
			return('9');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7)){
			return('C');
		}
		//button in this row is pressed
		//return something
	}
	
	GPIOB -> ODR |= (1UL << (1*0));
	GPIOB -> ODR |= (1UL << (1*1));
	GPIOB -> ODR |= (1UL << (1*2));
	GPIOB -> ODR &= ~(1UL << (1*3));
	if(!(IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6) & IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7))){
		//button in this row is pressed
		//return something
		if(!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_4)){
			return('*');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_5)){
			return('0');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_6)){
			return('#');
		}
		else if (!IS_BIT_SET(GPIOB -> IDR, GPIO_IDR_7)){
			return('D');
		}
		//button in this row is pressed
		//return something
	}
	return('f');
}

/***************************************************************
* KeyPad_GetKey() - Checks if key is pressed on maxtrix key pad.
* No inputs.
* No return value.
***************************************************************/
uint8_t KeyPad_GetKey(void){
		uint8_t pressedKey = '\0';
		static uint8_t lastKey = 'f';
	
		pressedKey = KeyPad_MatrixScan();
		if(pressedKey != 'f' && lastKey == 'f'){
			lastKey = pressedKey;	
			return(pressedKey);
		}
		lastKey = pressedKey;	
		return('f');
}
