 /******************************************************************************
 *
 * Module: ICU
 *
 * File Name: icu.c
 *
 * Description: Source file for the AVR ICU driver
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

#include "icu.h"
#include <avr/io.h> /*To access Timer1 Registers*/
#include <avr/interrupt.h> /*To Type ICU ISR*/

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Global variable to hold the address of the call back function*/
static volatile void (*g_callBackPtr)(void)=NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_CAPT_vect)
{
	if(g_callBackPtr!=NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the ICU driver
 * 	1. Set the required clock.
 * 	2. Set the required edge detection.
 * 	3. Enable the Input Capture Interrupt.
 * 	4. Initialize Timer1 Registers
 */
void Icu_init(const Icu_ConfigType * Config_Ptr )
{
	/* Configure ICP1/PD6 as i/p pin */
	DDRD &= ~(1<<PD6);

	/*Set FOCA & FOC1B, and clear other bits in TCCR1A*/
	TCCR1A=(1<<FOC1A)|(1<<FOC1B);

	/*Configure Edge & clock for ICU*/
	TCCR1B=(TCCR1B&0xA0)|(Config_Ptr->clock)|(Config_Ptr->edge<<ICES1);

	/*initialize Timer1 counter*/
	TCNT1=0;

	/* Initial Value for the input capture register */
	ICR1 = 0;

	/*Enable ICU interrupt*/
	TIMSK|=(1<<TICIE1);
}
/*
 * Description: Function to set the Call Back function address.
 */
void Icu_setCallBack(void(*a_ptr)(void))
{
	/*assign the address of the call back function to the global variable to be able
	 * to call it in the ISR
	 */
	g_callBackPtr=a_ptr;
}
/*
 * Description: Function to set the required edge detection.
 */
void Icu_setEdgeDetectionType(const Icu_EdgeType edgeType)
{
	/*Set the edge selection bit in TCCR1B as required*/
	TCCR1B=(TCCR1B&0xBF)|(edgeType<<ICES1);
}
/*
 * Description: Function to get the Timer1 Value when the input is captured
 *              The value stored at Input Capture Register ICR1
 */
uint16 Icu_getInputCaptureValue(void)
{
	return ICR1;
}
/*
 * Description: Function to clear the Timer1 Value to start count from ZERO
 */
void Icu_clearTimerValue(void)
{
	/*Set TCNT1 to equal zero*/
	TCNT1=0;
}
/*
 * Description: Function to disable the Timer1 to stop the ICU Driver
 */
void Icu_DeInit(void)
{
	/*Clear Timer1 registers*/
	TCCR1A=0;
	TCCR1B=0;
	TCNT1=0;
	ICR1=0;

	/*Disable ICU interrupt*/
	TIMSK&=~(1<<TICIE1);
}
