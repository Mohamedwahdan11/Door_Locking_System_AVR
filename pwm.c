/*
 *
 * Module: PWM
 * pwm.c
 *
 *  Created on: May 23, 2022
 *      Author: Mohamed Wahdan
 */

#include "avr/io.h" /* To use the TIMER0 Registers */
#include "pwm.h" /* To use the macros like SET_BIT */
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


void PWM_init(uint8 duty_cycle)
{
	TCNT0=0;   // Set The Timer Register With 0


	/*Set Port B Pin 3 (OC0) as Output Pin*/

	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);

	/* TCCR0 Register Bits Description:
		 * FOC0       = only active with no PWM mode
		 * WGM01:00   = Wave Generation Mode
		 * COM01:00   = Compare Match Output Mode
		 * CS02:01:00 = Clock Set
		 */

	TCCR0=(1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS01);

	/*Set the Compare register value*/
	switch (duty_cycle){
			default:
			case 100:
				OCR0=255;
				break;
			case 75:
				OCR0=192;
				break;
			case 50:
				OCR0=128;
				break;
			case 25:
				OCR0=64;
				break;
			}


}








