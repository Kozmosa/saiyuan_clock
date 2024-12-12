#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "OLEDDisplay.h"
#include "utils.h"

static const char *TAG = "activity.c";

typedef struct {
    TaskHandle_t* task_handle_p;
    char* task_name;
    int last_time;
    void_callback_t* destroyed_callback;
    OLEDDisplay_t* oled_p;
} activity_args_t;

extern void activity_alarm_setting(activity_args_t* args){

};
extern void activity_alarm_ringtone_setting(activity_args_t* args);
extern void activity_alarm_ringing(activity_args_t* args);
extern void activity_clock_main(activity_args_t* args){
//    OLEDDisplay_t *oled = OLEDDisplay_init(I2C_MASTER_NUM,0x78,I2C_MASTER_SDA_IO,I2C_MASTER_SCL_IO);
    OLEDDisplay_t *oled = args->oled_p;

    //    OLEDDisplay_fillRect(oled,7,7,18,18);
    //    OLEDDisplay_drawRect(oled,6,32,20,20);
    //    OLEDDisplay_display(oled);
    //    vTaskDelay(500 / portTICK_PERIOD_MS);

    // oled display infos
    refresh_time();

    while(1){
        refresh_time();

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
    xTaskCreate(test_screen_task, "test_screen_task_0", 1024 * 2, (void *)0, 10, NULL);
};