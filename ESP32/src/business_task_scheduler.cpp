#include "../include/business_task_scheduler.h"

TaskScheduler::TaskScheduler(Display& display) : _display(display) {}

void TaskScheduler::begin() {
    // 初始化定时任务列表
}

void TaskScheduler::addTask(const char* cronExpr, const char* imageUrl) {
    CronTask task;
    task.cronExpr = cronExpr;
    task.imageUrl = imageUrl;
    task.lastRun = 0;
    _tasks.push_back(task);
}

void TaskScheduler::checkTasks() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    for (auto& task : _tasks) {
        if (matchCron(task.cronExpr, &timeinfo)) {
            // 避免重复执行
            if (now - task.lastRun >= 60) {  // 最小间隔1分钟
                executeTask(task);
                task.lastRun = now;
            }
        }
    }
}

bool TaskScheduler::matchCron(const String& cronExpr, struct tm* timeinfo) {
    // 简单的cron匹配实现
    // 格式: 分 时 日 月 周
    // 例如: "30 8 * * *" 表示每天8:30
    
    String fields[5];
    int idx = 0;
    int lastIdx = 0;
    
    // 分解cron表达式
    for (int i = 0; i < cronExpr.length(); i++) {
        if (cronExpr[i] == ' ') {
            fields[idx++] = cronExpr.substring(lastIdx, i);
            lastIdx = i + 1;
        }
    }
    fields[idx] = cronExpr.substring(lastIdx);
    
    return matchField(fields[0], timeinfo->tm_min) &&
           matchField(fields[1], timeinfo->tm_hour) &&
           matchField(fields[2], timeinfo->tm_mday) &&
           matchField(fields[3], timeinfo->tm_mon + 1) &&
           matchField(fields[4], timeinfo->tm_wday);
}

bool TaskScheduler::matchField(const String& pattern, int value) {
    if (pattern == "*") return true;
    
    if (pattern.indexOf(',') != -1) {
        // 处理逗号分隔的多个值
        int idx = 0;
        int lastIdx = 0;
        while ((idx = pattern.indexOf(',', lastIdx)) != -1) {
            if (pattern.substring(lastIdx, idx).toInt() == value) {
                return true;
            }
            lastIdx = idx + 1;
        }
        return pattern.substring(lastIdx).toInt() == value;
    }
    
    return pattern.toInt() == value;
}

void TaskScheduler::executeTask(const CronTask& task) {
    // 下载并显示图片
    _display.downloadAndDisplayImage(task.imageUrl.c_str());
}