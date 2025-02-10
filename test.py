hspi.begin(18, 19, 23, 5);
hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display(GxEPD2_750c_Z08(/CS=5/ 5, /DC=/ 19, /RST=/ 16, /BUSY=/ 17)); // GDEW075Z08 800x480, GD7965


from machine import Pin
btn2 = Pin(34, Pin.IN) # 按钮2
btn3 = Pin(35, Pin.IN) # 按钮3
btn4 = Pin(39, Pin.IN) # 按钮4


import epaper7in5b
from machine import SPI, Pin
import framebuf

int BUSY_Pin = 17; 
int RES_Pin = 16; 
int DC_Pin = 19; 
int CS_Pin = 5; 
int SCK_Pin = 18; 
int SDI_Pin = 23; 

pinMode(BUSY_Pin, INPUT); 
pinMode(RES_Pin, OUTPUT);  
pinMode(DC_Pin, OUTPUT);    
pinMode(CS_Pin, OUTPUT);    
pinMode(SCK_Pin, OUTPUT);    
pinMode(SDI_Pin, OUTPUT);    

# 初始化 e-Paper
spi = SPI(1, baudrate=2000000, polarity=0, phase=0, sck=Pin(18, Pin.OUT), mosi=Pin(23, Pin.OUT), miso=Pin(19, Pin.OUT))

# E-Paper 相关引脚
cs = Pin(5, Pin.OUT)      # 片选
dc = Pin(19, Pin.OUT)     # 数据/命令
rst = Pin(16, Pin.OUT)    # 复位
busy = Pin(17, Pin.IN)    # 忙信号

# 初始化 E-Paper
epd = epaper7in5b.EPD(spi, cs, dc, rst, busy)
epd = epaper7in5.EPD(spi, cs, dc, rst, busy)
epd.init()

# 创建 FrameBuffer (黑白)
buf = bytearray(epd.width * epd.height // 4)  # 1-bit per pixel
fb = framebuf.FrameBuffer(buf, epd.width, epd.height, framebuf.MONO_HLSB)

# 清屏
fb.fill(1)

# 在 (10, 50) 位置写文字
fb.text("Hello, ePaper!", 10, 50, 1)  # 0 表示黑色

# 将 FrameBuffer 数据写入 e-Paper
epd.display_frame(buf)

from machine import ADC, Pin

adc = ADC(Pin(36))  # 假设电池电压接在 GPIO34
adc.atten(ADC.ATTN_11DB)  # 让 ADC 支持 0~3.3V 输入

R1 = 10000  # 上拉电阻 100kΩ
R2 = 10000   # 下拉电阻 10kΩ
factor = (R1 + R2) / R2  # 分压系数

from machine import ADC, Pin
import utime
adc = ADC(Pin(36, Pin.IN))
adc.atten(ADC.ATTN_11DB)
weakIO = Pin(25, Pin.OUT)
weakIO.value(0)
utime.sleep_ms(100)
adc.read_uv()

def read_battery_voltage():
    raw = adc.read()  # 读取 ADC 值（范围 0~4095）
    print(raw)
    voltage = (raw / 4095) * 3.3  # 计算 ADC 电压
    return voltage * factor  # 计算原始电池电压

print("Battery Voltage:", read_battery_voltage(), "V")
