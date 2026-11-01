/*
button test
light up LED when exactly one of the two buttons A or B are pressed.

PIN_LED lights LED when pulled low
PIN_BTN_A is the left button, is pulled to GND when pressed
PIN_BTN_B is the right button, is pulled to GND when pressed

*/

#include "ch32fun.h"
#include "board.h"
#include <stdio.h>

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

	funPinMode(PIN_LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);	   // Set PIN_LED to output
    
    // configure button pins as input pulled-up:
    funPinMode( PIN_BTN_A, GPIO_CFGLR_IN_PUPD );
    funDigitalWrite( PIN_BTN_A, 1 );
    funPinMode( PIN_BTN_B, GPIO_CFGLR_IN_PUPD );
    funDigitalWrite( PIN_BTN_B, 1 );

    printf("button test\n");
    printf("A pressed, B pressed\n");
	while(1)
	{
        int a_pressed = (0 == funDigitalRead( PIN_BTN_A ));
        int b_pressed = (0 == funDigitalRead(PIN_BTN_B));

        if (a_pressed != b_pressed) {
            // set LED on
            funDigitalWrite(PIN_LED, FUN_LOW);
        } else {
            // set LED off
            funDigitalWrite(PIN_LED, FUN_HIGH);
        }
        printf("%d, %d\n", a_pressed, b_pressed);
        Delay_Ms(100);
	}
}
