#include "anim.h"

#include "stdint.h"
#include "board.h"
#include "is31.h"
#include "funconfig.h"
#include "lib_rand.h"

struct conway_state
{
    // uint8_t state[IS31_HEIGHT * IS31_WIDTH];
    uint16_t rows[IS31_HEIGHT];
    uint16_t rows_older[IS31_HEIGHT];
    uint16_t steady_counter;
    uint16_t delay_counter;
    uint32_t generation_counter;
} cs;

void conways_init()
{
    cs.generation_counter = 0;
    cs.delay_counter = 0;
    cs.steady_counter = 0;
    // setup a new random state:
    for (int r = 0; r < IS31_HEIGHT; r++)
    {
        cs.rows_older[r] = 0x0000;
        cs.rows[r] = 0xFFFF & rand();
    }

    // // glider
    // for (int r = 0; r < IS31_HEIGHT; r++)
    // {
    //     cs.rows_older[r] = 0x0000;
    //     cs.rows[r] = 0x0000;
    // }
    // cs.rows[0] = 0b010;
    // cs.rows[1] = 0b100;
    // cs.rows[2] = 0b111;
}

uint8_t sample(int col, int row)
{
    while (col < 0)
        col += IS31_WIDTH;
    while (IS31_WIDTH <= col)
        col -= IS31_WIDTH;
    while (row < 0)
        row += IS31_HEIGHT;
    while (IS31_HEIGHT <= row)
        row -= IS31_HEIGHT;
    return (cs.rows[row] & (1 << col)) == 0 ? 0 : 1;
    // return (cs.state[IS31_WIDTH * row + col] & 0x01);
}

void conways_advance()
{
    // reduce update rate by not updating every conways_advance() call:
    cs.delay_counter++;
    if (cs.delay_counter <= 4) {
        return;
    }
    cs.delay_counter = 0;
    cs.generation_counter++;

    // don't get stuck with single glider flying forever:
    if (500 < cs.generation_counter) {
        // re-seed
        conways_init();
    }
    
    // was: is31_clear();
    // fadeout living cells instead of turning them fully off:
    for (int c = 0; c < IS31_WIDTH; c++)
    {
        for (int r = 0; r < IS31_HEIGHT; r++)
        {
            uint8_t val = is31_getpixel(c, r);
            is31_setpixel(c,r, val >> 2);
        }
    }

    uint16_t next[IS31_HEIGHT];
    for (int r = 0; r < IS31_HEIGHT; r++) {
        next[r] = 0;
    }

    for (int c = 0; c < IS31_WIDTH; c++)
    {
        for (int r = 0; r < IS31_HEIGHT; r++)
        {
            uint8_t was_alive = sample(c, r);
            uint8_t living_neighbors = 0;
            for (int dc = -1; dc <= 1; dc++)
            {
                for (int dr = -1; dr <= 1; dr++)
                {
                    living_neighbors += sample(c + dc, r + dr);
                }
            }
            living_neighbors -= was_alive;
            uint8_t alive = 0;

            if (was_alive && living_neighbors < 2)
                alive = 0;
            else if (was_alive && living_neighbors <= 3)
                alive = 1;
            else if (was_alive)
                alive = 0;
            else if (!was_alive && living_neighbors == 3)
                alive = 1;
            else
                alive = 0;

            next[r] = next[r] | (alive << c);

            if (alive)
            {
                is31_setpixel(c, r, max_intensity);
            }
        }
    }

    // check for if state is same as 2 previous
    cs.steady_counter++;
    for (int r = 0; r < IS31_HEIGHT; r++)
    {
        if (cs.rows_older[r] != next[r]) {
            cs.steady_counter = 0;
        }
    }

    // copy next state over previous state
    for (int r = 0; r < IS31_HEIGHT; r++)
    {
        cs.rows_older[r] = cs.rows[r];
        cs.rows[r] = next[r];
    }

    // TODO: make this threshold more time-based, like 1 second of just blinker, not update-count based
    if (cs.steady_counter >= 25) {
        conways_init();
    }
    
    is31_flip();
}