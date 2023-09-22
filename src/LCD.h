/********************************************************************************
* Name: LCD.h (interface)
* Author(s): Noah Grant, Wyatt Richard
* Date: March 31, 2023
* Description: LCD functions for mobile robot.
********************************************************************************/

#ifndef LCD_H
#define LCD_H

#include "../stm32-base/CMSIS/inc/stm32f303xe.h"

// Command to LCD module
#define LCD_CMD_CLEAR				0x01			// Clear screen and set DDRAM address to 0
#define LCD_CMD_HOME				0x02			// Return home (set DDRAM address to 0)
#define LCD_CMD_ENTRY				0x04			// Entry mode set
#define LCD_CMD_DISPLAY			0x08			// Control ON/OFF of display, cursor, and blink
#define LCD_CMD_CDSHIFT			0x10			// Cursor/display shift
#define LCD_CMD_FUNCTION		0x20			// Set of data length, lines, and font type
#define LCD_CMD_CGRAMADDR		0x40			// Set code generator CG-RAM addfress A5-A0
#define LCD_CMD_SETDDADDR		0x80			// Set display memory DDRAM addfress A6-A0

// Settings for LCD Command Entry
#define LCD_ENTRY_MOVE_DISPLAY	0x01
#define LCD_ENTRY_MOVE_CURSOR		0x00
#define LCD_ENTRY_INC						0x02
#define LCD_ENTRY_DEC						0x00

// Settings for LCD Command Display
#define LCD_DISPLAY_BLINK				0x01
#define LCD_DISPLAY_NOBLINK			0x00
#define LCD_DISPLAY_CURSOR			0x02
#define LCD_DISPLAY_NOCURSOR		0x00
#define LCD_DISPLAY_ON					0x04
#define LCD_DISPLAY_OFF					0x00

// Settings for LCD CD Shift (Shift Cursor or Display without changing DDRAM data)
#define LCD_CDSHIFT_RIGHT				0x04
#define LCD_CDSHIFT_LEFT				0x00
#define LCD_CDSHIFT_DISPLAY			0x08
#define LCD_CDSHIFT_CURSOR			0x00

// Settings for LCD function commands
#define LCD_FUNCTION_5X10FONT		0x04
#define LCD_FUNCTION_5X8FONT		0x00
#define LCD_FUNCTION_2LINES			0x08
#define LCD_FUNCTION_1LINE			0x00
#define LCD_FUNCTION_8BITBUS		0x10
#define LCD_FUNCTION_4BITBUS		0x00

// Common LCD Operation Delays
#define LCD_LONG_CMD_DELAY			16
#define LCD_STD_CMD_DELAY				2
#define TEST_DELAY							16

#define LCD_DDRAM_ADDR_LINE1		0x00
#define LCD_DDRAM_ADDR_LINE2		0x40

// GPIO Port Constants
#define LCD_GPIO_PORT						A
#define LCD_PORT								GPIOA->ODR
#define LCD_RS_BIT							(1UL << 6)				//PA6
#define LCD_E_BIT								(1UL << 7)				//PA7
#define LCD_BUS_BIT							(0xFUL << 8)		//PA8, 9, 10, and 11
#define LCD_BUS_BIT_POS					8

#define LCD_PORT_BITS						(LCD_RS_BIT | LCD_E_BIT | LCD_BUS_BIT)	//0x07E0	// bit 6, 7, 8, 9, and 11

// LCD Operation Helper Macros
#define LCD_E_LO					CLEAR_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_E_HI					SET_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_RS_IR					CLEAR_BITS( LCD_PORT, LCD_RS_BIT)
#define LCD_RS_DR					SET_BITS(LCD_PORT, LCD_RS_BIT)
#define LCD_BUS(value)		FORCE_BITS(LCD_PORT, LCD_BUS_BIT, (value) << LCD_BUS_BIT_POS)

// Other Constants
#define MAX_LCD_BUFSIZE		81	//80 characters + 1 null char

// LCD functions
void LCD_Init(void);
void LCD_Clear(void);
void LCD_HomeCursor(void);

void LCD_cmd(uint8_t cmd);
void LCD_data(uint8_t data);
	
void LCD_putc(unsigned char ch);
void LCD_puts(char* str);
void LCD_printf(char* str, ... );

void LCD_CustomChar(uint8_t character[8], uint8_t address);
void LCD_SetCustomCharIdentifier(uint8_t character[8]);

#endif
