# debounce
This is ATMEGA328 microcontroller debounce code for one or more switches or buttons.  Its based on a Hack-a-day algorythm - https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/#more-180185.  Its written for the ATMEGA328 but with minor adjustments it could be applied, I think, to any microcontroller.  The difference Id expect is in the port allocation and the register settings for the timer and the interupt vector/s.

I used Microchip/Atmel Studio 7 for this code development mainly as I like the Atmega328, and I have a little development board (example: https://www.aliexpress.com/item/1005001655965219.html?algo_exp_id=04cd05a0-25c5-434c-a6c9-19a92d8b56fe-13&utparam-url=scene%3Asearch%7Cquery_from%3A) for it so I can use it baremetal.  The board has a 8MHz crystal.  

I've prepared 3 options.  There is code for 1 button only, 2 buttons and "n" buttons, where "n" is the number of buttons attached to the chip.  The "n" button code has 3 versions - version 1 uses CTC mode for the timer and specifies the pins used for the buttons in an array and all on port D.  Version 2 allows the Port to be specified in arrays.  Version 3 uses structs to define the pins and ports used.

I've made libraries out of "N button v3" code as its the most generic and from the "One button" code as thats probably useful.  The others are just single files with the embedded code.  The way my code is written you have to add details of where the buttons are connected to the library ".c" file

Let me know if he code is useful to you!
