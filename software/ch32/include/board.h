#ifndef _BOARD_H
#define _BOARD_H

#include "ch32fun.h"

#define IS31_PIN_SCL PC5
#define IS31_PIN_SDA PC6
// by default not connected to is31. Switch solder jump JP6 to pins 2-3 to connect
#define IS31_PIN_INT PA1
// by default not connected to is31. Switch solder jump JP7 to pins 2-3 to connect
#define IS31_PIN_AIN PA2

#define SAO_PIN_SCL PC2
#define SAO_PIN_SDA PC1

// LED lights when pin low
#define PIN_LED PD0

// right button
#define PIN_BTN_A PD2
// left button
#define PIN_BTN_B PC7

// these are on header on bottom left corner:
#define PIN_TX PD5
#define PIN_RX PD6

// as shown in datasheet, 16x9 display, split into 8 column 9 row halves left/right
#define IS31_DISPLAY_LAYOUT 1
// adafruit charlieplex bonnet is 16x8 and has flipped rows/columns from what I expect
// #define IS31_DISPLAY_LAYOUT 2

#if IS31_DISPLAY_LAYOUT == 1
#define IS31_WIDTH 16
#define IS31_HEIGHT 9

#elif IS31_DISPLAY_LAYOUT == 2
#define IS31_WIDTH 16
#define IS31_HEIGHT 8

#endif


#endif
