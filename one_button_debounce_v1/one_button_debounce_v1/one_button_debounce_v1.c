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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "one_button_debounce_v1.h"
#include "BitManipulation.h"

#define MASK 0b11001111



//local variables





//Interrupt handling routines
//Timer 0
//increment a global variable (milliCtr) once each time
ISR(TIMER0_COMPA_vect)
{
	// consider what happens when it overflows...
	// when overflow is due at next interrupt instance then
	// reset starting counter
	if (milliCtr >= UINT64_MAX)  //UINT64_MAX should equal 0xffff ffff ffff ffff
	{
		startCnt = milliCtr;
	}
	milliCtr++;
	update_button(&button_history);
};






void start_oneButtonDebounce(void)
{
	// start the Millis timer - on timer 0 counts 125, prescale 64, interrupt on compare overflow
	//enable global interrupts
	sei();
	// The overflow interrupt is TIMER0_OVF_vect
	TIMSK0 |= (1<<1);  // set Timer/Counter0 Interrupt Mask Register - enable timer 0 output compare interrupt
	OCR0A = 0xFA; // set Timer/Counter Register - counter start point
	TCCR0A = 0x02; // set Timer/Counter Control Register A to "CTC mode"
	TCCR0B = 0x03; // set Timer/Counter Control Register B, 64 prescaler
	startCnt = milliCtr;
	CLEAR_BIT(DDRD, button);  //clear bit of switch to configure as input for button
	SET_BIT(PORTD, button); //set bit of switch to turn on pullup resistor
}

//read the button
uint8_t read_button(uint8_t byte, uint8_t bit)
{
	if ((byte & (1<<bit)) == 0) return 1; // (1<<bit) is to set the mask to all zeros except the bit in question. Single & as we are performing a logical "and", not a comparison)
	else return 0;
}

//update the button history
void update_button(uint8_t *button_history)
{
	*button_history = *button_history << 1;
	*button_history |= read_button(PIND, button);
}


uint8_t is_button_pressed(uint8_t *button_history)
{
	return (*button_history == 0b01111111);
}

uint8_t is_button_released(uint8_t *button_history)
{
	uint8_t released = 0;
	if ((*button_history & MASK) == 0b11000000)
	{
		released = 1;
		*button_history = 0b00000000;
	}
	return released;
}

uint8_t is_button_down(uint8_t *button_history)
{
	return (*button_history == 0b11111111);
}


uint8_t is_button_up(uint8_t *button_history)
{
	return (*button_history == 0b00000000);
}
