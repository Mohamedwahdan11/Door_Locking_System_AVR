/*
 * timer.h
 *
 *  Created on: Jul 3, 2022
 *      Author: DELL
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/


/*enum type to take the timer mode*/
enum mode {OVF,COMPARE,COMPARE_A,COMPARE_B};
/*enum type to take the timer prescale value for timer2*/
enum timer_pre_scale_2{pr_scale_1=1,pr_scale_8,pr_scale_32,pr_scale_64,pr_scale_128,pr_scale_256,pr_scale_1024};
/*enum type to take the timer prescale value for timer 0 & 1 */
enum timer_pre_scale_0_1{scale_1=1,scale_8,scale_64,scale_256,scale_1024};


/* type structure to take the configuration of timer 0*/
struct timer0_conf{
	uint8 int_value;
	uint8 comp_value;
	enum timer_pre_scale_0_1 scale;
	enum mode mode_value;

};
/* type structure to take the configuration of timer 1*/
struct timer1_conf{
	uint16 int_value;
	uint16 comp_value;
	enum timer_pre_scale_0_1 scale;
	enum mode mode_value;

};
/* type structure to take the configuration of timer 2*/
struct timer2_conf{
	uint8 int_value;
	uint8 comp_value;
	enum timer_pre_scale_2 scale;
	enum mode mode_value;

};




/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/*
 * function to turn on timer0
 */

void timer0_init(struct timer0_conf*conf);


/*
 * function to turn on timer1
 */
void timer1_init(struct timer1_conf*conf);
/*
 * function to turn on timer2
 */

void timer2_init(struct timer2_conf*conf );

/*
 * function to turn off timer0
 */

void timer_timer0_deinit(void);
/*
 * function to turn off timer1
 */

void timer_timer1_deinit(void);


/*
 * function to turn off timer2
 */

void timer_timer2_deinit(void);

/*
 * Description: Function to set the Call Back function address for timer 2.
 */

void timer_setCallBack_timer2(void(*a_ptr)(void));

/*
 * Description: Function to set the Call Back function address for timer 1.
 */

void timer_setCallBack_timer1(void(*a_ptr)(void));

/*
 * Description: Function to set the Call Back function address for timer 0.
 */

void timer_setCallBack_timer0(void(*a_ptr)(void));

#endif /* TIMER_H_ */
