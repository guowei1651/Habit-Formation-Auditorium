import machine

# from esp32_watchdog import DeviceWatchdog
from esp32_config import DeviceConfig
from esp32_dht import DeviceDHT
from esp32_display import DeviceDisplay
from esp32_time import DeviceRTC
from esp32_network import DeviceNetwork
from esp32_button import DeviceButton


class Device:
    def __new__(cls):
        if not hasattr(cls, 'instance'):
            cls.instance = super(Device, cls).__new__(cls)
        return cls.instance

    def __init__(self):
        # self.watchdog = DeviceWatchdog()
        self.config = DeviceConfig("HF")
        self.display = DeviceDisplay()
        self.network = DeviceNetwork(self.display, self.config)
        self.dht = DeviceDHT()
        self.rtc = DeviceRTC()
        self.buttons = DeviceButton(self.display)

    def status(self):
        """ 获取设备状态 """
        return {
            "watchdog": self.watchdog.status(),
            "display": self.display.status(),
            "network": self.network.status(),
            "dht": self.dht.status(),
            "rtc": self.rtc.status(),
            "buttons": self.buttons.status()
        }

    def get_device_id():
        chip_id = machine.unique_id()  # 获取 ESP32 唯一 ID
        return "".join("{:02x}".format(b) for b in chip_id)

    def initializeDevice(self):
        """ 初始化设备 """

        """ step1 配置网络 """
        self.network.configAndStartNetwork()