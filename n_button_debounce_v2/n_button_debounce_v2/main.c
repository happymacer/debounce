/*
 * N button_debounce_v2 - this version uses arrays to define the ports to use. V3 uses structs.  This version is easier to follow.
 * 
 * Created: 30/12/2023 6:43:16 PM
 * Author : Happymacer
 *
 * version 1 - 21/1/24 - uses CTC method as this auto resets the counter to 0 on compare match and "n" buttons 
 * version 2 - 25/4/24 - rewrite to allow port specification for the switch and some variables renamed for easier understanding.
 *
 * Test status v1 - 21/1/24 - works
 *            v2 - 25/4/24 - experimental
 *			  V2 - 30/4/24 - works 
 * 
 * 
 * refer https://www.nongnu.org/avr-libc/user-manual/index.html and https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html
 *
 * Process - 
 * 1 - setup a regular counter for 1ms ticks (ie the equivalent to Arduino Millis())
 * 2 - This version uses Timer 0 with a count value of 125 (0x7D) and prescale is 64 assuming 8MHz clock
 * 3 - Update the button in the ISR of the 1ms timer, so the button gets tested every 1ms
 * 4 - refer to button debounce algorithm https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/#more-180185 for details how the debounce works
 * 
 *
 */
 
//includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

//defines
#define SET_BIT(byte, bit) (byte |= (1 << bit))
#define CLEAR_BIT(byte, bit) (byte &= ~(1 << bit))
#define TOGGLE_BIT(byte, bit) (byte ^= (1 << bit))
#define LED 0x00
#define MASK 0b11000111
#define n 4 //4 buttons are installed


//Global variables
uint8_t button_history[n];
volatile uint64_t milliCtr;
volatile uint64_t startCnt;  //dont set these to 0 to ensure the compiler puts the variables in the .BSS section of the code (see Lib-c manual)
int delay = 1000;
uint8_t btnSmplePeriod = 5; // ie sample the buttons every 5ms
	
// set up buttons here:
const uint8_t button_bit[n] = {0x04, 0x05, 0x06, 0x05}; //remember that the array starts at index 0, not 1
volatile uint8_t *button_PIN[n] = {(uint8_t*)0x29, (uint8_t*)0x29, (uint8_t*)0x29, (uint8_t*)0x23}; //port input pin register address - last one is port B, others port D
volatile uint8_t *button_PORT[n] = {(uint8_t*)0x2B, (uint8_t*)0x2B, (uint8_t*)0x2B, (uint8_t*)0x25}; //port output pin register address - last one is port B, others port D	
volatile uint8_t *button_DDR[n] = {(uint8_t*)0x2A, (uint8_t*)0x2A, (uint8_t*)0x2A, (uint8_t*)0x24}; //port data direction register address - last one is port B, others port D	





/*  
//This is an example of how to set a register using its address (uses DDR as example).  Note that the button_DDR array is defined as an array of pointers
// also note that the array cannot be a "const" type as then its location and value are immutable
// see https://www.youtube.com/watch?v=W8REqKlGzDY&list=PLNyfXcjhOAwOF-7S-ZoW2wuQ6Y-4hfjMR&index=2
// note the array is marked as volatile so the compiler doesn't discard it during compile.


volatile uint8_t *button_DDR[n] = {(uint8_t*)0x2A, (uint8_t*)0x2A, (uint8_t*)0x2A, (uint8_t*)0x24}; //port data direction register address - last one is port B, others port D
#define n 4 //4 buttons are installed

int main (void)
{
	// setting a single array member:
	// *button_DDR[0] = *button_DDR[0] | (1<<5); //set bit D5 to output
	
	// setting the array
	for (int i=0; i<n; i++)
	{
		*button_DDR[i] = *button_DDR[i] | (1<<i);
	}
}


*/



//prototype functions
void startmillis();
void update_button(uint8_t *button_history, volatile uint8_t *button_port, uint8_t button_bit); 
uint8_t read_button(volatile uint8_t *port, uint8_t bit); 
uint8_t is_button_pressed(uint8_t *button_history); 
uint8_t is_button_released(uint8_t *button_history); 
uint8_t is_button_down(uint8_t *button_history); 
uint8_t is_button_up(uint8_t *button_history);

//Interrupt handling routines
//Timer 0  
//increment a global variable (milliCtr) once each time 
ISR(TIMER0_COMPA_vect)
{
	if (milliCtr-startCnt >= btnSmplePeriod) 
	{
		for (uint8_t i = 0; i < n; i++)
		{
			update_button(&button_history[i], button_PIN[i], button_bit[i]);
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

	

int main(void)
{
	//uint8_t press_count = 0;
	//uint8_t release_count = 0;
	// start the Millis timer - on timer 0 counts 125, prescale 64, interrupt on compare overflow, clock speed 8MHz
	startmillis();
	//use the LED on PD0 to signal the millis count is working
	startCnt = milliCtr;
	SET_BIT(DDRD,LED); //set bit D0 to output
	
	//for the button input pins, set the registers up.
	for (uint8_t i = 0; i<n; i++)
	{
		CLEAR_BIT(*button_DDR[i], button_bit[i]);  //clear bits to configure as input for buttons - should be 0 by default anyway but just in case.
		SET_BIT(*button_PORT[i], button_bit[i]);   //set bits to turn on pullup resistor
	}

	
    while (1) 
    {
		//if (milliCtr-startCnt >= delay) // count "delay" number of ms
		//{ 
			//startCnt = milliCtr;
			//CLEAR_BIT(PORTD, LED); // LED off
		//}	
		
		
		
		
 
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
	
	
	// Start all subroutine definitions here
	
	
	void startmillis()
	{
		// Timer 0 is free running and will count in 1 ms increments.  The buttons will be scanned at each increment of the timer.
		// "millictr" is also incremented at each 1ms cycle
		//enable global interrupts
		sei();
		// The overflow interrupt is TIMER0_OVF_vect
		TIMSK0 |= (1<<1);  // set Timer/Counter0 Interrupt Mask Register - enable timer 0 output compare interrupt 	
		OCR0A = 0x7D; // at 8MHz clock and 0xFA at 16MHz lock // set Timer/Counter Register - ie the counter start point for 1ms counts
		TCCR0A = 0x02; // set Timer/Counter Control Register A to "CTC mode"
		TCCR0B = 0x03; // set Timer/Counter Control Register B, 64 prescaler 
		
	}
	
	
	uint8_t read_button(volatile uint8_t *port, uint8_t bit)
	{
		if ((*port & (1<<bit)) == 0) return 1; // (1<<bit) is to set the mask to all zeros except the bit in question. Single & as we are performing a logical "and", not a comparison
		else return 0;
	}
	

	void update_button(uint8_t *button_history, volatile uint8_t *button_port, uint8_t button_bit)
	{
		*button_history = *button_history << 1;
		*button_history |= read_button(button_port, button_bit);
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


