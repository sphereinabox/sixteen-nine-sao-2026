# badge code, plasma animation
# talks directly to IS31FL3731 led driver,
# which requires JP4,JP5 to be closed to connect LED driver i2c to SAO i2c
#
# adapted from Arduino lolshield example previously on code.google.com
# currently there's a copy at https://github.com/jprodgers/LoLshield/blob/master/examples/Animations/LoLShield_Plasma/LoLShield_Plasma.ino

from is31_mpy import SixteenNine
display = SixteenNine(i2c0)
## if you're using other i2c pins:
# from machine import I2C, Pin
# I2C(scl, sda, freq=400000):
# i2c = I2C(Pin(27),Pin(26)freq=400000) 
    
display.led_init()

# test anim:
# for col in range(16):
#     for row in range(9):
#         addr = 16*row + col
#         set_pixel_slow(0, addr, 64)
#         time.sleep_ms(500)

# plasma

from math import sin, sqrt
import time

# copied/modified from LoLShield_Plasma example
phase = 0.0
phaseIncrement = 0.08
colorStretch = 0.11

framecount=0
next_framecount_update = 1 + time.time()
while True:
    framecount += 1
    phase += phaseIncrement

    p1 = ( (sin(phase*1.000)+1.0) * 7.5, (sin(phase*1.310)+1.0) * 4.0 )
    p2 = ( (sin(phase*1.770)+1.0) * 7.5, (sin(phase*2.865)+1.0) * 4.0 )


    for row in range(9):
        row_f = float(row)

        for col in range(16):
            col_f = float(col)

            # Calculate the distance between this LED, and p1.
            dist1 = ( col_f - p1[0], row_f - p1[1] ) # The vector from p1 to this LED.
            distance = dist1[0]*dist1[0] + dist1[1]*dist1[1]

            # Calculate the distance between this LED, and p2.
            dist2 = ( col_f - p2[0], row_f - p2[1] ) # The vector from p2 to this LED.
            # Multiply this with the other distance, this will create weird plasma values :)
            distance *= dist2[0]*dist2[0] + dist2[1]*dist2[1]
            distance = sqrt(distance)

            # Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
            # You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
            color_f = (sin( distance * colorStretch ) + 1.0) * 0.5 # range: 0.0...1.0

            # Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
            color_f *= color_f
            #color_f *= color_f*color_f*color_f # Uncomment this line to make it even darker :)

            # Scale the color up to 0..7 . Max brightness is 7.
            #LedSign::Set( col, row, byte( round(color_f * 7.0) ) );
            #my_setpixel(col, row, int(color_f * 64))
#             display.set_pixel(col, row, int(color_f * 255)) # brighter
            display.set_pixel(col, row, int(color_f * 64)) # dimmer

    display.update()

#     update_dur = time() - start_time
#     sleep_dur = 0.015 - update_dur
#     if sleep_dur < 0:
#         print('display update took ', update_dur)
#     else:
#         sleep(sleep_dur)
    #time.sleep_ms(1)
    now_monotonic = time.time()
    if next_framecount_update < now_monotonic:
        print(f'{framecount} fps')
        framecount = 0
        next_framecount_update = 1 + now_monotonic
