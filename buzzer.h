/*
 * buzzer.h
 *
 *  Created on: Jun 26, 2022
 *      Author: DELL
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/*define the buzzer connection pins*/
#define BUZZER_PORT PORTB_ID
#define BUZZER_PIN PIN2_ID


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the Buzzer driver..
 */

void buzzer_Init(void);

/*
 * Description :
 * Function responsible for turn the  off  the Buzzer.
 */


void buzzer_stop(void);

/*
 * Description :
 * Function responsible for turn the on the Buzzer.
 */


void buzzer_on(void);


#endif /* BUZZER_H_ */
