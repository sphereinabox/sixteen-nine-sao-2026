#ifndef _IS31_H
#define _IS31_H

#include <stdint.h>

// init is31 device, turn it on, clear display
int is31_init();

// set pixel buffer to off everywhere
void is31_clear();

// set single pixel to specified intensity 0..255
// 9 rows in range 0..8
// 16 col is in range 0..15
void is31_setpixel(uint8_t col, uint8_t row, uint8_t intensity);

// set buffered intensity for single pixel, is in range 0..255
// 9 rows in range 0..8
// 16 col is in range 0..15
uint8_t is31_getpixel(uint8_t col, uint8_t row);

// update the full display with the in-memory display buffer
int is31_flip();

#endif