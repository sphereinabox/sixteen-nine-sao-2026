#ifndef _ANIM_H
#define _ANIM_H

#include <stdint.h>

// defined in textscroll_font.c
extern const uint8_t font_bitmap[];

void bounce_init();
void bounce_advance();

void conways_init();
void conways_advance();

void plasma_init();
void plasma_advance();

void textscroll_init();
void textscroll_advance();

#endif
