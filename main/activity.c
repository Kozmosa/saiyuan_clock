#include "stdio.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "OLEDDisplay.h"
#include "utils.h"
#include "activity.h"
#include "unistd.h"
#include "string.h"
#include <time.h>

static const char *TAG = "activity.c";

// temporarily using drawing functions as subtitute for activity

void draw_alarm_setting(OLEDDisplay_t* oled) {
    OLEDDisplay_clear(oled);

    OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
    OLEDDisplay_setFont(oled,ArialMT_Plain_24);
    OLEDDisplay_drawString(oled,64, 00, "ALARM!!");
    OLEDDisplay_display(oled);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

extern void activity_alarm_setting(void* arg){
    ESP_LOGI(TAG,"Switched to activity alarm_setting");
  	activity_args_t* args = (activity_args_t*)arg;
    static_vars_t* static_vars_container = args->static_vars;
    OLEDDisplay_t *oled = args->oled_p;

    SemaphoreHandle_t print_mux = args->print_mux;

    time_t now;
    time(&now);
    // convert timestamp to hh mm ss int variable
    int alarm_hh = 0;
    int alarm_mm = 0;
    int alarm_ss = 0;
    convert_time_to_hhmmss(now, &alarm_hh, &alarm_mm, &alarm_ss);

    while(1){
        // oled display infos
        char time_str[128];
        sprintf(time_str, "%d:%d:%d", alarm_hh, alarm_mm, alarm_ss);

        OLEDDisplay_clear(oled);

        OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
        OLEDDisplay_setFont(oled,ArialMT_Plain_16);
        OLEDDisplay_drawString(oled,64, 00, "Setting alarm");
        OLEDDisplay_display(oled);

        OLEDDisplay_setFont(oled,ArialMT_Plain_10);
        OLEDDisplay_drawString(oled,64, 17, "Alarm time:");
        OLEDDisplay_display(oled);

        OLEDDisplay_setFont(oled,ArialMT_Plain_10);
        OLEDDisplay_drawString(oled,64, 25, time_str);
        OLEDDisplay_display(oled);
        ESP_LOGI(TAG,"Draw Time str: %s", time_str);

        int current_key = button_key_check();
        if(current_key == BUTTON_KEY_CANCEL) {
            break;
        } else if(current_key == BUTTON_KEY_CONFIRM) {
            OLEDDisplay_clear(oled);
            OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
            OLEDDisplay_setFont(oled,ArialMT_Plain_16);
            OLEDDisplay_drawString(oled,64, 00, "Confirm alarm?");
            OLEDDisplay_display(oled);

            current_key = button_key_check();
            if(current_key == BUTTON_KEY_CONFIRM) {
                alarm_t set_alarm;
                strcpy(set_alarm.alarm_time_string, time_str);
                set_alarm.alarm_timestamp = convert_hhmmss_to_timestamp(alarm_hh, alarm_mm, alarm_ss);
                if(*(static_vars_container->alarm_count) < *(static_vars_container->alarm_capacity)) {
                    static_vars_container->alarms[*(static_vars_container->alarm_count)].alarm_timestamp = set_alarm.alarm_timestamp;
                    strcpy(static_vars_container->alarms[*(static_vars_container->alarm_count)].alarm_time_string, set_alarm.alarm_time_string);
                    (*(static_vars_container->alarm_count))++;
                    ESP_LOGI(TAG, "Alarm set finished, time_str: %s", set_alarm.alarm_time_string);
                    current_key = BUTTON_KEY_MAIN_ACTIVITY;
                } else {
                    ESP_LOGI(TAG, "Alarm overwhelmed capacity.");
                    current_key = BUTTON_KEY_MAIN_ACTIVITY;
                }
                break;
            }
        } else if(current_key >= BUTTON_KEY_CLICK_UP && current_key <= BUTTON_KEY_CLICK_RIGHT) {
            switch (current_key)
            {
            case BUTTON_KEY_CLICK_UP:
                alarm_hh++;
                if(alarm_hh >= 24) {
                    alarm_hh = 0;
                }
                continue;
            case BUTTON_KEY_CLICK_DOWN:
                alarm_hh--;
                if(alarm_hh < 0) {
                    alarm_hh = 23;
                }
                continue;
            case BUTTON_KEY_CLICK_LEFT:
                alarm_mm--;
                if(alarm_mm < 0) {
                    alarm_mm = 59;
                }
                continue;
            case BUTTON_KEY_CLICK_RIGHT:
                alarm_mm++;
                if(alarm_mm >= 60) {
                    alarm_mm = 0;
                }
                continue;
            default:
                continue;
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // do nothing since it is not a freertos task

};

extern void activity_alarm_ringtone_setting(void* arg) {
    // alarm ringtone setting activity
    ESP_LOGI(TAG,"Switched to activity Alarm Ringtone Setting.");
  	activity_args_t* args = (activity_args_t*)arg;
    OLEDDisplay_t *oled = args->oled_p;

    char *time_s = *(args->static_vars->time_s);
    char *date_s = *(args->static_vars->date_s);
    SemaphoreHandle_t print_mux = args->print_mux;

    refresh_time(time_s, date_s);
    OLEDDisplay_clear(oled);

    OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
    OLEDDisplay_setFont(oled,ArialMT_Plain_16);
    OLEDDisplay_drawString(oled,64, 00, "Setting ringtone");
    OLEDDisplay_display(oled);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG,"ALARM_RINGTONE_SETTING activity user interface has been drawn.");
};

// usually arg is presented as a pointer to activity_args_t
// activity_args_t contains a pointer to the oled display, time and date strings, and a semaphore for print mux

extern void activity_alarm_ringing(void* arg) {
    // alarm ringing activity
    ESP_LOGI(TAG,"Switched to activity Alarm Ringing");
  	activity_args_t* args = (activity_args_t*)arg;
    OLEDDisplay_t *oled = args->oled_p;

    char *time_s = *(args->static_vars->time_s);
    char *date_s = *(args->static_vars->date_s);
    SemaphoreHandle_t print_mux = args->print_mux;

    refresh_time(time_s, date_s);
    OLEDDisplay_clear(oled);

    OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
    OLEDDisplay_setFont(oled,ArialMT_Plain_24);
    OLEDDisplay_drawString(oled,64, 00, "ALARM!!");
    OLEDDisplay_display(oled);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG,"ALARM_RINGING activity has been drawn.");

    while(1) {
        int current_key = button_key_check();
        ESP_LOGI(TAG,"ALARM_RINGING activity BLOCKED, waiting for key down.");
        // real ringing
        *(args->static_vars->isCommand) = true;
        command_t beep = {"alarm", 5};
        strcpy(args->static_vars->command_last->command_str, "alarm");
        args->static_vars->command_last->length = beep.length;
        *(args->static_vars->command_last_index) = 2;
        *(args->static_vars->isCommand) = true;
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
        
        if(current_key == BUTTON_KEY_CONFIRM) {
            break;
        } else if(current_key == BUTTON_KEY_CANCEL) {
            // implementation of skip clock function
            OLEDDisplay_clear(oled);
            OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
            OLEDDisplay_setFont(oled,ArialMT_Plain_16);
            OLEDDisplay_drawString(oled,64, 00, "Skip for 5min");
            OLEDDisplay_display(oled);
            
            // skip for 5 minutes
            alarm_t* nearest_alarm = find_nearest_alarm(args->static_vars);
            nearest_alarm->alarm_timestamp += 300;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
};

extern void activity_clock_main(void* arg){
  	ESP_LOGI(TAG,"Switched to activity main");
  	activity_args_t* args = (activity_args_t*)arg;
    OLEDDisplay_t *oled = args->oled_p;

    char *time_s = *(args->static_vars->time_s);
    char *date_s = *(args->static_vars->date_s);
    SemaphoreHandle_t print_mux = args->print_mux;

    // oled display infos
    refresh_time(time_s, date_s);

    while(1){
        refresh_time(time_s, date_s);

        OLEDDisplay_clear(oled);

        OLEDDisplay_setTextAlignment(oled,TEXT_ALIGN_CENTER);
        OLEDDisplay_setFont(oled,ArialMT_Plain_24);
        OLEDDisplay_drawString(oled,64, 00, time_s);
        // OLEDDisplay_display(oled);
        ESP_LOGI(TAG,"Draw Time: %s", time_s);

        OLEDDisplay_setFont(oled,ArialMT_Plain_16);
        OLEDDisplay_drawString(oled,64, 25, date_s);
        // OLEDDisplay_display(oled);
        ESP_LOGI(TAG,"Draw Date: %s", date_s);

        OLEDDisplay_setFont(oled,ArialMT_Plain_10);
        OLEDDisplay_drawString(oled,64, 40, "637 Clock");
        OLEDDisplay_display(oled);
        ESP_LOGI(TAG,"Draw App Name: 637 Clock");

        // check if it is triggered to switch activity
        enum BUTTON_KEY_FUNCTIONS key = button_key_check();
        switch (key)
        {
        case BUTTON_KEY_ALARM_ACTIVITY:
            activity_alarm_ringing(args);
            break;

        case BUTTON_KEY_ALARM_RINGTONE_ACTIVITY:
            ESP_LOGI(TAG,"Beep command has been triggered.");
            command_t beep = {"beep", 4};
            strcpy(args->static_vars->command_last->command_str, "beep");
            args->static_vars->command_last->length = beep.length;
            *(args->static_vars->command_last_index) = 1;
            *(args->static_vars->isCommand) = true;
            break;

        case BUTTON_KEY_ALARM_SETTING_ACTIVITY:
            ESP_LOGI(TAG, "Alarm setting command has been triggered.");
            activity_alarm_setting(args);

        case BUTTON_KEY_LIGHT_ON:
            ESP_LOGI(TAG, "Light on command has been triggered.");
            *(args->static_vars->command_last_index) = 3;
            *(args->static_vars->isCommand) = true;
            break;

        case BUTTON_KEY_LIGHT_OFF:
            ESP_LOGI(TAG, "Light off command has been triggered.");
            *(args->static_vars->command_last_index) = 4;
            *(args->static_vars->isCommand) = true;
            break;

        default:
            break;
        }

        // check alarms
        bool isRinging = check_alarms(args->static_vars);
        if(isRinging) {
            activity_alarm_ringing(args);
        }

        vTaskDelay(900 / portTICK_PERIOD_MS);
        // usleep(1000000);
    }
    ESP_LOGI(TAG,"Activity main has been drawn. Then delete.");

    vSemaphoreDelete(print_mux);
    // vTaskDelete(NULL);
};