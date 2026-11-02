/*
"Plasma" animation effect

Uses floating point numbers and slow emulated functions like sin()

sqrtf() wasn't working for me so I faked that out.

The floating point functions alone add 4k to flash size.
*/
#include "ch32fun.h"
#include <stdio.h>
#include <math.h>
#include "is31.h"

// use defines to make more meaningful names for our GPIO pins
#define PIN_LED PD0

float my_absf(float f) {
	if (f < 0) return -f;
	return f;
}

float my_sinf(float f) {
	return sinf(f);

	// triangle wave
	// f = my_absf(f/3.14f);
	// return fabs(floorf(f) - f - 0.5f)*2.0f - 1.0f; 

	// sawtooth wave
	// f = my_absf(f/3.14f);
	// return (floorf(f) - f)*2.0f - 1.0f;
}

void plasma()
{
	// copied/modified from LoLShield_Plasma example
	float phase = 0.0f;
	float phaseIncrement = 0.08f;
	float colorStretch = 0.11f;

	// framecount=0
	// next_framecount_update = 1 + time.monotonic()
	// while True:
	while (1)
	{
		// framecount += 1
		phase += phaseIncrement;

		float p1x = (my_sinf(phase * 1.000f) + 1.0f) * 7.5f;
		float p1y = (my_sinf(phase * 1.310f) + 1.0f) * 4.0f;
		float p2x = (my_sinf(phase * 1.770f) + 1.0f) * 7.5f;
		float p2y = (my_sinf(phase * 2.865f) + 1.0f) * 4.0f;

		for (int row = 0; row < 9; row++)
		{
			float row_f = row;
			for (int col = 0; col < 16; col++)
			{
				float col_f = col;

				// Calculate the distance between this LED, and p1.
				// The vector from p1 to this LED.
				float dist1x = my_absf(col_f - p1x);
				float dist1y = my_absf(row_f - p1y);
				// float distance = dist1x * dist1x + dist1y * dist1y;
				
				// Calculate the distance between this LED, and p2.
				// The vector from p2 to this LED.
				float dist2x = my_absf(col_f - p2x);
				float dist2y = my_absf(row_f - p2y);
				// Multiply this with the other distance, this will create weird plasma values :)
				// distance *= dist2x * dist2x + dist2y * dist2y;
				// distance = sqrtf(distance); // (.text.sqrtf+0x26): undefined reference to `__errno'

				// approximate distances sqrt(x*x+y*y) as (fabs(x)+y*y/2/fabs(x))
				float distance = (dist1x + dist1y * dist1y / 2 / dist1x) * 
					(dist2x + dist2y * dist2y / 2 / dist2x);

				// Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
				// You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
				float color_f = (my_sinf(distance * colorStretch) + 1.0f) * 0.5f; // range: 0.0...1.0

				// Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
				color_f *= color_f;
				// color_f *= color_f*color_f*color_f # Uncomment this line to make it even darker :)

				// is31_setpixel(col, row, (int)(color_f * 255)); // brighter
				is31_setpixel(col, row, (int)(color_f * 64.0f)); // dimmer
			}
		}

		is31_flip();
	}
}

/// main:

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

	funPinMode(PIN_LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);	   // Set PIN_LED to output

	Delay_Ms(100);
	printf("\r\r\n\ni2c test\n\r");

	Delay_Ms(100);

	if (!is31_init())
	{
		printf("is31 init okay!\r\n");

		plasma();
	}

	printf("Stuck here forever...\n\r");
	while (1)
		;
}
