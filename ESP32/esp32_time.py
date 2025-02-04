import time
import ntptime

class DeviceRTC:
    def __init__(self, host = "pool.ntp.org", updateCycle=24 * 60 * 60): # 默认每天更新一次
        self.updateCycle = updateCycle
        self.ntpServer = host

        # TODO: 每天更新一次时间的代码

    def status(self):
        return "online"

    def sync_ntp(self):
        ntptime.host = self.ntpServer
        ntptime.settime()
        print(time.localtime())

class DeviceTimer:
    def __init__(self, interval=60):
        self.interval = interval
        self.lastTime = time.time()

    def check(self):
        if time.time() - self.lastTime > self.interval:
            self.lastTime = time.time()
            return True
        return False