/*
 * button_debounce_v1.c
 * 
 * Created: 30/12/2023 6:43:16 PM
 * Author : Happymacer
 *
 * version 1 uses CTC method as this auto resets the counter to 0 on compare match and ONE button
 *
 * Test status - 21/1/24
 * this button debouncer has been tested on a Atmega328 with 8Mhz crystal and button on Port D pin 7
 * 
 * refer https://www.nongnu.org/avr-libc/user-manual/index.html and https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html
 *
 * Process - 
 * 1 - setup a regular counter for 1ms ticks (ie the equivalent to Arduino Millis())
 * 2 - This version uses Timer 0 with a count value of 125 and prescale is 64 assuming 8MHz clock.
 * 3 - Update the button in the ISR of the 1ms timer, so the button gets tested every 1ms
 * 4 - refer to button debounce algorithm https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/#more-180185 for details how the debounce works
 * 
 */
 
//includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "one_button_debounce_v1.h"
#include "BitManipulation.h"

//defines
#define LED 0x00
int delay = 1000;
int press_count = 0;
int release_count = 0;





	

int main(void)
{

	
	start_oneButtonDebounce();
	
	SET_BIT(DDRD,LED); //set bit D0 to output

	
	
    while (1) 
    {
		//if (milliCtr-startCnt >= delay) // count "delay" number of ms
		//{ 
			//startCnt = milliCtr;
			//
		//}	
		
		
	
		if (is_button_down(&button_history))
		{
			press_count++;
			TOGGLE_BIT(PORTD, LED); // toggle the LED
		}
		
		if (is_button_up(&button_history))
		{
			release_count++;
			//delay = 150;
		}				
    }
	
}

	
	
	

