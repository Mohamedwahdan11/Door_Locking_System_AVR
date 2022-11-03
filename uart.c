/*
 * uart.c
 *
 *  Created on: Jul 3, 2022
 *      Author: DELL
 */
#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/interrupt.h> /* For UART ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
	static volatile void (*g_callBackPtr_RX)(void) = NULL_PTR;
	/* Global variables to hold the address of the call back function in the application */
	static volatile void (*g_callBackPtr_TX)(void) = NULL_PTR;

	/*******************************************************************************
	 *                       Interrupt Service Routines                            *
	 *******************************************************************************/

	/*
	 * the ISR for reviver interrupt */

	ISR(USART_RXC_vect)
	{

		if(g_callBackPtr_RX != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_RX)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}

	/*
	 * the ISR for transmission interrupt */

	ISR(USART_TXC_vect)
	{
		if(g_callBackPtr_TX != NULL_PTR)
		{
			/* Call the Call Back function in the application after the receiving data */
			(*g_callBackPtr_TX)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
	}



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 * 4. setup interrupt
 */
void UART_init(struct uart_conf* state)
{
	uint16 ubrr_value = 0;

	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);

	/************************** UCSRB Description **************************
	 * RXCIE bit_0 = Disable/Enable USART RX Complete Interrupt Enable
	 * TXCIE bit_1 = Disable/Enable USART Tx Complete Interrupt Enable
	 * UDRIE bit_2 = Disable/Enable USART Data Register Empty Interrupt Enable
	 * RXEN  bit_3 = Receiver Enable
	 * RXEN  bit_4 = Transmitter Enable
	 * UCSZ2 bit_5 = For chose the data bit data mode
	 * RXB8  bit_6 = the 9th bit in the 9 bits mode for RX
	 * TXB8  bit_7 = the 9th bit in the 9 bits mode for TX
	 ***********************************************************************/ 
	UCSRB = state->ucsrb.data;
	
	/************************** UCSRC Description **************************
	 * URSEL bit_0  = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL bit_1  =   Asynchronous/Synchronous Operation
	 * UPM1  bit_2  =   parity bit mode
	 * UPM0  bit_3  =   parity bit mode
	 * USBS  bit_4  =   One/two stop bit
	 * UCSZ1 bit_5  =   For chose the data bit data mode
	 * UCSZ0 bit_6  =   For chose the data bit data mode
	 * UCPOL bit_7  =   Used with the Synchronous operation only
	 ***********************************************************************/ 	
	UCSRC = state->ucsrc.data;
	
	/* Calculate the UBRR register value */
	ubrr_value = (uint16)(((F_CPU / (state->baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{

	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */

    return UDR;		
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();

	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
void UART_setCallBack_RX(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr_RX  = a_ptr;
}



void UART_setCallBack_TX(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr_TX = a_ptr;
}
