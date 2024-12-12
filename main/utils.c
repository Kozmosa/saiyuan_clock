//
// Created by 27866 on 2024/12/12.
//
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <sys/time.h>

// common types definitions
typedef void (*void_callback_t)(void);

// time services
void refresh_time(char *time_s, char *date_s) {
    // get time from local clock
    time_t now;
    struct tm timeinfo;

    time(&now);
    //    time_t now = timestamp_buf;
    // 将时区设置为中国标准时间
    setenv("TZ", "CST-8", 1);
    tzset();

    localtime_r(&now, &timeinfo);

    strftime(time_s, sizeof(time_s), "%H:%M:%S", &timeinfo);
    strftime(date_s, sizeof(date_s), "%Y-%m-%d", &timeinfo);
}

void get_time(char *time_a, char *date_a){
    time_t now;
    struct tm timeinfo;

    time(&now);
    // 将时区设置为中国标准时间
    setenv("TZ", "CST-8", 1);
    tzset();

    localtime_r(&now, &timeinfo);

    char time_b[64] = {};
    char date_b[64] = {};
    strftime(time_b, sizeof(time_b), "%H:%M:%S", &timeinfo);
    strftime(date_b, sizeof(date_b), "%Y-%m-%d", &timeinfo);
}

// uart services