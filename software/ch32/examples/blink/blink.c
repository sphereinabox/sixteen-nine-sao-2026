#include "ch32fun.h"
#include "board.h"
#include <stdio.h>

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

	funPinMode(PIN_LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);	   // Set PIN_LED to output

	// blink demo:
	while(1)
	{
		printf("test\n");
		funDigitalWrite( PIN_LED,     FUN_HIGH ); // Turn on PIN_LED
		Delay_Ms( 1000 );
		funDigitalWrite( PIN_LED,     FUN_LOW );  // Turn off PIN_LED
		Delay_Ms( 1000 );
	}
}
