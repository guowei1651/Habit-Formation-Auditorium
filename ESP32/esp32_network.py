import time

import time
import socket

import network 
import ESP32.esp32_main as esp32_main
import machine

def _web_server():
    """ Web 服务器，用户输入 WiFi 配置 """
    addr = socket.getaddrinfo("0.0.0.0", 80)[0][-1]
    s = socket.socket()
    s.bind(addr)
    s.listen(5)
    print("🌐 Web 服务器启动，等待用户输入 WiFi 配置")

    while True:
        conn, addr = s.accept()
        request = conn.recv(1024).decode()

        if "GET / " in request:
            # 返回 HTML 页面
            html = """\
HTTP/1.1 200 OK
Content-Type: text/html

<!DOCTYPE html>
<html>
<head><title>ESP32 WiFi 配置</title></head>
<body>
    <h2>输入 WiFi 配置信息</h2>
    <form action="/save">
        SSID: <input type="text" name="ssid"><br>
        密码: <input type="password" name="password"><br>
        <input type="submit" value="保存">
    </form>
</body>
</html>
"""
            conn.send(html)

        elif "GET /save?" in request:
            # 解析 WiFi 配置参数
            params = request.split("GET /save?")[1].split(" ")[0]
            params = {p.split("=")[0]: p.split("=")[1] for p in params.split("&")}
            
            ssid = params.get("ssid", "").replace("+", " ")
            password = params.get("password", "")

            if ssid and password:
                save_config(ssid, password)
                conn.send("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nWiFi 配置已保存，重启设备连接 WiFi")
                time.sleep(3)
                machine.reset()
            else:
                conn.send("HTTP/1.1 400 Bad Request\r\n\r\n请输入完整的 WiFi 信息")

        conn.close()

class DeviceNetwork:
    def __init__(self, display, configMode):
        self.wlan = network.WLAN()
        self.display = display

        # 显示 WiFi 图标。如果未联网则显示需要联网
        # TODO: 显示联网之前信息
        self.display.reset()
        self.display.clear()
        self.display.draw_text(100, 200, "🚀 AP 模式启动，WiFi 名称: Habit_FORMATION，🔗 请连接 WiFi 并访问: http://192.168.4.1")

    def configAndStartNetwork(self):
        # 检查是否已联网
        if not self.wlan.isconnected():
            self.startAP()
            _web_server()
        else:
            print("✅ WiFi 已连接:", self.wlan.ifconfig());

        while network.WLAN.isconnected():
            time.sleep(5) # 等待用户配置 WiFi

    def status(self):
        # TODO: 检查网络状态
        # 需要检查是否为 STA 模式
        # 需要检查是否已联网

        return network.WLAN.status()
        
    def configNetwork(self):
        pass

    def isconnected(self):
        print("📡 检查网络连接状态 ..." + network.WLAN.status())
        return network.WLAN.status() == network.STAT_GOT_IP
    
    def connectWifi(self, ssid, password):
        if not ssid:
            print("❌ 未找到 WiFi 配置，进入 AP 配置模式")
            return False

        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        wlan.connect(ssid, password)

        print(f"📡 正在连接 WiFi: {ssid} ...")
        for _ in range(10):
            if wlan.isconnected():
                print("✅ WiFi 连接成功:", wlan.ifconfig())
                return True
            time.sleep(1)
        
        print("❌ WiFi 连接失败")
        return False
    
    def scanWifi():
        """ 扫描附近的 WiFi 网络 """
        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        networks = wlan.scan()
        return networks

    def startAP():
        """ 开启 AP 模式，用户可连接 ESP32 进行配置 """
        ap = network.WLAN(network.WLAN.IF_AP) # create access-point interface
        ap.config(essid="Habit_FORMATION")  # AP SSID & 密码
        ap.config(max_clients=10) # set how many clients can connect to the network
        ap.active(True)         # activate the interface
        print("🚀 AP 模式启动，WiFi 名称: Habit_FORMATION，🔗 请连接 WiFi 并访问: http://192.168.4.1进行配置")

        while True:
            time.sleep(1)  # AP 模式一直运行，直到用户配置完成

        ap.active(False)  # 关闭 AP 模式