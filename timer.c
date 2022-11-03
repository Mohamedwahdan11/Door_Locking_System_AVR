/*
 * timer.c
 *
 *  Created on: Jul 3, 2022
 *      Author: DELL
 */


#include "timer.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/interrupt.h> /* For UART ISR */

/* Global variables to hold the address of the call back function in the application */
	static volatile void (*g_callBackPtr_timer0)(void) = NULL_PTR;
	static volatile void (*g_callBackPtr_timer1)(void) = NULL_PTR;
	static volatile void (*g_callBackPtr_timer2)(void) = NULL_PTR;

	/*******************************************************************************
	 *                       Interrupt Service Routines                            *
	 *******************************************************************************/

	ISR(TIMER2_COMP_vect)
	{
		if(g_callBackPtr_timer2 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_timer2)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}
	ISR(TIMER2_OVF_vect)
	{
		if(g_callBackPtr_timer2 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_timer2)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}
	ISR(TIMER1_COMPA_vect)
	{
		if(g_callBackPtr_timer1 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_timer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}
	ISR(TIMER1_COMPB_vect)
		{
			if(g_callBackPtr_timer1 != NULL_PTR)
			{
				/* Call the Call Back function in the application after the receiving data */
				(*g_callBackPtr_timer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
			}
		}
	ISR(TIMER1_OVF_vect)
	{
		if(g_callBackPtr_timer1 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_timer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}
	ISR(TIMER0_COMP_vect)
	{
		if(g_callBackPtr_timer0 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_timer0)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}
	ISR(TIMER0_OVF_vect)
	{
		if(g_callBackPtr_timer0 != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_timer0)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/




	void timer0_init(struct timer0_conf* conf)
	{
		TCNT0=conf->int_value;
		OCR0=conf->comp_value;
		if(conf->mode_value==OVF)
		{
			TCCR0=(1<<FOC0)|conf->scale;
			TIMSK|=(1<<TOIE0);
		}

		else if(conf->mode_value==COMPARE)
		{
			TCCR0=(1<<FOC0)|(1<<WGM01)|conf->scale;
			TIMSK|=(1<<OCIE0);
		}


	}

	void timer1_init(struct timer1_conf* conf)
	{
		TCNT1=conf->int_value;
		if(conf->mode_value==OVF)
		{
			TCCR1A=(1<<FOC1A)|(1<<FOC1B);
			TCCR1B=conf->scale;
			TIMSK|=(1<<TOIE1);
		}

		else if(conf->mode_value==COMPARE_A)
		{
			OCR1A=conf->comp_value;
			TCCR1A=(1<<FOC1A)|(1<<FOC1B);
			TCCR1B=(1<<WGM12)|conf->scale;
			TIMSK|=(1<<OCIE1A);
		}
		else if(conf->mode_value==COMPARE_B)
		{
			OCR1B=conf->comp_value;
			TCCR1A=(1<<FOC1A)|(1<<FOC1B);
			TCCR1B=(1<<WGM12)|conf->scale;
			TIMSK|=(1<<OCIE1B);
		}



	}

	void timer2_init(struct timer2_conf* conf )
	{
		TCNT2=conf->int_value;
		OCR2=conf->comp_value;
		if(conf->mode_value==OVF)
		{
			TCCR2=(1<<FOC0)|conf->scale;
			TIMSK|=(1<<TOIE2);
		}

		else if(conf->mode_value==COMPARE)
		{
			TCCR2=(1<<FOC0)|(1<<WGM01)|conf->scale;
			TIMSK|=(1<<OCIE2);
		}


	}


/*
 * function to turn off timer 0 by setting the register TCCR0 with 0
 * */

void timer_timer0_deinit(void)
{
	TCCR0=0;
}

/*
 * function to turn off timer 1 by setting the register TCCR0 with 0
 * */

void timer_timer1_deinit(void)
{
	TCCR1B=0;
}

/*
 * function to turn off timer 2 by setting the register TCCR0 with 0
 * */

void timer_timer2_deinit(void)
{
	TCCR2=0;
}


/*
 * function to set the call back address that will be called by the ISR
 * */

void timer_setCallBack_timer0(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr_timer0 = a_ptr;
}

/*
 * function to set the call back address that will be called by the ISR
 * */


void timer_setCallBack_timer1(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr_timer1 = a_ptr;
}

/*
 * function to set the call back address that will be called by the ISR
 * */


void timer_setCallBack_timer2(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr_timer2 = a_ptr;
}

