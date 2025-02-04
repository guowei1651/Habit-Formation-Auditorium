import time
import json
import machine
import urequests

BASE_URL = "http://你的服务器地址"

class IoTDevice:
    def __init__(self, device):
        self.device = device

    def status(self):
        return "online"

    def register_device(self):
        device_id = self.deviceget_device_id()
        payload = {"device_id": device_id, "status": "online"}
        
        response = urequests.post(BASE_URL + "/register", json=payload)
        print("注册设备:", response.text)
        response.close()

    def get_carousel_images(self):
        response = urequests.get(BASE_URL + "/get_images")
        images = response.json()
        print("轮播图片:", images)
        response.close()
        return images

    def download_image(self, url, filename):
        response = urequests.get(url)
        with open(filename, "wb") as f:
            f.write(response.content)
        response.close()
        print("下载完成:", filename)

    def send_heartbeat(self):
        device_id = get_device_id()
        payload = {"device_id": device_id, "status": "active"}
        
        response = urequests.post(BASE_URL + "/heartbeat", json=payload)
        print("心跳:", response.text)
        response.close()

    def update_script(self, url, filename):
        response = urequests.get(url)
        with open(filename, "w") as f:
            f.write(response.text)
        response.close()
        print("更新完成:", filename)
        
        machine.reset()

    def update_firmware(self, firmware_url):
        response = urequests.get(firmware_url, stream=True)
        with open("firmware.bin", "wb") as f:
            while True:
                chunk = response.raw.read(1024)
                if not chunk:
                    break
                f.write(chunk)
        response.close()
        
        print("固件下载完成，重启设备")
        machine.reset()

    def loop(self):
        # 注册设备
        while True:
            isRegister = self.register_device()
            if isRegister:
                break;
            time.sleep(5) # 每 5 秒重试一次

        # 发送心跳
        while True:
            self.send_heartbeat()
            time.sleep(5 * 60) # 每 5 分钟发送一次心跳