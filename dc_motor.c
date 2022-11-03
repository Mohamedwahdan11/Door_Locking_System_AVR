/*
 * dc_motor.c
 *
 *  Created on: May 24, 2022
 *      Author: DELL
 */

#include "gpio.h"
#include "dc_motor.h"
#include "pwm.h"








void DcMotor_Init(void)
{
	/*make the dc motor pins as output pins*/
	GPIO_setupPinDirection(PORTB_ID, PIN0_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(PORTB_ID, PIN1_ID, PIN_OUTPUT);
	/*initialize the pins with logic 0 to make the fan off at the beginning*/
	GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_LOW);
	GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_LOW);
}

/*Description:
	 * 	if condition to set the mode of the fan where it is off or with
	 * 	clock wise direction or anti-clock wise direction
	 * the function return : void
	 * the function argument:uint8 speed and enum state;
	 */


void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	/*if condition to set the mode of the fan where it is off or with
	 * 	clock wise direction or anti-clock wise direction
	 * 	*/



	if(state==ANTI_CLK_WISE)
	{
		GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_LOW);
		GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_HIGH);

	}
	else if(state==CLK_WISE)
	{
		GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_HIGH);
		GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_LOW);


	}
	else
	{
		GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_LOW);
		GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_LOW);

	}

	/*set the PWM sith the wanted speed*/
	PWM_init(speed);



}
