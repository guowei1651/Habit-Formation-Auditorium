from time import *

from esp32_main import *
from iot_device import *

def main():
    device = Device()

    # 启动网络
    device.initializeDevice()

    iotDevice = IoTDevice()
    iotDevice.loop()

main()
