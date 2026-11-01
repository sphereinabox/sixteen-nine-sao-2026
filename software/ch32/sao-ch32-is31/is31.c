#include "ch32fun.h"

#include <stdio.h>
#include <string.h>
#include "is31.h"
#include "board.h"

// use bitbang i2c because I want to leave i2c 
// hardware peripheral available for SAO comms
#define DELAY1 Delay_Us(1);
#define DELAY2 Delay_Us(2);
#define DSCL_IHIGH  { funPinMode( IS31_PIN_SCL, GPIO_CFGLR_IN_PUPD ); funDigitalWrite( IS31_PIN_SCL, 1 ); } 
#define DSDA_IHIGH  { funPinMode( IS31_PIN_SDA, GPIO_CFGLR_IN_PUPD ); funDigitalWrite( IS31_PIN_SDA, 1 ); } 
#define DSDA_INPUT  { funPinMode( IS31_PIN_SDA, GPIO_CFGLR_IN_PUPD ); funDigitalWrite( IS31_PIN_SDA, 1 ); } 
#define DSCL_OUTPUT { funDigitalWrite( IS31_PIN_SCL, 0 ); funPinMode( IS31_PIN_SCL, GPIO_CFGLR_OUT_2Mhz_PP );  } 
#define DSDA_OUTPUT { funDigitalWrite( IS31_PIN_SDA, 0 ); funPinMode( IS31_PIN_SDA, GPIO_CFGLR_OUT_2Mhz_PP );  } 
#define READ_DSDA    funDigitalRead( IS31_PIN_SDA )
#define I2CNEEDGETBYTE 0
#define I2CNEEDSCAN    0

#include "static_i2c.h"

void is31_i2c_setup(void)
{
	ConfigI2C();
}

int i2c_periph_send(
	uint8_t i2c_addr,
	const uint8_t *data,
	int sz)
{
	SendStart();
	int r = SendByte(i2c_addr<<1);
	if (r) return r;
	for (int i=0; i < sz; i++) {
		r = SendByte(data[i]);
		if (r) return 1;
	}
	SendStop();
	return 0;
}

//////////////// IS31 stuff //////////

#define IS31_ADDR 0x74

const uint8_t is31_config_bytes[] = {
	// destination address
	0x00,
	// 0x00: configuration register
	0b00000000,
	// 0x01 picture display register
	0b00000000,
	// 0x02 auto play control
	0b00000000,
	// 0x03 auto play control 2
	0b0000000,
	// 0x04 reserved (no function)
	0x00,
	// 0x05 display option
	0b00000000,
	// 0x06 audio synchronization register
	0b00000000,
	// 0x07 frame state
	0b00000000,
	// 0x08 breath control
	0b00000000,
	// 0x09 breath control 2
	0b00000000,
	// 0x0A shutdown, normal operation
	0b00000001,
	// 0x0B AGC control
	0b00000000,
	// 0x0C audio ADC rate
	0b00000000};

uint8_t is31_intensity_buf[145];
// todo: uint8_t is31_page_next;

int is31_init()
{
	is31_i2c_setup();

	uint8_t smallbuf[2];
	int retcode;

	// printf("set page to config bank\r\n");
	// set page to config bank
	smallbuf[0] = 0xFD;
	smallbuf[1] = 0x0B;
	retcode = i2c_periph_send(IS31_ADDR, smallbuf, 2);
	if (retcode)
		return retcode;

	// printf("write config\r\n");
	// write config
	retcode = i2c_periph_send(IS31_ADDR, is31_config_bytes, sizeof(is31_config_bytes) / sizeof(is31_config_bytes[0]));
	if (retcode)
		return retcode;

	// reset pages:
	for (uint8_t page = 8; page > 0;)
	{
		page--;
		// printf("set page\r\n");
		// set page
		smallbuf[0] = 0xFD;
		smallbuf[1] = page;
		retcode = i2c_periph_send(IS31_ADDR, smallbuf, 2);
		if (retcode)
			return retcode;

		// printf("enable outputs\r\n");
		// enable outputs
		memset(is31_intensity_buf, 0xFF, 19);
		is31_intensity_buf[0] = 0x00;
		retcode = i2c_periph_send(IS31_ADDR, is31_intensity_buf, 19);
		if (retcode)
			return retcode;

		// printf("set per-pixel intensity\r\n");
		// set per-pixel intensity everywhere to zero
		is31_clear();
		is31_intensity_buf[0] = 0x24;
		retcode = i2c_periph_send(IS31_ADDR, is31_intensity_buf, 145);
		if (retcode)
			return retcode;
	}

	return 0;
}

void is31_clear()
{
	memset(is31_intensity_buf+1, 0x00, 144);
}

#define IS31_SWAP_ROW_COL 1

void is31_setpixel(uint8_t col, uint8_t row, uint8_t intensity)
{
#if IS31_DISPLAY_LAYOUT == 1
	(is31_intensity_buf+1)[16 * row + col] = intensity;
#elif IS31_DISPLAY_LAYOUT == 2
	// try to deal with charlieplex bonnet having re-arranged rows/columns
	// the first row in the buffer is actually the first and 10th columns
	if (row == 8)
		return;
	if (col < 8) {
		(is31_intensity_buf+1)[16 * (col) + (8-row) + 15] = intensity;
	} else {
		(is31_intensity_buf+1)[16 * (col-8) + (8-row) + 15+8] = intensity;
	}
#else
#error unknown/invalid IS31_DISPLAY_LAYOUT define
#endif
}

uint8_t is31_getpixel(uint8_t col, uint8_t row)
{
#if IS31_DISPLAY_LAYOUT == 1
	return (is31_intensity_buf+1)[16 * row + col];
#elif IS31_DISPLAY_LAYOUT == 2
	if (row == 8)
		return 0;
	if (col < 8) {
		return (is31_intensity_buf+1)[16 * (col) + (8-row) + 15];
	} else {
		return (is31_intensity_buf+1)[16 * (col-8) + (8-row) + 15+8];
	}
#else
#error unknown/invalid IS31_DISPLAY_LAYOUT define
#endif
}

int is31_flip()
{
	return i2c_periph_send(IS31_ADDR, is31_intensity_buf, 145);
}
