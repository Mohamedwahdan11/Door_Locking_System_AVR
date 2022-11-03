/*
 * MC_2_App.c
 *
 *  Created on: Jun 26, 2022
 *      Author: DELL
 */

#include "dc_motor.h"
#include "timer.h"
#include "buzzer.h"
#include "twi.h"
#include "external_eeprom.h"
#include "uart.h"
#include "avr/io.h"
#include "util/delay.h"
#include "gpio.h"
#include "avr/sleep.h"

void recive_uart_data(void);
void update_passcode_eeprom(void);
void activate_buzzer(void);
void open_door(void);
void recive_passcode(void);


/*global variable to save the current pass code*/
uint8 g_key[5]={'1','1','1','1','\0'};
/*global variable to indicate if this is the first time the door is being used or it is used after
 * reset or the power is shut down as it's responsible is if the it is the first time to use the
 * the door set pass code 0000 if not read the current saved pass code in the EEPROM*/
uint8 g_key_check=0;


/*/*global variable to read the order send from the other MCU*/
uint8 data=0;

/*code to indicate the MCU 1 is ready to receive data */
#define MC1_READY '$'


void main(void)
{
	/* Enable Global Interrupt I-Bit */
		SREG |= (1<<7);

	/*
	 * Initialize the TWI/I2C configuration structure
	 * select the TWBR value , the prescale value & the address
	 * */
	struct twi_config twi_con ={ 1,pre_one,2};

	/* Initialize the TWI/I2C configuration structure */
	struct uart_conf uart_con = {9600,0x98,0x86};

	/* Initialize the UART Driver */
	UART_init(&uart_con);

	UCSRB&=~(1<<RXCIE);// to turn off the RX interrupt

	/* Set the Call back function pointer in the UART driver */
	UART_setCallBack_RX(recive_uart_data);

		/* Initialize the TWI/I2C Driver */
		TWI_init(& twi_con);

		/* Initialize the DC_motor Driver */
		DcMotor_Init();

		/* Initialize the Buzzer Driver */
		buzzer_Init();




		/* check if it is the  first use of the door or not
		 * as after the system is reset to get the saved pass code from the EEPROM */
	EEPROM_readByte(0x0301, &g_key_check);
	_delay_ms(10);/* Read 0x0301 from the external EEPROM */

	/*if it is not the first use then update the current pass code
	 * to be send to the other MCU
	 * */

	if(g_key_check==1)
	{
		EEPROM_readByte(0x0302, &g_key[0]);
		_delay_ms(10);
		EEPROM_readByte(0x0303, &g_key[1]);
		_delay_ms(10);
		EEPROM_readByte(0x0304, &g_key[2]);
		_delay_ms(10);
		EEPROM_readByte(0x0305, &g_key[3]);
		_delay_ms(10);
	}

	/*
	 * send the current pass code to the other MCU
	 * */

	/* Wait until MC2 is ready to receive the string */



	while(UART_recieveByte() != MC1_READY){}

	UART_sendString(g_key);
	UART_sendByte('#');

	UCSRB|=(1<<RXCIE);



	while(1)
	{

		/*this code is from the Internet*/

		// controller sleep mode
		    set_sleep_mode(SLEEP_MODE_IDLE); // sleep mode selected
		    sleep_enable();  // set SE bit
		    sleep_cpu(); // sleep mode activated

	}





	}


/*
 * function to be called by the UART ISR to know what is the order by reading the
 * sent data
 * */
void recive_uart_data(void)
{

	data=UART_recieveByte(); //read the order number

	/*
	 * if conditions to chose what to do if the sent number is:
	 * 1: then the entered pass code is correct and open the door
	 * 2: then the entered pass code is incorrect more than 3 times and active the buzzer
	 * 3: the user wants to change the pass code and receive the new pass code and save it
	 * other do nothing
	 * */

	if(data=='1')
	{
		open_door();
		data=0;
	}
	else if(data=='2')
	{
		activate_buzzer();
		data=0;
	}
	else if(data=='3')
	{
		recive_passcode();
		data=0;
	}
	else
	{
		data=0;
	}
}

/*
 * function is used to send the current pass code to other MCU
 * the function sent the string using polling technique
 * */
void send_passcode(void)
{
	UART_sendString(g_key);
}

/*
 * function is used to receive the new pass code to other MCU
 * the function receive the string using polling technique
 * */

void recive_passcode(void)
{
	UCSRB&=~(1<<RXCIE);         // disable the receiver interrupt to use polling technique
	UART_receiveString(g_key);  // call the receive string to get the new code
	UCSRB|=(1<<RXCIE); 		    // enable the receiver interrupt again
	update_passcode_eeprom(); 	//send the new code to the eeprom
}


