#ifndef ACTIVITY_H
#define ACTIVITY_H
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "OLEDDisplay.h"
#include "utils.h"

typedef struct {
    TaskHandle_t* task_handle_p;
    char* task_name;
    int last_time;
    void_callback_t* destroyed_callback;
    OLEDDisplay_t* oled_p;
    static_vars_t* static_vars;
    SemaphoreHandle_t print_mux;
} activity_args_t;

extern void activity_alarm_setting(void* arg);
extern void activity_alarm_ringtone_setting(void* arg);
extern void activity_alarm_ringing(void* arg);
extern void activity_clock_main(void* arg);

#endif