#include "../include/utils.h"

void espSleep(uint32_t seconds)
{
  // 睡眠前禁用GPIO39中断
  gpio_intr_disable(GPIO_NUM_39);

  esp_sleep_enable_timer_wakeup(seconds * 1000000ULL);
  // esp_deep_sleep_start();
  esp_light_sleep_start();

  // 唤醒后重新启用GPIO39中断
  gpio_intr_enable(GPIO_NUM_39);
}