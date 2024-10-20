/*
 * N button debounce v3 - Version 3 uses typedef and structs to define the port the button is connected to
 * 
 * Created: 30/12/2023 
 * Author : Happymacer
 *
 * 
 * Test status V3 - 4/5/24  - experimental
 *			   V3 - 5/5/24  - works
 */


#define LED 0x00  //the LED is on port D0



//includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "BitManipulation.h"
#include "n_button_debounce_v3.h"
//int delay = 1000;

int main(void)
{
	// start the Millis timer - on timer 0 counts 125, prescale 64, interrupt on compare overflow
	start_debounce();
	
	//use the LED on PD0 to signal the millis count is working
	SET_BIT(DDRD,LED); //set Port D0 to output
	CLEAR_BIT(PORTD,LED); //turn the LED off
	

    while (1) 
    {

		// test the buttons
		
		//------------------- Button 0
			
		if (is_button_down(&button_history[0]))
		{
			SET_BIT(PORTD,LED);
		}
		if (is_button_up(&button_history[0]))
		{
			CLEAR_BIT(PORTD,LED);
		}
		
		//------------------- Button 1
				
		if (is_button_down(&button_history[1]))
		{
			SET_BIT(PORTD,LED);
		}
		if (is_button_up(&button_history[1]))
		{
			CLEAR_BIT(PORTD,LED);
		}
		
		//------------------- Button 2
				
		if (is_button_down(&button_history[2]))
		{
			SET_BIT(PORTD,LED);
		}
		if (is_button_up(&button_history[2]))
		{
			CLEAR_BIT(PORTD,LED);
		}
		
		//------------------- Button 3
		
		if (is_button_down(&button_history[3]))
		{
			SET_BIT(PORTD,LED);
		}
		if (is_button_up(&button_history[3]))
		{
			CLEAR_BIT(PORTD,LED);
		}			
    }
} //end of main.c
	
	

	
	
