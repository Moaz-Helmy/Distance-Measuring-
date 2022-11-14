/******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Description: Source file for the LCD driver
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/
#include "lcd.h"
#include "gpio.h"
#include "common_macros.h"/*To use macro GET_BIT*/
#include <stdlib.h>/*To use itoa function*/
#include <util/delay.h>/*To use delay function*/

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the LCD:
 * 1. Setup the LCD pins directions by use the GPIO driver.
 * 2. Setup the LCD Data Mode 4-bits or 8-bits.
 */
void LCD_init(void)
{
	/* Configure the direction for RS and E pins as output pins */
	GPIO_setupPinDirection(LCD_RS_PORT_ID,LCD_RS_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_E_PORT_ID,LCD_E_PIN_ID,PIN_OUTPUT);

#ifdef LCD_RW_IS_CONNECTED_TO_MCU
	/* Configure the direction for RW pin as output pin */
	GPIO_setupPinDirection(LCD_RW_PORT_ID, LCD_RW_PIN_ID, PIN_OUTPUT);
	/* Write 0 to RW*/
	GPIO_writePin(LCD_RW_PORT_ID,LCD_RW_PIN_ID, LOGIC_LOW);
#endif
	/* LCD Power ON delay always > 15ms */
	_delay_ms(20);

#if (LCD_DATA_BITS_MODE==8)
	/* Configure the data port as output port */
	GPIO_setupPortDirection(LCD_DATA_PORT_ID,PORT_OUTPUT);

	/* use 2-lines LCD + 8-bits Data Mode + 5*7 dot display Mode */
	LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);

#elif(LCD_DATA_BITS_MODE==4)
	/* Configure 4 pins in the data port as output pins */
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DATA_DB4_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DATA_DB5_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DATA_DB6_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DATA_DB7_PIN_ID,PIN_OUTPUT);

	/* Send for 4 bit initialization of LCD  */
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT1);
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT2);

	/* use 2-lines LCD + 4-bits Data Mode + 5*7 dot display Mode */
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);
#endif

	/* cursor off */
	LCD_sendCommand(LCD_CURSOR_OFF);

	/* clear LCD at the beginning */
	LCD_sendCommand(LCD_CLEAR_COMMAND);
}

/*
 * Description :
 * Send the required command to the screen
 */
void LCD_sendCommand(uint8 command)
{
	/* Instruction Mode RS=0 */
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Tas = 50ns */
	/* Enable LCD E=1 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);/* delay for processing Tpw - Tdws = 190ns */

#if (LCD_DATA_BITS_MODE==8)

	/* out the required command to the data bus D0 --> D7 */
	GPIO_writePort(LCD_DATA_PORT_ID,command);
	_delay_ms(1);/* delay for processing Tdsw = 100ns */
	/* Disable LCD E=0 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Th = 13ns */

#elif(LCD_DATA_BITS_MODE==4)

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB4_PIN_ID,GET_BIT(command,PIN4_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB5_PIN_ID,GET_BIT(command,PIN5_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB6_PIN_ID,GET_BIT(command,PIN6_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB7_PIN_ID,GET_BIT(command,PIN7_ID));

	_delay_ms(1);/* delay for processing Tdsw = 100ns */
	/* Disable LCD E=0 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Th = 13ns */
	/* Enable LCD E=1 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);/* delay for processing Tpw - Tdws = 190ns */

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB4_PIN_ID,GET_BIT(command,PIN0_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB5_PIN_ID,GET_BIT(command,PIN1_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB6_PIN_ID,GET_BIT(command,PIN2_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB7_PIN_ID,GET_BIT(command,PIN3_ID));

	_delay_ms(1);/* delay for processing Tdsw = 100ns */
	/* Disable LCD E=0 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Th = 13ns */

#endif

}

/*
 * Description :
 * Display the required character on the screen
 */
void LCD_displayCharacter(uint8 character)
{
	/* Data Mode RS=1 */
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);/* delay for processing Tas = 50ns */
	/* Enable LCD E=1 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);/* delay for processing Tpw - Tdws = 190ns */

#if (LCD_DATA_BITS_MODE==8)

	/* out the required command to the data bus D0 --> D7 */
	GPIO_writePort(LCD_DATA_PORT_ID,character);
	_delay_ms(1);/* delay for processing Tdsw = 100ns */
	/* Disable LCD E=0 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Th = 13ns */

#elif(LCD_DATA_BITS_MODE==4)

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB4_PIN_ID,GET_BIT(character,PIN4_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB5_PIN_ID,GET_BIT(character,PIN5_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB6_PIN_ID,GET_BIT(character,PIN6_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB7_PIN_ID,GET_BIT(character,PIN7_ID));

	_delay_ms(1);/* delay for processing Tdsw = 100ns */
	/* Disable LCD E=0 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Th = 13ns */
	/* Enable LCD E=1 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);/* delay for processing Tpw - Tdws = 190ns */

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB4_PIN_ID,GET_BIT(character,PIN0_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB5_PIN_ID,GET_BIT(character,PIN1_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB6_PIN_ID,GET_BIT(character,PIN2_ID));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DATA_DB7_PIN_ID,GET_BIT(character,PIN3_ID));

	_delay_ms(1);/* delay for processing Tdsw = 100ns */
	/* Disable LCD E=0 */
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);/* delay for processing Th = 13ns */

#endif
}

/*
 * Description :
 * Display the required string on the screen
 */
void LCD_displayString(const char* string)
{
	uint8 str_index=0;
	for(str_index=0;string[str_index]!='\0';str_index++)
	{
		LCD_displayCharacter(string[str_index]);
	}
}

/*
 * Description :
 * Move the cursor to a specified row and column index on the screen
 */
void LCD_moveCursor(uint8 row,uint8 col)
{
	uint8 lcd_memory_address;

	/* Calculate the required address in the LCD DDRAM */
	switch(row)
	{
	case 0:
		lcd_memory_address=col;
		break;
	case 1:
		lcd_memory_address=col+0x40;
		break;
	case 2:
		lcd_memory_address=col+0x10;
		break;
	case 3:
		lcd_memory_address=col+0x50;
		break;
	}

	/* Move the LCD cursor to this specific address */
	LCD_sendCommand(lcd_memory_address|LCD_SET_CURSOR_LOCATION);

}

/*
 * Description :
 * Display the required string in a specified row and column index on the screen
 */
void LCD_displayStringRowColumn(uint8 row,uint8 col,const char*string)
{
	/* go to to the required LCD position */
	LCD_moveCursor(row,col);
	/* display the string */
	LCD_displayString(string);
}

/*
 * Description :
 * Send the clear screen command
 */
void LCD_clearScreen(void)
{
	/* Send clear display command */
	LCD_sendCommand(LCD_CLEAR_COMMAND);
}

/*
 * Description :
 * Display the required decimal value on the screen
 */
void LCD_integerToString(long data)
{
	/* String to hold the asci result */
	char buff[16];
	/* Use itoa C function to convert the data to its corresponding ASCII value, 10 for decimal */
	itoa(data,buff,10);
	/* Display the string */
	LCD_displayString(buff);
}
