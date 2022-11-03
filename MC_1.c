/*
 * MC_1.c
 *
 *  Created on: Jul 3, 2022
 *      Author: DELL
 */

#include "avr/io.h"
#include "lcd.h"
#include "uart.h"
#include "timer.h"
#include "keypad.h"
#include "util/delay.h" /* For the delay functions */

uint8 check_pass_code(uint8 *first,uint8 *second);
void buzzer(void);
void correct_pass_code(void);


/*global variable to save the entered pass code*/
uint8 g_key_pressed[5]={0,0,0,0,'\0'};
/*global variable to save the current pass code*/
uint8 g_current_passcode[5]={1,1,1,1,'\0'};


/*code to indicate the MCU 1 is ready to receive data */
#define MC1_READY '$'


int main(void)
{

	/* Initialize the TWI/I2C configuration structure */
	struct uart_conf uart_con = {9600,0x18,0x86};

	/* Initialize the UART Driver */
	UART_init(&uart_con);

	/*Initialize the LCD driver*/
	LCD_init();


	/*display on the LCD to enter the new pass code*/
	LCD_displayStringRowColumn(0,0," Enter PassCode");

	LCD_moveCursor(1,0);

	/*define the needed variables*/
	uint8 key=0;//take the pressed key value
	uint8 i=0;//used for looping
	uint8 check_pass_code_value=0;//to hold if the the user enter the right pass code
	uint8 try_count=0;//to hold the number of times the user enter wrong pass code

	/* Send MC2_READY byte to MC1 to ask it to send the string */
		UART_sendByte(MC1_READY);

		/* Receive the pass code that is saved in the EEPROM from MC1 through UART */
		UART_receiveString(g_current_passcode);

		/*i make this operation the last thing in the initialization to give time to the other
		 * MCU to finish his initializations and get the pass code from the EEPROM */

	while(1)
    {

		/*first entering the pass code*/


		/* if any switch pressed for more than 500 ms it counts more than one press */  

		/*i  work with the keypad inputs values as ASCII to help me in the tasting phase
		 * by tracking the code by the virtual terminal */

		key = KEYPAD_getPressedKey()+48; // get the pressed key number

		/*to be only entered when the user enter a number which mean he is entering the pass code*/
		if((key <= '9') && (key >= '0'))
		{
			LCD_displayCharacter('*');//display on LCD
			g_key_pressed[i]=key;//get the pressed number and save it
			i++;//increment to save the next element in the array
			key='A';//change the value of key to not to enter the if unless the key is pressed

			/*if the user entered four elements*/
			if(i==4)
			{

				i=0;// set i with  to start new loop

				/*check if the user enter the right pass code*/
				check_pass_code_value=check_pass_code(g_current_passcode,g_key_pressed);

				/*if the function return 1 this mean that the user entered the right pass code */

				if(check_pass_code_value==1)
				{
					correct_pass_code();//call right pass code function
					/*set the variable which is responsible for count the number of incorrect
					 * pass code trials with 0*/
					try_count=0;
				}

				/*if the function return 0 this mean that the user entered the incorrect pass code */

				if(check_pass_code_value==0)
				{
					/*if the user enter the pass code incorrect four 3 times the buzzer will
									 * be activate */
					if(try_count==2)
					{
						buzzer();//active the buzzer
						/*set the variable which is responsible for count the number of incorrect
						* pass code trials with 0*/
						try_count=0;
					}
					else
					{
						/*if it is not more than three times will write on screen "wrong Code"
						 * and increment the number of trials*/
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0," Wrong Code");
						try_count++;
						_delay_ms(500);
					}
				}
				/*display on screen Enter pass code*/
				check_pass_code_value='\0';
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0," Enter PassCode");
				LCD_moveCursor(1,0);
			}
		}

		_delay_ms(500); /* Press time */
    }
}


/*
 * function responsible to run when the user entered the right pass code which will ask the user if
 * he wants to open the door or he wants to change the pass code and then do it
 * */

