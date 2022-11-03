/*
 * dc_motor.h
 *
 *  Created on: May 24, 2022
 *      Author: DELL
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

typedef enum
{
	OFF,CLK_WISE,ANTI_CLK_WISE
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the DC Motor driver..
 */

void DcMotor_Init(void);

/*
 * Description :
 * Function responsible for turn the fan off or on with specific speed (duty cycle)
 * and with certain direction as clock wise or anti-clock wise.
 */


void DcMotor_Rotate(DcMotor_State state,uint8 speed);



#endif /* DC_MOTOR_H_ */
