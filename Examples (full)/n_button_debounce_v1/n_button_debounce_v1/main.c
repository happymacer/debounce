/*
 * N button_debounce_v1 - this version only allows buttons on port D (as example) - 
 * V2 version uses arrays to define the ports to use if not the same and V3 uses structs.
 * 
 * Created: 30/12/2023 6:43:16 PM
 * Author : Happymacer
 *
 * version 1 - 21/1/24 - uses CTC method as this auto resets the counter to 0 on compare match and "n" buttons
 *
 * Test status - 21/1/24 OK
 * 
 * 
 * refer https://www.nongnu.org/avr-libc/user-manual/index.html and https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html
 *
 * Process - 
 * 1 - setup a regular counter for 1ms ticks (ie the equivalent to Arduino Millis())
 * 2 - This version uses Timer 0 with a count value of 125 and prescale is 64 assuming 8MHz clock.
 * 3 - Update the button in the ISR of the 1ms timer, so the button gets tested every 1ms
 * 4 - refer to button debounce algorithm https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/#more-180185 for details how the debounce works
 * 5 - assume all buttons on same port (Port D)
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
#define n 4 //4 buttons


//Global variables
uint8_t button_history[n];
volatile uint64_t milliCtr;
volatile uint64_t startCnt;  //dont set these to 0 to ensure the compiler puts the variables in the .BSS section of the code (see Lib-c manual)
int delay = 1000;
const uint8_t button[n] = {0x07, 0x06, 0x05, 0x04}; //remember that the array starts at index 0, not 1  All buttons on same port.
	

//prototype functions
void startmillis();
void update_button(uint8_t *button_history, uint8_t button_port, uint8_t button_bit); 
uint8_t read_button(uint8_t byte, uint8_t bit); 
uint8_t is_button_pressed(uint8_t *button_history); 
uint8_t is_button_released(uint8_t *button_history); 
uint8_t is_button_down(uint8_t *button_history); 
uint8_t is_button_up(uint8_t *button_history);

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
	
	for (uint8_t i=0; i < n; i++)
	{
		update_button(&button_history[i], PIND, button[i]); // used port D for buttons
	}
	

}; 

	

int main(void)
{
	uint8_t press_count = 0;
	uint8_t release_count = 0;
	// start the Millis timer - on timer 0 counts 125, prescale 64, interrupt on compare overflow
	startmillis();
	//use the LED on PD0 to signal the millis count is working
	startCnt = milliCtr;
	SET_BIT(DDRD,LED); //set bit D0 to output
	for (uint8_t i=0; i<n; i++)
	{
		CLEAR_BIT(DDRD, button[i]);  //clear bits to configure as input for buttons on port D 
		SET_BIT(PORTD, button[i]); //set bits to turn on pullup resistor on port D
	}

	
    while (1) 
    {
		if (milliCtr-startCnt >= delay) // count "delay" number of ms
		{ 
			startCnt = milliCtr;
			TOGGLE_BIT(PORTD, LED); // toggle the LED
		}	
		
		//read the button
	
		if (is_button_down(&button_history[3]))
		{
			//press_count++;
			delay = 20;
		}
		if (is_button_down(&button_history[2]))
		{
			//press_count++;
			delay = 150;
		}
		
		if (is_button_down(&button_history[1]))
		{
			//release_count++;
			delay = 500;
		}
		if (is_button_down(&button_history[0]))
		{
			//release_count++;
			delay = 1000;
		}				
    }
	
} //end of main.c
	
	
	// Start all subroutine definitions here
	
	
	void startmillis()
	{
		//enable global interrupts
		sei();
		// The overflow interrupt is TIMER0_OVF_vect
		TIMSK0 |= (1<<1);  // set Timer/Counter0 Interrupt Mask Register - enable timer 0 output compare interrupt 	
		OCR0A = 0xFA; // set Timer/Counter Register - counter start point 
		TCCR0A = 0x02; // set Timer/Counter Control Register A to "CTC mode"
		TCCR0B = 0x03; // set Timer/Counter Control Register B, 64 prescaler 
		
	}
	
	
	uint8_t read_button(uint8_t port, uint8_t bit)
	{
		if ((port & (1<<bit)) == 0) return 1; // (1<<bit) is to set the mask to all zeros except the bit in question. Single & as we are performing a logical "and", not a comparison)
		else return 0;
	}
	

	void update_button(uint8_t *button_history, uint8_t button_port, uint8_t button_bit)
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


