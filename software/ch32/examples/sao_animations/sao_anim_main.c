#include "ch32fun.h"
#include "board.h"
#include "i2c_slave.h"
#include <stdio.h>
#include "anim.h"
#include "is31.h"

volatile uint8_t i2c_registers[255] = {
	// see format from https://badge.team/docs/standards/sao/binary_descriptor/
	// header
	'L','I','F','E',
	// name length
	15,
	// driver name length
	14,
	// driver data length
	0,
	// num extra drivers
	0,
	// Name, no null terminator
	'S','i','x','t','e','e','n','N','i','n','e','C','h','3','2',
	// Driver name, no null terminator
	'S','i','x','t','e','e','n','9','C','h','3','2','V','1',
	// zeros for the rest for now
	0x00
};

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

volatile int isr_i2c_write_start = 0, isr_i2c_write_end = 0;

void i2c_onWrite(uint8_t reg, uint8_t length)
{
	// Called from the interrupt when an I2C write operation completes
	// Put one-off operations involving changes to i2c_registers here,
	// but you can also just read i2c_registers from main().

	if (length <= 0) {
		return;
	}
	// record the max start/end (inclusive) addresses
	
	int i2c_write_start = isr_i2c_write_start;
	int i2c_write_end = isr_i2c_write_end;
	if (reg < i2c_write_start) {
		i2c_write_start = reg;
	}
	if (isr_i2c_write_end < reg + length - 1) {
		i2c_write_end = reg + length - 1;
	}
	isr_i2c_write_start = i2c_write_start;
	isr_i2c_write_end = i2c_write_end;
}

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

	// Set PIN_LED to output
	// the LED lights up when pin is pulled low
	funPinMode(PIN_LED, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funDigitalWrite(PIN_LED, 1); // LED off

	// configure button pins as input pulled-up:
	funPinMode(PIN_BTN_A, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(PIN_BTN_A, 1);
	funPinMode(PIN_BTN_B, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(PIN_BTN_B, 1);

	// Check if i2c pins for is31 are jumpered to I2C pins for SAO
	// then abort animations so badge can control display directly.
	// First: configure i2c pins for IS31 as outputs
	funPinMode(IS31_PIN_SDA, GPIO_CFGLR_OUT_10Mhz_PP); // SDA
	funDigitalWrite(IS31_PIN_SDA, 1);
	funPinMode(IS31_PIN_SCL, GPIO_CFGLR_OUT_10Mhz_PP); // SCL
	funDigitalWrite(IS31_PIN_SCL, 1);
	// configure SAO i2c pins as inputs
	funPinMode(SAO_PIN_SDA, GPIO_CFGLR_IN_PUPD); // SDA
	funPinMode(SAO_PIN_SCL, GPIO_CFGLR_IN_PUPD); // SCL
	funDigitalWrite(SAO_PIN_SDA, 1);
	funDigitalWrite(SAO_PIN_SCL, 1);
	Delay_Us(1);
	if (funDigitalRead(SAO_PIN_SDA) == 0 ||
		funDigitalRead(SAO_PIN_SCL) == 0)
	{
		// well I'm not pulling them down, so they're not connected
		printf("SAO_PIN_SDA or SAO_PIN_SCL is low?\n");
	}
	else
	{
		// try IS31_PIN_SCL/SDA low
		funDigitalWrite(IS31_PIN_SCL, 0);
		funDigitalWrite(IS31_PIN_SDA, 0);
		Delay_Ms(1);
		if (funDigitalRead(SAO_PIN_SCL) == 0 ||
			funDigitalRead(SAO_PIN_SCL) == 0)
		{
			funDigitalWrite(IS31_PIN_SCL, 1);
			funDigitalWrite(IS31_PIN_SDA, 1);
			// configure bitbang i2c outputs as floating inputs:
			funPinMode(IS31_PIN_SDA, GPIO_CFGLR_IN_FLOAT); // SDA
			funPinMode(IS31_PIN_SCL, GPIO_CFGLR_IN_FLOAT); // SCL

			blink_error(2);

			while (true)
			{
				printf("Powered up and IS31_PIN_SCL/SDA to SAO_PIN_SCL/SDA\n");
				printf("Skipping animations so badge can control display directly...\n");
				Delay_Ms(1000);
			}
		}
	}

	// Initialize I2C slave pins for SAO:
	funPinMode(SAO_PIN_SDA, GPIO_CFGLR_OUT_10Mhz_AF_OD); // SDA
	funPinMode(SAO_PIN_SCL, GPIO_CFGLR_OUT_10Mhz_AF_OD); // SCL
	SetupI2CSlave(SAO_I2C_ADDRESS, i2c_registers, sizeof(i2c_registers), i2c_onWrite, NULL, false);

	// Initalize IS31FL3731 LED matrix:
	if (is31_init() != 0)
	{
		printf("IS31 display init failed\n");
		// display init failed
		while (1)
			blink_error(3);
	}

	// set i2c bus defaults:
	int anim = 1;
	i2c_registers[SAO_REG_ANIMATION_CHOICE] = 1;
	anim = 1;

	// one larger than highest animation number:
	const int anim_max = 5;

	// TODO: setup animation
	bounce_init();
	conways_init();
	textscroll_init();
	plasma_init();

	int a_was_pressed = 0; // right button
	int b_was_pressed = 0; // left button

	// animation loop
	while (1)
	{
		// respond to i2c changes
		funDigitalWrite(PIN_LED, (i2c_registers[SAO_REG_LED] & 1) == 1 ? 0 : 1);

		int i2c_write_start = isr_i2c_write_start;
		int i2c_write_end = isr_i2c_write_end;
		isr_i2c_write_start = 0;
		isr_i2c_write_end = 0;

		if ((i2c_write_start != 0) && (i2c_write_end != 0)) {
			printf("i2c regs written %d through %d\n", i2c_write_start, i2c_write_end);
		}

		if (i2c_write_start <= SAO_REG_ANIMATION_CHOICE &&
			SAO_REG_ANIMATION_CHOICE <= i2c_write_end) {
			anim = i2c_registers[SAO_REG_ANIMATION_CHOICE];
		}

		// process input
		int a_pressed = (0 == funDigitalRead(PIN_BTN_A));
		int b_pressed = (0 == funDigitalRead(PIN_BTN_B));

		uint8_t button_bits = 0;

		if (a_was_pressed && !a_pressed)
		{
			anim += 1;
			if (anim_max <= anim)
			{
				anim = 1;
			}
			// todo: anim init

			printf("button pressed, switched to anim %d\n", anim);
		}

		if (b_was_pressed && !b_pressed)
		{
			anim -= 1;
			if (anim <= 0)
			{
				anim = anim_max-1;
			}
			// todo: anim init

			printf("button pressed, switched to anim %d\n", anim);
		}

		if (a_pressed)
			button_bits = button_bits | 0x1;
		if (b_pressed)
			button_bits = button_bits | 0x2;
		i2c_registers[SAO_REG_BUTTON_BITS] = button_bits;

		// TODO: advance animation
		switch (anim)
		{
		case 0:
			// static
			// TODO: draw from i2c buffer
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
		case 4:
			bounce_advance();
			break;
		default:
			anim = 1;
			break;
		}
		// TODO: delay amount specified by animation
		Delay_Ms(20);

		a_was_pressed = a_pressed; // right button
		b_was_pressed = b_pressed; // left button
	}
}
