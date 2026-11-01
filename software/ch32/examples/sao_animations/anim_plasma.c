#include "anim.h"
#include "is31.h"

#include <math.h>

// plasma animation state
struct
{
    float phase;
    float phaseIncrement;
    float colorStretch;
} ps;

float my_absf(float f)
{
    if (f < 0)
        return -f;
    return f;
}

#define MY_PI 3.1415926f


float my_sinf(float f)
{
    // return sinf(f);

    // triangle wave
    // f = my_absf(f/3.14f);
    // return fabs(floorf(f) - f - 0.5f)*2.0f - 1.0f;

    // sawtooth wave
    f = my_absf(f / 3.1415926f);
    return (floorf(f) - f) * 2.0f - 1.0f;

    // approximation?
    // sin(x) = cos(x + pi/2)
    // float temp = my_absf(f + 0.5f * MY_PI) / MY_PI;
    // f = floorf(f) - f;
    // // approximation for cos(f)
    // if (f <= 0.5f * MY_PI) {
    //     // positive
    //     return 1.0f - (5 * f * f) / (MY_PI * MY_PI + f * f);
    // } else {
    //     // negative
    //     f = f - 0.5f * MY_PI;
    //     temp = 1.0f - (5 * f * f) / (MY_PI * MY_PI + f * f);
    //     return -temp;
    // }
}

float my_sqrtf(float dx, float dy)
{
    // compute approximation for sqrt(dx*dx+dy*dy)
    float s = dx*dx + dy*dy;
    float est = 
        (dx + dy * dy / 2 / dx);
    for (int i = 0; i < 2; i++) {
        est = 0.5f * (est + (s / est));
    }
    return est;
}

void plasma_init()
{
    // copied/modified from LoLShield_Plasma example
    ps.phase = 0.0f;
    ps.phaseIncrement = 0.08f;
    ps.colorStretch = 0.11f;
}

void plasma_advance()
{
    ps.phase += ps.phaseIncrement;

    float p1x = (my_sinf(ps.phase * 1.000f) + 1.0f) * 7.5f;
    float p1y = (my_sinf(ps.phase * 1.310f) + 1.0f) * 4.0f;
    float p2x = (my_sinf(ps.phase * 1.770f) + 1.0f) * 7.5f;
    float p2y = (my_sinf(ps.phase * 2.865f) + 1.0f) * 4.0f;

    for (int row = 0; row < 9; row++)
    {
        float row_f = row;
        for (int col = 0; col < 16; col++)
        {
            float col_f = col;

            // Calculate the distance between this LED, and p1.
            // The vector from p1 to this LED.
            float dist1x = my_absf(col_f - p1x);
            float dist1y = my_absf(row_f - p1y);
            // float distance = dist1x * dist1x + dist1y * dist1y;

            // Calculate the distance between this LED, and p2.
            // The vector from p2 to this LED.
            float dist2x = my_absf(col_f - p2x);
            float dist2y = my_absf(row_f - p2y);
            // Multiply this with the other distance, this will create weird plasma values :)
            // distance *= dist2x * dist2x + dist2y * dist2y;
            // distance = sqrtf(distance); // (.text.sqrtf+0x26): undefined reference to `__errno'

            // approximate distances sqrt(x*x+y*y) as (fabs(x)+y*y/2/fabs(x))
            float distance =
                my_sqrtf(dist1x, dist1y) *
                my_sqrtf(dist2x, dist2y);

            // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
            // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
            float color_f = (my_sinf(distance * ps.colorStretch) + 1.0f) * 0.5f; // range: 0.0...1.0

            // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
            color_f *= color_f;
            // color_f *= color_f*color_f*color_f # Uncomment this line to make it even darker :)

            // is31_setpixel(col, row, (int)(color_f * 255)); // brighter
            is31_setpixel(col, row, (int)(color_f * 64.0f)); // dimmer
        }
    }

    is31_flip();
}