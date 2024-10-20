/*********************************************************************
 * N button debounce v3 - Version 3 uses typedef and structs to define the port the button is connected to
 * 
 * Created: 30/12/2023 
 * Author : Happymacer
 * 
 * Usage: 
 * Set up the button connections by adding the location of your buttons (port and pin no's) 
 * in the btn[] definition.   
 * 
 * Test status V3 - 4/5/24  - experimental
 *			   V3 - 5/5/24  - works
 *
 **********************************************************************/

//includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "BitManipulation.h"
#include "n_button_debounce_v3.h"

uint8_t btnSmplePeriod = 5; // ie sample the buttons every 5ms
typedef struct
{
	uint8_t terminal;   // the actual pin the button is connected to
	volatile uint8_t *inputPort;  // the port to be read for that button	
	volatile uint8_t *outputPort; // the port to write to if setting up internal pullup resistors
	volatile uint8_t *ddr;
} Buttons;

//	format is {pin number, input port number, output port number, data direction register of the port}
Buttons btn[n] = 
	{
	{0x04, (uint8_t*)0x29, (uint8_t*)0x2B, (uint8_t*)0x2A}, 
	{0x05, (uint8_t*)0x29, (uint8_t*)0x2B, (uint8_t*)0x2A}, 
	{0x06, (uint8_t*)0x29, (uint8_t*)0x2B, (uint8_t*)0x2A},
	{0x05, (uint8_t*)0x23, (uint8_t*)0x25, (uint8_t*)0x24} // this button is on PortB pin 5
	};
	// Add more buttons in the same way up to 8.  If more are needed then change the variable definitions too,
	// to 16 bit numbers
	

/**************************************************************  
*
* This is an example of how to set a register using its address (uses DDR as example).  This lot is just for info.
* 
* Note that the button_DDR array is defined as an array of pointers
* 
* Also note that the array cannot be a "const" type as then its location and value are immutable
* see https://www.youtube.com/watch?v=W8REqKlGzDY&list=PLNyfXcjhOAwOF-7S-ZoW2wuQ6Y-4hfjMR&index=2
* note the array is marked as volatile so the compiler doesn't discard it during compile.
*
* https://www.nongnu.org/avr-libc/user-manual/group__avr__sfr__notes.html - discusses the offset of 0x20 and some compiler directives
*
* volatile uint8_t *button_DDR[n] = {(uint8_t*)0x2A, (uint8_t*)0x2A, (uint8_t*)0x2A, (uint8_t*)0x24}; //port data direction register address - last one is port B, others port D
* #define n 4 //4 buttons are installed
*
* int main (void)
* {
*	// setting a single array member:
*	// *button_DDR[0] = *button_DDR[0] | (1<<5); //set bit D5 to output
*	
*	// setting the array
*	for (int i=0; i<n; i++)
*	{
*		*button_DDR[i] = *button_DDR[i] | (1<<i);
*	}
* }
*
***************************************************************/



//Interrupt handling routines
//Timer 0
//increment a global variable (milliCtr) once each time
ISR(TIMER0_COMPA_vect)
{
	if (milliCtr-startCnt >= btnSmplePeriod)
	{
		for (uint8_t i = 0; i < n; i++)
		{
			update_button(&button_history[i], btn[i].inputPort, btn[i].terminal);
		}
	}
	// consider what happens when it overflows...
	// when overflow is due at next interrupt instance then
	// reset starting counter
	if (milliCtr >= UINT64_MAX)  //UINT64_MAX should equal 0xffff ffff ffff ffff
	{
		startCnt = milliCtr;
	} else
	{
		milliCtr++;
	}
	
};




	
	void start_debounce()
	{
		// Timer 0 is free running and will count in 1 ms increments.  
		// The buttons will be scanned at each increment of the timer.
		// "millictr" is also incremented at each 1ms cycle
		
		//enable global interrupts
		sei();
		
		// The overflow interrupt is TIMER0_OVF_vect
		TIMSK0 |= (1<<1);  // set Timer/Counter0 Interrupt Mask Register - enable timer 0 output compare interrupt
		OCR0A = 0x7D; // at 8MHz clock and 0xFA at 16MHz lock // set Timer/Counter Register - counter start point for 1ms counts
		TCCR0A = 0x02; // set Timer/Counter Control Register A to "CTC mode"
		TCCR0B = 0x03; // set Timer/Counter Control Register B, 64 prescaler
		
		//for the button input pins, set the registers up.
		for (uint8_t i = 0; i<n; i++)
		{
			CLEAR_BIT(*btn[i].ddr, btn[i].terminal);  //clear bits to configure as input for buttons - should be 0 by default anyway but just in case.
			SET_BIT(*btn[i].outputPort, btn[i].terminal); //set bits to turn on pullup resistor
			//*btn[i].outputPort |= (1<<btn[i].terminal);
		}
		startCnt = milliCtr;
	}
	
	
	uint8_t read_button(volatile uint8_t *port, uint8_t bit)
	{
		if ((*port & (1<<bit)) == 0) return 1; // (1<<bit) is to set the mask to all zeros except the bit in question. Single & as we are performing a logical "and", not a comparison)
		else return 0;
	}
	

	void update_button(uint8_t *button_history, volatile uint8_t *button_port, uint8_t button_bit)
	{
		*button_history = *button_history << 1;
		*button_history |= read_button(button_port, button_bit);
	}
	
	//Button state detection routines
	uint8_t is_button_pressed(uint8_t *button_history)
	{
		return (*button_history == 0b00111111);
		//the source article looks for a button that might drop the signal for a few bits, and we look for the signal either side.  I don't want that so deleted the code.
	}
	
	
	uint8_t is_button_released(uint8_t *button_history)
	{
		return (*button_history == 0b11100000);
		//the source article looks for a button that might drop the signal for a few bits, and we look for the signal either side.  I don't want that so deleted the code.
	}


	uint8_t is_button_down(uint8_t *button_history)
	{
		return (*button_history == 0b11111111);
	}


	uint8_t is_button_up(uint8_t *button_history)
	{
		return (*button_history == 0b00000000);
	}




