#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "OLEDDisplay.h"
#include "utils.h"
#include "activity.h"

static const char *TAG = "activity.c";

extern void activity_alarm_setting(void* args){

};
extern void activity_alarm_ringtone_setting(void* arg);
extern void activity_alarm_ringing(void* arg);
extern void activity_clock_main(void* arg){
  	activity_args_t* args = (activity_args_t*)arg;
    OLEDDisplay_t *oled = args->oled_p;

    char *time_s = args->static_vars->time_s;
    char *date_s = args->static_vars->date_s;
    SemaphoreHandle_t print_mux = args->print_mux;

    // oled display infos
    refresh_time(time_s, date_s);

    while(1){
        refresh_time(time_s, date_s);

        OLEDDisplay_clear(oled);

        OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
        OLEDDisplay_setFont(oled,ArialMT_Plain_24);
        OLEDDisplay_drawString(oled,64, 00, time_s);
        OLEDDisplay_display(oled);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        OLEDDisplay_setFont(oled,ArialMT_Plain_16);
        OLEDDisplay_drawString(oled,64, 25, date_s);
        OLEDDisplay_display(oled);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        OLEDDisplay_setFont(oled,ArialMT_Plain_10);
        OLEDDisplay_drawString(oled,64, 40, "637 Clock");
        OLEDDisplay_display(oled);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        vTaskDelay(680 / portTICK_PERIOD_MS);

        // oled display infos
        OLEDDisplay_clear(oled);

        OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
        OLEDDisplay_setFont(oled,ArialMT_Plain_24);
        OLEDDisplay_drawString(oled,64, 00, "HACK!!");
        OLEDDisplay_display(oled);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vSemaphoreDelete(print_mux);
    vTaskDelete(NULL);
    xTaskCreate(activity_alarm_setting, "test_screen_task_0", 1024 * 2, (void *)0, 10, NULL);
};