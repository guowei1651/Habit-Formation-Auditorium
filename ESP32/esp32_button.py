import json
import machine

class DeviceButton:
    def __init__(self, display):
        self.display = display

        self.btn1 = machine.Pin(6, machine.Pin.IN, machine.Pin.PULL_UP)
        self.btn2 = machine.Pin(7, machine.Pin.IN, machine.Pin.PULL_UP)
        self.btn3 = machine.Pin(5, machine.Pin.IN, machine.Pin.PULL_UP)
        # TODO: 确认按钮4的引脚
        self.btn4 = machine.Pin(8, machine.Pin.IN, machine.Pin.PULL_UP)

    def status(self):
        # TODO: 获取按钮状态
        """ 获取按钮状态 """
        return {
            "btn1": self.btn1.value(),
            "btn2": self.btn2.value(),
            "btn3": self.btn3.value(),
            "btn4": self.btn4.value()
        }

    def processIRQ(self):
        # 进入配置模式的回调函数
        def enter_config_mode(pin):
            print("进入配置模式..." + json.dumps(pin))
            self.display.clear()
            self.display.draw_text(50, 100, "Config Mode , pin: " + json.dumps(pin))

        # 绑定中断
        self.btn1.irq(trigger=machine.Pin.IRQ_FALLING, handler=enter_config_mode)