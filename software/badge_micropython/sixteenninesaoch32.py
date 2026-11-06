# micropython i2c driver wrapper for SixteenNineSAO

# https://badge.team/docs/standards/sao/ describes i2c eeprom at address 0x50
# 0x50 is decimal 80
SAO_I2C_ADDRESS = 0x50
# the header is bytes at address 0 through 35
SAO_REG_BUTTON_BITS = 48
SAO_REG_LED = 49
SAO_REG_ANIMATION_CHOICE = 50
# value options for SAO_REG_ANIMATION_CHOICE 
SAO_ANIMATION_CHOICE_STATIC = 0
SAO_ANIMATION_CHOICE_CONWAYS = 1
SAO_ANIMATION_CHOICE_TEXT_SCROLL = 2
SAO_ANIMATION_CHOICE_PLASMA = 3
SAO_ANIMATION_CHOICE_BOUNCE = 4

SAO_REG_ANIM_INTENSITY_MAX = 51
# default value is 64, max is 255

# not yet implemented:
# SAO_REG_ANIM_CONWAY_SLOWDOWN = 52
# SAO_REG_ANIM_CONWAY_OPTIONS = 53
# SAO_REG_ANIM_TEXT_SLOWDOWN = 55

# register location for either:
#  - display bitmap for STATIC mode
#  - text string for TEXT_SCROLL animation
SAO_REG_DISPLAY_OR_TEXT = 64

SAO_DRIVER_NAME = 'Sixteen9Ch32V1'

class SixteenNineSaoCh32V1:
    def __init__(self, i2c, addr=SAO_I2C_ADDRESS):
        self._addr = addr
        self._i2c = i2c
        # query device, check header
        buf = self._i2c.readfrom_mem(self._addr, 0, 8)
        # buf[0] should be 0x4C is 'L' but isn't checked
        self._assert_equal(buf[1], 0x49) # 'I'
        self._assert_equal(buf[2], 0x46) # 'F'
        self._assert_equal(buf[3], 0x45) # 'E'
        namelen = buf[4]
        drivernamelen = buf[5]
        driverdatalen = buf[6]
        num_additional_drivers = buf[7]
        
        # read name, drivername, driverdata
        nextbuf = self._i2c.readfrom_mem(self._addr, 8, namelen + drivernamelen + driverdatalen)
        name_str = ''.join(chr(c) for c in nextbuf[:namelen])
        # print(f'found name: {name_str}')
        driver_name_str = ''.join(chr(c) for c in nextbuf[namelen:namelen+drivernamelen])
        # print(f'found driver_name: {driver_name_str}')
        self._assert_equal(driver_name_str, 'Sixteen9Ch32V1') # 'E'
        
        # currently I ignore "additional drivers"
    def _assert_equal(self, b1, b2):
        if b1 != b2:
            raise ValueError(f'Header expected {b1:x} got {b2:x}')
    def led_set(self, value):
        self._i2c.writeto_mem(self._addr, SAO_REG_LED, bytes([value]))
    def buttons_get(self):
        return self._i2c.readfrom_mem(self._addr, SAO_REG_BUTTON_BITS, 1)[0]
    def anim_set(self, anim):
        if (anim < 0) or (255 < anim):
            raise ValueError(f'value must be in range 0..255, got {anim}')
        self._i2c.writeto_mem(self._addr, SAO_REG_ANIMATION_CHOICE, bytes([anim]))
    def max_intensity_set(self, value):
        if (value < 0) or (255 < value):
            raise ValueError(f'value must be in range 0..255, got {value}')
        self._i2c.writeto_mem(self._addr, SAO_REG_ANIM_INTENSITY_MAX, bytes([value]))
    def static_display_set(self, pixeldata):
        if not ((type(pixeldata) is bytes) or (type(pixeldata) is bytearray)):
            raise valueError(f'Expected pixeldata of type bytes or bytearray')
        if 16*9 < len(pixeldata):
            raise ValueError(f'pixeldata must have length of up to 16*9 bytes. {value}')
        # set pixel data
        self._i2c.writeto_mem(self._addr, SAO_REG_DISPLAY_OR_TEXT, pixeldata)
        # refresh display
        self._i2c.writeto_mem(self._addr, SAO_REG_ANIMATION_CHOICE, bytes([SAO_ANIMATION_CHOICE_STATIC]))
    def scrolling_text_set(self, text):
        textmaxlen = 254-SAO_REG_DISPLAY_OR_TEXT
        if textmaxlen < len(text):
            raise ValueError('text too long')
        # todo: build bytearray of text, null terminated
        textbytes = bytearray(text, 'ascii')
        textbytes.append(0)
        # set pixel data
        self._i2c.writeto_mem(self._addr, SAO_REG_DISPLAY_OR_TEXT, textbytes)
        # refresh display
        self._i2c.writeto_mem(self._addr, SAO_REG_ANIMATION_CHOICE, bytes([SAO_ANIMATION_CHOICE_TEXT_SCROLL]))
