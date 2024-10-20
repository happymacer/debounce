/*
 * TWO button_debounce_v1.c
 * 
 * Created: 30/12/2023 
 * Author : Happymacer
 *
 * This version 1 uses CTC method as this auto resets the counter to 0 on compare match and at least 2 buttons
 *
 * This file has all the code in this file.   
 * 
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

//defines
#define SET_BIT(byte, bit) (byte |= (1 << bit))
#define CLEAR_BIT(byte, bit) (byte &= ~(1 << bit))
#define TOGGLE_BIT(byte, bit) (byte ^= (1 << bit))
#define button1 0x07
#define button2 0x06
#define LED 0x00
#define MASK 0b11000111

//Global variables
uint8_t button1_history;
uint8_t button2_history;
volatile uint64_t milliCtr;
volatile uint64_t startCnt;  //dont set these to 0 to ensure the compiler puts the variables in the .BSS section of the code (see Lib-c manual)
int delay = 1000;


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
	update_button(&button1_history, PIND, button1);
	update_button(&button2_history, PIND, button2);
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
	CLEAR_BIT(DDRD, button1);  //clear bit D7 to configure as input for button1
	CLEAR_BIT(DDRD, button2);  //clear bit D6 to configure as input for button2
	SET_BIT(PORTD, button1); //set bit D7 to turn on pullup resistor
	SET_BIT(PORTD, button2); //set bit D6 to turn on pullup resistor
	
    while (1) 
    {
		if (milliCtr-startCnt >= delay) // count "delay" number of ms
		{ 
			startCnt = milliCtr;
			TOGGLE_BIT(PORTD, LED); // toggle the LED
		}	
		
		//read the button
	
		if (is_button_down(&button2_history))
		{
			press_count++;
			delay = 50;
		}
		
		if (is_button_up(&button2_history))
		{
			release_count++;
			//delay = 150;
		}				
    }
	
}
	
	
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


