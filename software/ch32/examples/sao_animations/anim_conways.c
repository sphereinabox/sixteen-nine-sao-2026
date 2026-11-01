#include "anim.h"

#include "stdint.h"
#include "board.h"
#include "is31.h"
#include "funconfig.h"
#include "lib_rand.h"

#define CONWAYS_INTENSITY 64

struct conway_state
{
    // uint8_t state[IS31_HEIGHT * IS31_WIDTH];
    uint16_t rows[IS31_HEIGHT];
} cs;

void conways_init()
{
    // setup a new state
    for (int r = 0; r < IS31_HEIGHT; r++)
    {
        cs.rows[r] = 0xFFFF & rand();
        // cs.rows[r] = 0;
        // for (int c = 0; c < IS31_WIDTH; c++)
        // {
        //     cs.state[IS31_WIDTH * r + c] = 0;
        // }
    }

    // glider
    // cs.rows[0] = 0b010;
    // cs.rows[1] = 0b100;
    // cs.rows[2] = 0b111;

    // glider
    // cs.state[IS31_WIDTH * 0 + 1] = 1;
    // cs.state[IS31_WIDTH * 1 + 2] = 1;
    // cs.state[IS31_WIDTH * 2 + 0] = 1;
    // cs.state[IS31_WIDTH * 2 + 1] = 1;
    // cs.state[IS31_WIDTH * 2 + 2] = 1;
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
    is31_clear();

    uint16_t next[IS31_HEIGHT];
    for (int r = 0; r < IS31_HEIGHT; r++) {
        next[r] = 0;
    }

    // uint8_t next[IS31_HEIGHT * IS31_WIDTH];
    // for (int r = 0; r < IS31_HEIGHT; r++)
    // {
    //     for (int c = 0; c < IS31_WIDTH; c++)
    //     {
    //         next[IS31_WIDTH * r + c] = 0;
    //     }
    // }

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
            // // up left
            // living_neighbors += sample(c - 1, r - 1);
            // // up center
            // living_neighbors += sample(c + 0, r - 1);
            // // up right
            // living_neighbors += sample(c + 1, r - 1);
            // // left
            // living_neighbors += sample(c - 1, r + 0);
            // // right
            // living_neighbors += sample(c + 1, r + 0);
            // // down left
            // living_neighbors += sample(c - 1, r + 1);
            // // down center
            // living_neighbors += sample(c + 0, r + 1);
            // // down right
            // living_neighbors += sample(c + 1, r + 1);
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
            // next[IS31_WIDTH * r + c] = alive;

            if (alive)
            {
                is31_setpixel(c, r, CONWAYS_INTENSITY);
            }
        }
    }

    // copy next state over previous state
    for (int r = 0; r < IS31_HEIGHT; r++)
    {
        cs.rows[r] = next[r];
    }

    // for (int r = 0; r < IS31_HEIGHT; r++)
    // {
    //     for (int c = 0; c < IS31_WIDTH; c++)
    //     {
    //         cs.state[IS31_WIDTH * r + c] =
    //             next[IS31_WIDTH * r + c] |
    //             (0xFF & (cs.state[IS31_WIDTH * r + c] << 1));
    //     }
    // }

    is31_flip();
}