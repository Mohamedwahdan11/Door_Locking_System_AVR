/*
 * uart.h
 *
 *  Created on: Jul 3, 2022
 *      Author: DELL
 */
#ifndef UART_H_
#define UART_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*
 * define a register type data type to be used to take the register values as bits or as one
 * byte */

union reg_8bits{
	uint8 data;
	struct{
		uint8 bit0:1;
		uint8 bit1:1;
		uint8 bit2:1;
		uint8 bit3:1;
		uint8 bit4:1;
		uint8 bit5:1;
		uint8 bit6:1;
		uint8 bit7:1;

	}bits;
};

/*
 * define structure which take the int values of the UART
 * i used  UCSRB and UCSRC in this way not by sending the
 * "word size,stop bits" to have the ability to set and reset the
 * interrupts, i know i can make it by sending like 5th term to active
 * TX interrupt or RX interrupt or both, but i like it in this way   */

struct uart_conf{
	uint32 baud_rate;
	union reg_8bits ucsrb;
	union reg_8bits ucsrc;

};


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 * 4. set up interrupts
 */
void UART_init(struct uart_conf* state);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

/*
 * Description:
 * Function to set the Call Back function address for the receiver interrupt.
 */
void UART_setCallBack_RX(void(*a_ptr)(void));

/*
 * Description:
 * Function to set the Call Back function address for the transmitter interrupt.
 */
void UART_setCallBack_TX(void(*a_ptr)(void));


#endif /* UART_H_ */
