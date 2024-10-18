/*************************************************************************************************************
 * N button_debounce_v3 - Version 3 uses typedef and structs to define the port the button is connected to
 * 
 * Created: 30/12/2023 6:43:16 PM
 * Author : Happymacer
 *

 * Test status 
 *			  V3 - 4/5/24  - experimental
 *			  V3 - 5/5/24  - works
 * 
 * 
 * refer https://www.nongnu.org/avr-libc/user-manual/index.html and https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html
 * 
 * Notes - 
 * 1 - setup a regular counter for 1ms ticks (ie the equivalent to Arduino Millis())
 * 2 - This version uses Timer 0 with a count value of 125 (0x7D) and prescale is 64 assuming 8MHz clock.
 * 3 - Update the button in the ISR of the 1ms timer, so the button gets tested every 5ms
 * 4 - refer to button debounce algorithm https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/#more-180185 for details how the debounce works
 * 5 - In this code below, 3 buttons are port D and last button is on port B
 * 6 - Note that the routine uses interrupts
 *
 * To use these routines for debouncing switches set the number of switches (n) and what ports they are    
 * connected in the implementation "c" file.
 *
 *
 *
 *
 *
 ************************************************************************************************************/
 #ifndef NBUTTONDEBOUNCE_v3_H
 #define NBUTTONDEBOUNCE_v3_H

//defines
#define n 4 //4 buttons are installed


//Global variables
volatile uint64_t milliCtr;
volatile uint64_t startCnt;  //dont set these to 0 to ensure the compiler puts the variables in the .BSS section of the code (see Lib-c manual)
uint8_t button_history[n];



//prototype functions
void start_debounce(void);
void update_button(uint8_t *button_history, volatile uint8_t *button_port, uint8_t button_bit);
uint8_t read_button(volatile uint8_t *port, uint8_t bit);
uint8_t is_button_pressed(uint8_t *button_history);
uint8_t is_button_released(uint8_t *button_history);
uint8_t is_button_down(uint8_t *button_history);
uint8_t is_button_up(uint8_t *button_history);

#endif //NBUTTONDEBOUNCE_v3_H