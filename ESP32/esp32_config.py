import json

from esp32 import NVS

class DeviceConfig:
    def __init__(self, namespace):
        self.config = NVS(namespace) # 初始化 NVS, 命名空间为 HF

    def status(self):
        return "online"

    def saveConfig(self, key, value, is_json=False):
        """ 保存 WiFi 配置到文件（模拟 NVS） """
        if (is_json):
            self.config.set_blob(key, json.dumps(value))
        else:
            self.config.set_blob(key, value)
        self.config.commit()

    def loadConfig(self, key, is_json=False):
        """ 从文件加载 WiFi 配置 """
        buf = bytearray(128)
        self.config.get_blob(key, buf)
        result = str(buf, "utf-8")

        if (is_json):
            return json.load(result)    
        else:
            return result