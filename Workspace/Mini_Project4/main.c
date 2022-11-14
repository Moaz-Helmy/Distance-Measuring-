/******************************************************************************
 *
 * Title: 		Distance Measuring Project
 *
 * Description: This application uses GPIO, ICU, LCD, Ultrasonic Sensor
 * 				to measure the distance sensed by the Ultrasonic sensor, then
 * 				display this distance on the LCD
 *
 * Author:		Moaz Mohamed
 *
 * Date: 		11/10/2022
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "lcd.h"
#include "ultrasonic.h"
#include <avr/io.h>/*To access SREG register*/

/*********************Program starts here*********************/
int main(void)
{
	/*variable to store the distance*/
	uint16 distance=0;

	/* Enable Global Interrupt I-Bit */
	SREG |= (1<<7);

	/*Hardware Initializations*/
	LCD_init();
	Ultrasonic_init();

	/*Display "Distance= " only once at the beginning*/
	LCD_displayString("Distance= ");

	/*Program Logic*/
	while(1)
	{
		/*Read the distance sensed by the ultrasonic sensor*/
		distance=Ultrasonic_readDistance();

		/*Display this distance at row 0 and column 10 on the LCD*/
		LCD_moveCursor(0, 10);
		LCD_integerToString(distance);

		/*Add "cm" notation to the displayed number*/
		LCD_displayString("cm ");

	}
}

