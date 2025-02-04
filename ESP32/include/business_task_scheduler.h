#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <Arduino.h>
#include <vector>
#include "esp32_display.h"
#include "esp32_time.h"

struct CronTask {
    String cronExpr;      // cron表达式
    String imageUrl;      // 显示图片URL
    unsigned long lastRun; // 上次运行时间
};

class TaskScheduler {
public:
    TaskScheduler(Display& display);
    void begin();
    void addTask(const char* cronExpr, const char* imageUrl);
    void checkTasks();
    
private:
    Display& _display;
    std::vector<CronTask> _tasks;
    
    bool matchCron(const String& cronExpr, struct tm* timeinfo);
    bool matchField(const String& pattern, int value);
    void executeTask(const CronTask& task);
};

#endif