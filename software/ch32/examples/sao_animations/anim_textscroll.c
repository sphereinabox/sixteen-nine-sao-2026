#include "ch32fun.h"
#include <stdio.h>
#include "is31.h"
#include "anim.h"
#include <stdint.h>

// text-scroll state
struct
{
    enum
    {
        STATE_LETTER_NEW = 0,
        STATE_LETTER = 1,
        STATE_LETTER_GAP = 2,
        STATE_TEXT_END = 3
    } state;
    uint8_t letter_col; // column index in character, counting from left
    // number of columns wide the character takes:
    uint8_t letter_col_count;
    // one byte per row of pixels for letter, first is top row and so-on.
    uint8_t letter_font[9];
    char *text;
} ts;

// find width of character and letter bitmap
// fills: ts.letter_col_count, ts.letter_font
void find_letter_bitmap(char c) {
    // start with special cases:
    if (c == ' ') {
        ts.letter_col_count = 2;
        memset(ts.letter_font, 0x00, 9);
        return;
    }

    // character not in font:
    ts.letter_col_count = 7;
    ts.letter_font[0] = 0b11111110;
    ts.letter_font[1] = 0b10000010;
    ts.letter_font[2] = 0b11000110;
    ts.letter_font[3] = 0b10101010;
    ts.letter_font[4] = 0b10010010;
    ts.letter_font[5] = 0b10101010;
    ts.letter_font[6] = 0b11000110;
    ts.letter_font[7] = 0b10000010;
    ts.letter_font[8] = 0b11111110;


    // find c in font_bitmap_letters
    int letter_index = 0;
    while (1)
    {
        if (font_bitmap[letter_index * 10] == '\0')
        {
            // not found
            printf("no '%c' in font\n", ts.text[0]);
            letter_index = 0;
            break;
        }
        if (font_bitmap[letter_index * 10] == ts.text[0])
        {
            // printf("found '%c' in font at index %d\n", text[0], letter_index);
            break;
        }
        letter_index++;
    }

    ts.letter_col = 0;
    memcpy(
        ts.letter_font,
        font_bitmap + 1 + 10 * letter_index,
        9);

    // find how wide character is
    // bitwise or all bits in font for character together
    uint8_t font_char_bits = 0;
    for (int row = 0; row < 9; row++)
    {
        font_char_bits = font_char_bits | ts.letter_font[row];
    }
    // printf("font_char_bits of '%x'\n", font_char_bits);

    if (font_char_bits == 0)
    {
        // weird, the font for this letter is blank
        ts.letter_col_count = 1;
    }
    else
    {
        ts.letter_col_count = 8;
        // this could use count trailing zeros but I don't need to optimize that far
        while ((1 < ts.letter_col_count) && ((font_char_bits & 1) == 0))
        {
            font_char_bits = font_char_bits >> 1;
            ts.letter_col_count = ts.letter_col_count - 1;
        }
    }
}

void textscroll_init()
{
    ts.state = STATE_LETTER_NEW;
    ts.text = 
        "Hello Hackaday Supercon X 2026\n"
        "The quick brown fox jumps over the lazy dog\n"
        "Waltz, bad nymph, for quick jigs vex\n"
        "Sphinx of black quartz, judge my vow.\n"
        "How vexingly quick daft zebras jump!\n"
        "The five boxing wizards jump quickly.\n"
        "Pack my box with five dozen liquor jugs.";

    is31_clear();
}

void textscroll_advance()
{
    // advance pixels on screen left by one pixel
    for (int col = 1; col < 16; col++)
    {
        for (int row = 0; row < 9; row++)
        {
            is31_setpixel(
                col - 1, row,
                is31_getpixel(col, row));
        }
    }

    if (ts.state == STATE_LETTER_NEW)
    {
        if (ts.text[0] == '\0' || ts.text[0] == '\n')
        {
            ts.state = STATE_TEXT_END;
            ts.letter_col_count = 0;
        }
        else
        {
            ts.state = STATE_LETTER;
            find_letter_bitmap(ts.text[0]);
        }

        // printf("after STATE_LETTER_NEW\n");
        // printf("letter_col_count: %d\n", letter_col_count);
        // printf("letter_col: %d\n", letter_col);
        // printf("state: %d\n", state);
    }

    if (ts.state == STATE_LETTER)
    {
        // draw next column of letter
        uint8_t col_mask = 1 << (7 - ts.letter_col);
        for (int row = 0; row < 9; row++)
        {
            if (ts.letter_font[row] & col_mask)
            {
                is31_setpixel(15, row, max_intensity);
            }
            else
            {
                is31_setpixel(15, row, 0);
            }
        }

        ts.letter_col++;

        // check for end of letter
        if (ts.letter_col_count <= ts.letter_col)
        {
            ts.state = STATE_LETTER_GAP;
        }
    }
    else if (ts.state == STATE_LETTER_GAP)
    {
        // draw one column blank
        // then switch to next letter
        for (int row = 0; row < 9; row++)
        {
            is31_setpixel(15, row, 0);
        }
        ts.text = ts.text + 1;
        ts.state = STATE_LETTER_NEW;
    }
    else if (ts.state == STATE_TEXT_END)
    {
        // draw blank columns until whole screen is blank
        for (int row = 0; row < 9; row++)
        {
            is31_setpixel(15, row, 0);
        }
        ts.letter_col_count += 1;
        if (16 <= ts.letter_col_count)
        {
            if (ts.text[0] == '\0') {
                // return;
                textscroll_init();
            } else {
                ts.text = ts.text + 1;
                ts.state = STATE_LETTER_NEW;
            }
        }
    }

    // printf("is31_flip()\n");
    is31_flip();
}