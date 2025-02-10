#include <Arduino.h>
#include <HardwareSerial.h>

#include <hal/gpio_types.h>
#include <esp_task_wdt.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_bt.h>
#include <esp_bt_main.h>

#include "../include/esp32_log.h"
#include "../include/esp32_pins.h"
#include "../include/esp32_button_interrupts.h"
#include "../include/esp32_display.h"

// https://esp32.com/viewtopic.php?t=10647

// onebutton https://blog.csdn.net/weixin_44887565/article/details/132487945
// xTaskCreate https://blog.51cto.com/u_14499/9758534
// 同上 https://blog.csdn.net/MHD0815/article/details/136825267

const char* buttonTag = "button";

// 防抖延时
const unsigned long DEBOUNCE_DELAY = 200;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
TaskHandle_t button1TaskHandle = NULL;

gpio_int_type_t isr_mode[] = {GPIO_INTR_HIGH_LEVEL, GPIO_INTR_LOW_LEVEL};

IRAM_ATTR void gpio_isr(void *param)
{
    Button *button = (Button *)param;
    ESP32_LOGD(buttonTag, "start gpio isr : %d cnt: %d", button->getPin(), button->getISRCount());
    button->incrementCount();

    esp_task_wdt_reset(); // 手动喂狗，防止超时

    // TODO: button->notify();
}

Button::Button(int pin)
{
    esp32_log_level_set(buttonTag, ESP_LOG_INFO);

    ESP32_LOGD(buttonTag, "Button 构造器 pin: %d", pin);

    _pin = pin;
    // 按钮初始化
    // pinMode(pin, INPUT_PULLUP);

    switch (_pin)
    {
    case 34:
        _gpioNum = GPIO_NUM_34;
        break;
    case 35:
        _gpioNum = GPIO_NUM_35;
        break;
    case 39:
        _gpioNum = GPIO_NUM_39;
        break;
    default:
        ESP32_LOGE(buttonTag, "Invalid pin: %d", _pin);
        return;
    }
}

void Button::begin()
{
    ESP32_LOGD(buttonTag, "Button begin pin: %d", _pin);

    gpio_num_t gpioNum = getGPIO();

    gpio_isr_handler_add(gpioNum, gpio_isr, this);

    // 设置为下降沿触发
    gpio_set_intr_type(gpioNum, GPIO_INTR_ANYEDGE);
    gpio_intr_enable(gpioNum);

    ESP32_LOGD(buttonTag, "Button begin 结束 pin: %d", _pin);
    // attachInterrupt(digitalPinToInterrupt(_pin), handleButton3, FALLING);
}

void Button::notify()
{
    // unsigned long currentTime = millis();
    // if (currentTime - lastDebounceTime1 > DEBOUNCE_DELAY)
    // {
    // 中断上半部
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // vTaskNotifyGiveFromISR(button1TaskHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    // lastDebounceTime1 = currentTime;
    // }
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // if (WiFi.status() == WL_CONNECTED)
    // {
    //     HTTPClient http;
    //     http.begin("http://your-api-endpoint-1.com/api");
    //     int httpCode = http.GET();
    //     if (httpCode > 0)
    //     {
    //         String payload = http.getString();
    //         Serial.println(payload);
    //     }
    //     http.end();
    // }

    ESP32_LOGD(buttonTag, "notify");

    for (const auto &observer : observers)
    {
        ESP32_LOGD(buttonTag, "notify 运行观察者");
        observer();
    }
}

const char* buttonManageTag = "ButtonManager";

void ButtonManager::begin()
{
    ESP32_LOGD(buttonManageTag, "begin start");

    // Disable Bluetooth modem sleep
    esp_bt_controller_enable(ESP_BT_MODE_IDLE);
    esp_bt_sleep_disable();

    uint64_t INPUT_PIIN_MASK = 0;
    for (auto &button : buttons)
    {
        INPUT_PIIN_MASK |= (1ULL << button->getPin());
    }

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = INPUT_PIIN_MASK;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE; // 改为下拉
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;    // 禁用上拉

    gpio_config(&io_conf);

    // 添加错误处理
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK)
    {
        ESP32_LOGE(buttonManageTag, "ISR install failed with error: %d", err);
        return;
    }

    for (auto &button : buttons)
    {
        button->begin();
    }

    ESP32_LOGD(buttonManageTag, "begin done");
}