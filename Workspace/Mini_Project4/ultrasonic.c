/******************************************************************************
 *
 * Module: Ultrasonic Sensor
 *
 * File Name: ultrasonic.c
 *
 * Description: Source file for the Ultrasonic Sensor driver
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

#include "ultrasonic.h"
#include "gpio.h"
#include "icu.h"
#include <util/delay.h>/*To use 10us delay in Ultrasonic_Trigger() function*/
#include <math.h> /*To use the ceil() function in Ultrasonic_readDistance() function*/

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/*Flag to tell whether to start distance calculations or not, i.e., ISR has happened
 * and edgeProcessing() has finished calculating the high time.
 */
static volatile uint8 g_startCalculations=0;

/*Global variable to store the calculated high time*/
static volatile uint16 g_highTime=0;

/*This interrupt counter is used to aid in calculating the high time in the edgeProcessing function*/
static volatile uint8 g_interruptCounter=0;

/*******************************************************************************
 *                     Static Functions Prototypes                             *
 *******************************************************************************/
/*
 * Description: This function is responsible for:
 * 1- Sending the trigger pulse to the ultrasonic sensor.
 */
static void Ultrasonic_Trigger(void);

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description: This function is responsible for:
 * 1- Initialize The ICU module as required.
 * 2- Setup the ICU call back function.
 * 3- Setup the direction for the trigger pin as output pin through the GPIO driver.
 */
void Ultrasonic_init(void)
{
	/*Configuration Structure for the ICU*/
#if (ICU_TIMER_PRESCALER == 1)

	Icu_ConfigType Icu_Config_Struct={F_CPU_CLOCK,RISING};

#elif (ICU_TIMER_PRESCALER == 8)

	Icu_ConfigType Icu_Config_Struct={F_CPU_8,RISING};

#elif (ICU_TIMER_PRESCALER == 64)

	Icu_ConfigType Icu_Config_Struct={F_CPU_64,RISING};

#elif (ICU_TIMER_PRESCALER == 256)

	Icu_ConfigType Icu_Config_Struct={F_CPU_256,RISING};

#elif (ICU_TIMER_PRESCALER == 1024)

	Icu_ConfigType Icu_Config_Struct={F_CPU_1024,RISING};

#endif
	/*Initialize the ICU*/
	Icu_init(&Icu_Config_Struct);

	/*Send the call back function of the ultrasonic to the ICU driver
	 * to be called when an edge is detected, .i.e, interrupt.
	 */
	Icu_setCallBack(Ultrasonic_edgeProcessing);

	/*Setup the direction of the trigger pin as output pin*/
	GPIO_setupPinDirection(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, PIN_OUTPUT);
}

/*
 * Description: This function is responsible for:
 * 1- Sending the trigger pulse to the ultrasonic sensor.
 */
static void Ultrasonic_Trigger(void)
{
	/*Set the trigger Pin high for 10 us*/
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, LOGIC_HIGH);
	_delay_us(TRIGGER_PIN_DELAY_TIME_MICROSECONDS);

	/*Clear the trigger pin*/
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID, LOGIC_LOW);
}

/*
 * Description: This function is responsible for:
 * 1- Send the trigger pulse by using Ultrasonic_Trigger function.
 * 2- Start the measurements by the ICU from this moment.
 */
uint16 Ultrasonic_readDistance(void)
{
	static uint16 distance=0;

	/*Send the trigger pulse*/
	Ultrasonic_Trigger();

	/*Start the high time, when high time is calculated, .i.e, The g_startCalculations=1*/
	if(g_startCalculations==1)
	{
		/*Set the flag to zero for the next time*/
		g_startCalculations=0;

		/*Calculate the distance, then return it*/
		distance=(uint16)ceil((((float64)(SOUND_SPEED_CENTIMETERS_SECONDS/2)/(float64)(FREQ_CPU_CLOCK))*((float64)g_highTime)*(float64)ICU_TIMER_PRESCALER));

		/*Error handling at specific cases. After this handling, the driver can accurately calculate the distance up to 4 meters with no error*/
		if(distance>=59&&distance<129)
		{
			/*if distance >= 59 the error is -1, so we add 1 to distance*/
			distance+=1;
		}
		else if(distance>=129&&distance<199)
		{
			/*if distance >= 129 the error is -2, so we add 2 to distance*/
			distance+=2;
		}
		else if(distance>=199&&distance<270)
		{
			/*if distance >= 199 the error is -3, so we add 3 to distance*/
			distance+=3;
		}
		else if(distance>=270&&distance<343)
		{
			/*if distance >= 270 the error is -4, so we add 4 to distance*/
			distance+=4;
		}
		else if(distance>=343)
		{
			/*if distance >= 270 the error is -5, so we add 5 to distance*/
			distance+=5;
		}

		/*Return the modified distance*/
		return distance;

	}
	else
	{
		/*return old calculated distance for now till new distance is calculated, because it's a static variable*/
		return distance;
	}

}

/*
 * Description: This function is responsible for:
 * 1- This is the call back function called by the ICU driver.
 * 2- This is used to calculate the high time (pulse time) generated by the ultrasonic sensor.
 */
void Ultrasonic_edgeProcessing(void)
{
	g_interruptCounter++;
	/*Check for first rising edge*/
	if(g_interruptCounter==1)
	{
		/*Clear the timer to start high time calculation from this moment*/
		Icu_clearTimerValue();
		/*Set ICU to operate with falling edge to get the high time when ISR is called next time*/
		Icu_setEdgeDetectionType(FALLING);
	}
	else if(g_interruptCounter==2)
	{
		/*Read the highTime value*/
		g_highTime=Icu_getInputCaptureValue();
		/*Set the ICU to operate with rising edge again, .i.e reinitialize the ICU for next distance calculation*/
		Icu_setEdgeDetectionType(RISING);
		/*Reset the interrupt counter*/
		g_interruptCounter=0;
		/*set the g_startCalculations=1 to allow the Ultrasonic_readDistance(void) to do distance calculation*/
		g_startCalculations=1;
	}
	else
	{
		/*Do Nothing, the program will never enter here*/
	}
}

