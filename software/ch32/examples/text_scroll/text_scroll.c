/*
Scrolling text demo
*/
#include "ch32fun.h"
#include <stdio.h>

#include "is31.h"
#include "font.h"

// 0..255
#define FONT_INTENSITY_VALUE 64

void text_scroll_column_delay() {
    // Delay_Ms(150); // slow
    // Delay_Ms(50); // okay
    Delay_Ms(25); // reasonably fast
    // Delay_Ms(10); // faster
}

void text_scroll(char * text)
{
    printf("text_scroll(\"%s\")\n", text);

    enum {
        STATE_LETTER_NEW = 0,
        STATE_LETTER = 1,
        STATE_LETTER_GAP = 2,
        STATE_TEXT_END = 3
    } state = STATE_LETTER_NEW;

    // clear display
    is31_clear();
    is31_flip();

    // details for current letter
    uint8_t letter_col; // column index in character, counting from left
    uint8_t letter_col_count;
    const uint8_t * letter_font = font_bitmap+1;

    while (1) {
        // advance pixels on screen left by one pixel
        for (int col=1; col<16; col++) {
            for (int row = 0; row < 9; row++) {
                is31_setpixel(col-1, row, 
                    is31_getpixel(col, row));
            }
        }

        if (state == STATE_LETTER_NEW) {
            if (text[0] == '\0') {
                state = STATE_TEXT_END;
                letter_col_count = 0;
            } else {
                state = STATE_LETTER;

                // find text[0] in font_bitmap_letters
                int letter_index = 0;
                while (1) {
                    if (font_bitmap[letter_index*10] == '\0') {
                        // not found
                        printf("no '%c' in font\n", text[0]);
                        letter_index = 0;
                        break;
                    }
                    if (font_bitmap[letter_index*10] == text[0]) {
                        // printf("found '%c' in font at index %d\n", text[0], letter_index);
                        break;
                    }
                    letter_index++;
                }

                
                letter_col = 0;
                letter_font = font_bitmap + 1 + 10*letter_index;
                
                // find how wide character is
                // bitwise or all bits in font for character together
                uint8_t font_char_bits = 0;
                for (int row = 0; row < 9; row++) {
                    font_char_bits = font_char_bits | letter_font[row];
                }
                // printf("font_char_bits of '%x'\n", font_char_bits);

                if (font_char_bits == 0) {
                    // weird, the font for this letter is blank
                    letter_col_count = 1;
                } else {
                    letter_col_count = 8;
                    // this could use count trailing zeros but I don't need to optimize that far
                    while ((1 < letter_col_count) && ((font_char_bits & 1) == 0)) {
                        font_char_bits = font_char_bits >> 1;
                        letter_col_count = letter_col_count - 1;
                    }
                }
            }

            if (text[0] == ' ') {
                letter_col_count = 2;
            }

            // printf("after STATE_LETTER_NEW\n");
            // printf("letter_col_count: %d\n", letter_col_count);
            // printf("letter_col: %d\n", letter_col);
            // printf("state: %d\n", state);
        }

        if (state == STATE_LETTER) {
            // draw next column of letter
            uint8_t col_mask = 1 << (7-letter_col);
            for (int row = 0; row < 9; row++) {
                if (letter_font[row] & col_mask ) {
                    is31_setpixel(15, row, FONT_INTENSITY_VALUE);
                } else {
                    is31_setpixel(15, row, 0);
                }
            }

            letter_col++;

            // check for end of letter
            if (letter_col_count <= letter_col) {
                state = STATE_LETTER_GAP;
            }
        } else if (state == STATE_LETTER_GAP) {
            // draw one column blank
            // then switch to next letter
            for (int row = 0; row < 9; row++) {
                is31_setpixel(15, row, 0);
            }
            text = text+1;
            state = STATE_LETTER_NEW;
        } else if (state == STATE_TEXT_END) {
            // draw blank columns until whole screen is blank
            for (int row = 0; row < 9; row++) {
                is31_setpixel(15, row, 0);
            }
            letter_col_count += 1;
            if (16 <= letter_col_count) {
                return;
            }
        }

        // printf("is31_flip()\n");
		is31_flip();
        text_scroll_column_delay();
    }
}

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

    printf("Scrolling text demo\n");

	Delay_Ms(100);

	if (!is31_init())
	{
		printf("is31 init okay!\r\n");

        while (1) {
            text_scroll("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
            text_scroll("The quick brown fox jumps over the lazy dog");
            text_scroll("Waltz, bad nymph, for quick jigs vex");
            text_scroll("Sphinx of black quartz, judge my vow.");
            text_scroll("How vexingly quick daft zebras jump!");
            text_scroll("The five boxing wizards jump quickly.");
            text_scroll("Pack my box with five dozen liquor jugs.");
        }
    }

	printf("Stuck here forever...\n\r");
	while (1)
		;
}
