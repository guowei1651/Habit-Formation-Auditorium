import urequests
import time

import framebuf
from machine import Pin, SPI

# NeoPixel and APA106 driver¶
'''from neopixel import NeoPixel
pin = Pin(0, Pin.OUT)   # set GPIO0 to output to drive NeoPixels
np = NeoPixel(pin, 8)   # create NeoPixel driver on GPIO0 for 8 pixels
np[0] = (255, 255, 255) # set the first pixel to white
np.write()              # write data to all pixels
r, g, b = np[0]         # get first pixel colour


import epaper
display = epaper.GDEW075T8()
display.init()
display.clear()
'''

# SPI 配置
HSPI_CLK = 18
HSPI_MOSI = 23
HSPI_CS = 5
EINK_DC = 19
EINK_RST = 16
EINK_BUSY = 17

class DeviceDisplay:
    def __init__(self):


class DeviceDisplay234:
    def __init__(self):
        # 初始化 SPI & GPIO
        self.spi = SPI(2, baudrate=4000000, polarity=0, phase=0, sck=Pin(HSPI_CLK), mosi=Pin(HSPI_MOSI))
        self.cs = Pin(HSPI_CS, Pin.OUT)
        self.dc = Pin(EINK_DC, Pin.OUT)
        self.rst = Pin(EINK_RST, Pin.OUT)
        self.busy = Pin(EINK_BUSY, Pin.IN)

        self.width = 800
        self.height = 480
        self.buffer = bytearray(self.width * self.height // 8)
        self.fb = framebuf.FrameBuffer(self.buffer, self.width, self.height, framebuf.MONO_HLSB)

    def status(self):
        return "online"

    def reset(self):
        self.rst.value(1)
        time.sleep(0.2)
        self.rst.value(0)
        time.sleep(0.2)
        self.rst.value(1)
        time.sleep(0.2)

    def send_command(self, command):
        self.dc.value(0)
        self.cs.value(0)
        self.spi.write(bytearray([command]))
        self.cs.value(1)

    def send_data(self, data):
        self.dc.value(1)
        self.cs.value(0)
        self.spi.write(bytearray([data]))
        self.cs.value(1)

    def clear(self):
        self.fb.fill(1)
        self.update()

    def draw_text(self, x, y, text, size=2):
        self.fb.text(text, x, y, 0)
        self.update()

    def update(self):
        # 刷新屏幕
        for i in range(len(self.buffer)):
            self.send_data(self.buffer[i])
        self.send_command(0x22)  # 更新指令
        self.send_command(0x20)  # 执行刷新