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


#ifndef ONEBUTTONDEBOUNCE_H
#define ONEBUTTONDEBOUNCE_H



#define button 0x03

//Global variables
volatile uint64_t startCnt;  //don't set these to 0 to ensure the compiler puts the variables in the .BSS section of the code (see Lib-c manual)
volatile uint64_t milliCtr;
uint8_t button_history;



//prototype functions
//void startmillis();
void start_oneButtonDebounce(void);
void update_button(uint8_t *button_history);
uint8_t read_button(uint8_t byte, uint8_t bit);
uint8_t is_button_pressed(uint8_t *button_history);
uint8_t is_button_released(uint8_t *button_history);
uint8_t is_button_down(uint8_t *button_history);
uint8_t is_button_up(uint8_t *button_history);
	
	
	
	
	
#endif /*ONEBUTTONDEBOUNCE_H*/
