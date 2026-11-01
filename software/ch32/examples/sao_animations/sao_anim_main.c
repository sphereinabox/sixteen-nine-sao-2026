#include "ch32fun.h"
#include "board.h"
#include <stdio.h>
#include "anim.h"
#include "is31.h"

void blink_error(int num)
{
	// blink LED num times
	Delay_Ms(500);
	for (int i = 0; i < num; i++)
	{
		// on
		funDigitalWrite(PIN_LED, FUN_LOW);
		Delay_Ms(250);
		// off
		funDigitalWrite(PIN_LED, FUN_HIGH);
		Delay_Ms(250);
	}
	Delay_Ms(500);
}

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

	// Set PIN_LED to output
	// the LED lights up when pin is pulled low
	funPinMode(PIN_LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

	// configure button pins as input pulled-up:
    funPinMode( PIN_BTN_A, GPIO_CFGLR_IN_PUPD );
    funDigitalWrite( PIN_BTN_A, 1 );
    funPinMode( PIN_BTN_B, GPIO_CFGLR_IN_PUPD );
    funDigitalWrite( PIN_BTN_B, 1 );

	if (is31_init() != 0)
	{
		printf("IS31 display init failed\n");
		// display init failed
		while (1)
			blink_error(2);
	}

	// TODO: setup animation
	bounce_init();
	conways_init();
	textscroll_init();
	plasma_init();

	int anim = 3;
	int anim_max = 4;
	int a_was_pressed = 0; // right button
	int b_was_pressed = 0; // left button

	// animation loop
	while (1)
	{
		// process input
        int a_pressed = (0 == funDigitalRead(PIN_BTN_A));
        int b_pressed = (0 == funDigitalRead(PIN_BTN_B));

		if (a_was_pressed && !a_pressed) {
			anim += 1;
			if (anim_max <= anim) {
				anim = 0;
			}
			// todo: anim init

			printf("button pressed, switched to anim %d\n", anim);
		}

		// TODO: advance animation
		switch (anim)
		{
		case 0:
			bounce_advance();
			break;
		case 1:
			conways_advance();
			break;
		case 2:
			textscroll_advance();
			break;
		case 3:
			plasma_advance();
			break;
		default:
			anim = 0;
			break;
		}
		// TODO: delay amount specified by animation
		Delay_Ms(20);

		a_was_pressed = a_pressed; // right button
		b_was_pressed = b_pressed; // left button
	}
}
