#include "anim.h"
#include "board.h"
#include "is31.h"

struct bounce_state
{
    int width, height;
    int dx, dy;
    int x, y;
} b;

void bounce_init()
{
    // todo: random start position
    // todo: random width
    // todo: random start direction
    b.dx = 1;
    b.dy = 1;
    b.x = 0;
    b.y = 0;
    b.width = 2;
    b.height = 1;
}


void bounce_advance()
{
    is31_clear();

    // draw box
    for (int dw = 0; dw < b.width; dw++)
    {
        for (int dh = 0; dh < b.height; dh++)
        {
            is31_setpixel(b.x + dw, b.y + dh, max_intensity);
        }
    }

    if (b.x + b.dx < 0)
    {
        b.dx *= -1;
    }
    else if (IS31_WIDTH <= b.x + b.dx + b.width - 1)
    {
        b.dx *= -1;
    }

    if (b.y + b.dy < 0)
    {
        b.dy *= -1;
    }
    else if (IS31_HEIGHT <= b.y + b.dy + b.height - 1)
    {
        b.dy *= -1;
    }

    // advance position
    b.x += b.dx;
    b.y += b.dy;

    is31_flip();
}