void correct_pass_code(void)
{

	/*variable which will  take the order */
	uint8 order=0;
	/* display on LCD "if he wants to open the door press +
	 * and if he wants to change pass code press -
	 * */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"+ Open Door");
	LCD_displayStringRowColumn(1,0,"- ChangePassCode");

	/*the do while loop to ensure that the user only enters + or - */
	do
	{
		/*read the  pressed key*/
		order=KEYPAD_getPressedKey();

	}while((order!='+')&&(order!='-'));


	/* Press time */
	_delay_ms(500);

	/*if the user press + it means that he wants to open the door*/

	if(order=='+')
	{
		/*set the timer 1  in compare mode with 33 seconds*/
		static struct timer1_conf conf={0,32226,scale_1024,COMPARE_A};

		/*display The Door Openn on the LCD*/
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"The Door Open");

		/*send the order to the MCU to open the door*/
		UART_sendByte('1');

		/*active the timer 1 to act as delay with 33 seconds*/
		timer1_init(&conf);
		/*clear the interrupt bit */
		TIMSK&=~(1<<OCIE1A);
		/*track the interrupt flag */
		while(!(TIFR&(1<<OCF1A)));
		/*clear the flag*/
		TIFR|=(1<<OCF1A);
		/*deactivate the timer*/
		timer_timer1_deinit();
		return;


		/*if i userd 8Mhz freq. the code will be
		 * 1st: 58594 will be 65535 and then:
		 * for(int k=0;K<=3;K++)
		 * {
		 * 		 while(!(TIFR&(1<<OCF1A)));
		 *		 TIFR|=(1<<OCF1A);
		 *  }
		 * and then will put compare value = 61204 and
		 *
		 * while(!(TIFR&(1<<OCF1A)));
		 * TIFR|=(1<<OCF1A);
		 *
		 *   */


	}

	/*if the user press - it means that he wants to change the pass code*/


	if(order=='-')
	{
		/*new variable to take each press*/
		uint8 new='\0';

		/*display on the to enter the new pass code */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Enter New Code");
		LCD_moveCursor(1,0);



		/*two arrays to take the new pass code two times and check them if they are the same
		 * if not the user need to do the same process again*/
		uint8 new_pass[5]={1,1,1,1,'\0'};
		uint8 new_pass_2[5]={2,2,2,2,'\0'};


		uint8 i=0;  //variable for looping

		/*take the new code to the first time*/
		while(!(new_pass[i]=='\0'))
		{
			/*get the key press*/
			new = KEYPAD_getPressedKey()+48; /* get the pressed key number */

			/*check if the user only enter a number*/
			if((new <= '9') && (new >= '0'))
			{

				LCD_displayCharacter('*');//display on screen *
				new_pass[i]=new;//save the the number in the array
				i++;//increment the i variable
				/*change the value of the press variable to enter the condition only after new press*/
				new='A';
			}
			_delay_ms(500); /* Press time */

		}

		/*display on the to enter the new pass code again */
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Enter it again");
				LCD_moveCursor(1,0);

		i=0;//set the looping variable with 0
		/*take the new code to the second time*/
		while(new_pass_2[i]!='\0')
		{
			/*get the key press*/
			new = KEYPAD_getPressedKey()+48; /* get the pressed key number */

			/*check if the user only enter a number*/
			if((new <= '9') && (new >= '0'))
			{
				LCD_displayCharacter('*');//display on screen *
				new_pass_2[i]=new;//save the the number in the array
				i++;//increment the i variable
				/*change the value of the press variable to enter the condition only after new press*/
				new='A';
			}
			_delay_ms(500); /* Press time */

		}

		/*check if the two arrays are equal "that the user enter the new pass code two times
		 * correctly
		 * */

		if(check_pass_code(new_pass,new_pass_2))
		{
			/*display on the LCD that the pass code is changed*/
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"PassCode changed");
			_delay_ms(500);

			/*save the new pass code*/
			for(int j=0;j<4;j++)
			{
				g_current_passcode[j]=new_pass[j];
			}
			/*send the new pass code to the other MCU to save it in the EEPROM*/
			UART_sendByte('3'); //send the order which responsible to change the pass code
			_delay_ms(20);
			UART_sendString(g_current_passcode);//send the new pass code
			UART_sendByte('#');//send the end of string terminator
		}
		/*if the user enter two different codes display in LCD wrong pass code*/
		else
		{

			LCD_clearScreen();
			LCD_displayString("Wrong Pass Code");
			LCD_displayStringRowColumn(1, 0, "Try Again");
			_delay_ms(500);
		}

	}
}
/*
 * function responsible for activation the buzzer by sending order to
 * the other MCU to active the buzzer for 60 seconds
 * display ERROR on the LCD */
void buzzer(void)
{
	/*set the timer 1  in compare mode with 60 seconds*/
	static struct timer1_conf conf={0,58594,scale_1024,COMPARE_A};

	/*send the order to the MCU to active the buzzer*/
	UART_sendByte('2');
	/*display ERROR on the LCD*/
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"ERROR");

	/*active the timer 1 to act as delay with 60 seconds*/
	timer1_init(&conf);

	/*clear the interrupt bit */
	TIMSK&=~(1<<OCIE1A);

	/*track the interrupt flag */
	while(!(TIFR&(1<<OCF1A)));
	/*clear the flag*/
	TIFR|=(1<<OCF1A);
	/*deactivate the timer*/
	timer_timer1_deinit();



	/*if i userd 8Mhz freq. the code will be
	 * 1st: 58594 will be 65535 and then:
	 * for(int k=0;K<=7;K++)
	 * {
	 * 		 while(!(TIFR&(1<<OCF1A)));
	 *		 TIFR|=(1<<OCF1A);
	 *  }
	 * and then will put compare value = 9998 and
	 *
	 * while(!(TIFR&(1<<OCF1A)));
	 * TIFR|=(1<<OCF1A);
	 *
	 *   */

}
/*
 * function to compare 2 arrays for  the first four elements
 * and return 1 if both are equal and zero if are not equal
 * */

uint8 check_pass_code(uint8 *first,uint8 *second)
{
	for(int i=0;i<4;i++)
	{
		if(first[i]!=second[i])
		{
			return 0;
		}
	}
	return 1;
}