/*
 * function used to open the door
 * */
void open_door(void)
{


	/*
	 * the timer configuration is defined her to make it not as global variable
	 * to make this configuration local to this function, and we can solve it by change
	 * the structure parameters every time it is called but this will affect the
	 * the code size and the time response and that is why i define it as static variable.
	 *     */
	static struct timer1_conf conf={0,15000,scale_1024,COMPARE_A};

	/*
	 * set the call back function of the UART RX ISR with the required function to be
	 * called
	 *   */
	timer_setCallBack_timer1(open_door);

	/*
	 * define a static variable and initialize it with 0 to use it with if condition
	 * to control what to happen every time the timer compare value rise a flag and generate an
	 * interrupt
	 * */
	static uint8 teck=0;

	/*
	 * 1st to open the door in 15 seconds
	 * */
	if(teck==0)
	{
		DcMotor_Rotate(CLK_WISE,50);  // rotate the DC motor clk wise with speed 50%
		conf.comp_value=14648;		  // set the compare value with the required counts to get 15 seconds
		timer1_init(&conf);			  // start the timer
		teck=1;						  // increment the static variable to control the next move
		return;						  // to break the function
	}

	/*
	* 2nd to stop the door for 3 seconds
	* */

	if(teck==1)
	{
		DcMotor_Rotate(OFF,0); // stop the DC motor
		conf.comp_value=2929;  // set the compare value with the required counts to get 3 seconds
		timer1_init(&conf);    // start the timer
		teck=2;				   // increment the static variable to control the next move
		return;  			   // to break the function

	}

	/*
	 * 3rd to close the door in 15 seconds
	 * */

	if(teck==2)
	{
	DcMotor_Rotate(ANTI_CLK_WISE,50); // rotate the DC motor clk wise with speed 50%
		conf.comp_value=14648;        // set the compare value with the required counts to get 15 seconds
		timer1_init(&conf);			  // start the timer
		teck=3;						  // increment the static variable to control the next move
		return;                       // to break the function

	}

	/*
	 * stop the door and the timer
	 * */
	if(teck==3)
	{
		DcMotor_Rotate(OFF,0); // stop the DC motor
		timer_timer1_deinit(); // stop the timer
		teck=0;          	   // set the static variable with 0
	}
}

/*if i userd 8Mhz freq. the code will be
 * 1st: 14648 will be 65535 and then and in tech=1 i will put compare value = 51651 and
 * 		shift the other after it ticks tecks.
 * 2nd: 2929 will be 23437
 * 3rd: the other 14648 will be same as first  */







/*function to activate the buzzer with one second*/
void activate_buzzer(void)
{


	/*
	 * the timer configuration is defined her to make it not as global variable
	 * to make this configuration local to this function, and we can solve it by change
	 * the structure parameters every time it is called but this will affect the
	 * the code size and the time response and that is why i define it as static variable.
	 *     */

	static struct timer1_conf conf={0,58593,scale_1024,COMPARE_A};

	/*
	 * set the call back function of the UART RX ISR with the required function to be
	 * called
	 *   */
	timer_setCallBack_timer1(activate_buzzer);

	/*
		 * define a static variable and initialize it with 0 to use it with if condition
		 * to control what to happen every time the timer compare value rise a flag and generate an
		 * interrupt
		 * */
	static uint8 teck=0;

	/*
	 * 1st active the buzzer for 3 seconds
	 * */
	if(teck==0)
	{
		buzzer_on();         // active the buzzer
		timer1_init(&conf);  // start the timer with1 minute
		teck=1;              // increment the static variable to control the next move
		return;				 // // to break the function
	}

	/*
	 * stop the buzzer and the timer
	 * */
	if(teck==1)
	{

		buzzer_stop();         // stop the buzzer
		timer_timer1_deinit(); // stop the timer
	}
}


/*if i userd 8Mhz freq. the code will be
 * 1st: 58593 will be 65535 and then and the first if will be at tech=7, rhen will put
 * compare value = 9998 and shift the other after it ticks tecks.
 *   */




/*function used to write the new pass code in the EEPROM*/
void update_passcode_eeprom(void)
{
		EEPROM_writeByte(0x0302, g_key[0]); // write the first code
		_delay_ms(10);
		EEPROM_writeByte(0x0303, g_key[1]);//write the second code
		_delay_ms(10);
		EEPROM_writeByte(0x0304, g_key[2]);//write the third code
		_delay_ms(10);
		EEPROM_writeByte(0x0305, g_key[3]);//write the fourth code
		_delay_ms(10);


		/* set value with 1 that indicates that the pass code
		 * has changed after the first use
		 * */

		EEPROM_writeByte(0x0301, 1);
		_delay_ms(10);

}





