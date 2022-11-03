/*
 * buzzer.c
 *
 *  Created on: Jun 26, 2022
 *      Author: DELL
 */
#include "buzzer.h"
#include "gpio.h"








void buzzer_Init(void)
{
	/*make the buzzer pin as output pin*/
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN , PIN_OUTPUT);


	/*initialize the pins with logic 0 to make the buzzer off at the beginning*/
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);

}

/*Description:
	 * 	function to set the mode of the buzzer  off
	 * the function return : void
	 * the function argument:void;
	 */


void buzzer_stop(void)
{

	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);

}

/*Description:
	 * 	function to set the mode of the buzzer on
	 * the function return : void
	 * the function argument:void;
	 */


void buzzer_on(void)
{

	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_HIGH);

}

