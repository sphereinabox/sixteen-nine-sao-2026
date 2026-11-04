## Simple is31fl3731 driver for MicroPython
# example hardware: https://www.adafruit.com/product/2946

# default i2c addr 0x74
# can also be 0x75, 0x76 or 0x77

# can output 16x9 grid of grayscale (8 bit) intensity
# has memory for 8 screens of data, recommends using them to get full-screen synchronized display update
# 400kHz max i2c frequency


class SixteenNine:
    def __init__(self, i2c):
        self._i2c = i2c
        self.buf2 = bytearray(2)
        self.i2c_addr = 0x74
        self.display_buf = bytearray([0x24] + [0x00]*144)
        self.nextfb = 0 # toggle between frames in chip 0/1 to present the entire display at once
        
    def _i2c_set_byte(self, addr, value):
        'set one byte inside led driver at address addr'
        self.buf2[0] = addr
        self.buf2[1] = value
        self._i2c_write(self.buf2)

    def _i2c_write(self, buf):
        self._i2c.writeto(self.i2c_addr, buf)
    
    def led_init(self):
        # set page to config bank
        self._i2c_set_byte(0xFD, 0x0B)
        # write config
        init_bytes = bytes([
            # destination address
            0x00, 
            # 0x00: configuration register
            0b000_00_000,
            # 0x01 picture display register
            0b00000_000,
            # 0x02 auto play control
            0b0_000_0_000,
            # 0x03 auto play control 2
            0b00_00000,
            # 0x04 reserved (no function)
            0x00,
            # 0x05 display option
            0b00_0_0_0_000,
            # 0x06 audio synchronization register
            0b0000000_0,
            # 0x07 frame state
            0b000_0_0_000,
            # 0x08 breath control
            0b0_000_0_000,
            # 0x09 breath control 2
            0b000_0_0_000,
            # 0x0A shutdown, normal operation
            0b0000000_1,
            # 0x0B AGC control
            0b000_0_0_000,
            # 0x0C audio ADC rate
            0b0000_0000
            ])
        self._i2c_write(init_bytes)
        
        # reset pages:
        enable_bytes = bytes([0x00] + [0xFF]*18)
        intensity_buf = bytes([0x24] + [0x00]*144)
        for page in range(8):
            # set page
            self._i2c_set_byte(0xFD, page)
            # enable outputs
            self._i2c_write(enable_bytes)
            # set per-pixel intensity
            self._i2c_write(intensity_buf)
        # set page 0
        self._i2c_set_byte(0xFD, 0)

    def set_pixel_slow(self, page, address, val):
        # set page (0xFD is BANK_ADDRESSS)
        self._i2c_set_byte(0xFD, page)
        # 0x24 is COLOR_OFFSET
        self._i2c_set_byte(0x24 + address, val)
        
    def set_pixel(self, col, row, color):
        pixel_addr = 16*row + col + 1
        self.display_buf[pixel_addr] = color
    
    def update(self):
        # set page (0xFD is BANK_ADDRESSS)
        #self._i2c_set_byte(0xFD, nextfb)
        # write intensity values
        self._i2c_write(self.display_buf)
        #     # switch to config page
        #     self._i2c_set_byte(0xFD, 0x0B)
        #     # change displayed page
        #     self._i2c_set_byte(0x00, nextfb)

        self.nextfb = (self.nextfb + 1) & 0b111

