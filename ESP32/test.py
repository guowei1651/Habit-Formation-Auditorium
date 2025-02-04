from machine import Pin, SPI
import time

# GPIO 引脚定义
BUSY_Pin = Pin(17, Pin.IN)
RES_Pin = Pin(16, Pin.OUT)
DC_Pin = Pin(19, Pin.OUT)
CS_Pin = Pin(5, Pin.OUT)
SCK_Pin = Pin(18, Pin.OUT)
SDI_Pin = Pin(23, Pin.OUT)

# 初始化 SPI
spi = SPI(1, baudrate=2000000, polarity=0, phase=0, sck=SCK_Pin, mosi=SDI_Pin)

# GPIO 控制函数
def digital_write(pin, value):
    pin.value(value)

def digital_read(pin):
    return pin.value()

# EPD 控制宏
def EPD_W21_CS_0(): digital_write(CS_Pin, 0)
def EPD_W21_CS_1(): digital_write(CS_Pin, 1)

def EPD_W21_DC_0(): digital_write(DC_Pin, 0)
def EPD_W21_DC_1(): digital_write(DC_Pin, 1)

def EPD_W21_RST_0(): digital_write(RES_Pin, 0)
def EPD_W21_RST_1(): digital_write(RES_Pin, 1)

def is_EPD_W21_BUSY(): return digital_read(BUSY_Pin)

# 延时函数
def driver_delay_us(xus):
    time.sleep_us(xus)

def driver_delay_xms(xms):
    time.sleep_ms(xms)

def lcd_chkstatus():
    while is_EPD_W21_BUSY():  # 等待屏幕空闲
        time.sleep_ms(10)

# SPI 传输
def SPI_Write(value):
    EPD_W21_CS_0()
    spi.write(bytearray([value]))
    EPD_W21_CS_1()

def EPD_W21_WriteCMD(command):
    EPD_W21_CS_0()
    EPD_W21_DC_0()  # 命令模式
    SPI_Write(command)
    EPD_W21_CS_1()

def EPD_W21_WriteDATA(data):
    EPD_W21_CS_0()
    EPD_W21_DC_1()  # 数据模式
    SPI_Write(data)
    EPD_W21_CS_1()

# e-Paper 初始化
def EPD_W21_Init():
    EPD_W21_RST_0()
    time.sleep_ms(100)
    EPD_W21_RST_1()
    time.sleep_ms(100)

def EPD_init():
    EPD_W21_Init()
    EPD_W21_WriteCMD(0x01)  # POWER SETTING
    EPD_W21_WriteDATA(0x07)
    EPD_W21_WriteDATA(0x07)  # VGH=20V, VGL=-20V
    EPD_W21_WriteDATA(0x3f)  # VDH=15V
    EPD_W21_WriteDATA(0x3f)  # VDL=-15V

    EPD_W21_WriteCMD(0x04)  # Power on
    lcd_chkstatus()  # 等待屏幕空闲

    EPD_W21_WriteCMD(0x00)  # PANEL SETTING
    EPD_W21_WriteDATA(0x0F)

    EPD_W21_WriteCMD(0x61)  # TRES 分辨率设置
    EPD_W21_WriteDATA(0x03)  # 800x480
    EPD_W21_WriteDATA(0x20)
    EPD_W21_WriteDATA(0x01)
    EPD_W21_WriteDATA(0xE0)

    EPD_W21_WriteCMD(0x15)
    EPD_W21_WriteDATA(0x00)

    EPD_W21_WriteCMD(0x50)  # VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x11)
    EPD_W21_WriteDATA(0x07)

    EPD_W21_WriteCMD(0x60)  # TCON SETTING
    EPD_W21_WriteDATA(0x22)

def EPD_refresh():
    EPD_W21_WriteCMD(0x12)  # DISPLAY REFRESH
    driver_delay_xms(100)
    lcd_chkstatus()

def EPD_sleep():
    EPD_W21_WriteCMD(0x50)  # VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x97)

    EPD_W21_WriteCMD(0x02)  # Power off
    lcd_chkstatus()
    
    EPD_W21_WriteCMD(0x07)  # Deep sleep
    EPD_W21_WriteDATA(0xA5)

# 显示图片
def PIC_display1():
    EPD_W21_WriteCMD(0x10)  # 传输旧数据
    for _ in range(48000):
        EPD_W21_WriteDATA(0xFF)  # BW 数据（填充白色）

    EPD_W21_WriteCMD(0x13)  # 传输新数据
    for _ in range(48000):
        EPD_W21_WriteDATA(0x00)  # Red 数据（填充黑色）

def PIC_display_Clean():
    EPD_W21_WriteCMD(0x10)  # 旧数据
    for _ in range(48000):
        EPD_W21_WriteDATA(0xFF)  # 全白

    EPD_W21_WriteCMD(0x13)  # 新数据
    for _ in range(48000):
        EPD_W21_WriteDATA(0x00)  # 全黑

# ESP32 低功耗睡眠
def espSleep(minutes):
    from machine import deepsleep
    deepsleep(minutes * 60 * 1000)

# 运行主程序
def main():
    EPD_init()
    PIC_display1()
    EPD_refresh()
    EPD_sleep()
    time.sleep(3)

    # 清除屏幕
    EPD_init()
    PIC_display_Clean()
    EPD_refresh()
    EPD_sleep()

    espSleep(60)

if __name__ == "__main__":
    main()
