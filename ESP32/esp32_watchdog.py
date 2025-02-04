from machine import WDT

# enable the WDT with a timeout of 5s (1s is the minimum)
class DeviceWatchdog:
    def __init__(self):
        # self.wdt = WDT(timeout=5000)
        pass

    def status(self):
        return "online"

    def feed(self):
        # TODO: 固定时间喂狗
        self.wdt.feed()

