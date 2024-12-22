//
// Created by 27866 on 2024/12/12.
//
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "utils.h"
#include "bsp_matrixkey.h"
#include "math.h"

#include <sys/time.h>

// common types definitions
typedef void (*void_callback_t)(void);

const char* TAG = "utils.c";

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);
extern bool check_alarm(alarm_t* alarm);
extern void alarm_ring(alarm_t* alarm, static_vars_t* static_vars_container);
extern bool check_alarms(static_vars_t* static_vars_container);
extern void button_app_main(void);
extern int button_key_check(void);
void echo_task(void *arg);
extern void alarm_task(void* pvParameters);
extern void uart_app_main(static_vars_t* static_vars_container);
extern void command_handler(char* command, static_vars_t* static_vars_container);
extern void convert_time_to_hhmmss(time_t time, int* hh, int* mm, int* ss);
extern time_t convert_hhmmss_to_timestamp(int hh, int mm, int ss);
extern alarm_t* find_nearest_alarm(static_vars_t* static_vars_container);

// time services
extern void refresh_time(char *time_s, char *date_s) {
    // get time from local clock
    time_t now;
    struct tm timeinfo;

    time(&now);
    //    time_t now = timestamp_buf;
    // 将时区设置为中国标准时间
    setenv("TZ", "CST-8", 1);
    tzset();

    localtime_r(&now, &timeinfo);

    strftime(time_s, sizeof(time_s)*32, "%H:%M:%S", &timeinfo);
    strftime(date_s, sizeof(date_s)*32, "%Y-%m-%d", &timeinfo);
}

extern void get_time(char *time_a, char *date_a){
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

extern bool check_alarm(alarm_t* alarm) {
    if(alarm != NULL){
        ESP_LOGI(TAG, "Detected alarm.");
        ESP_LOGI(TAG, "Alarm timestamp: %lld", alarm->alarm_timestamp);
        if(alarm->alarm_timestamp != 0) {
            ESP_LOGI(TAG, "Alarm time string: %s", alarm->alarm_time_string);
        }
    }
    time_t alarm_time = alarm->alarm_timestamp;
    time_t now;
    time(&now);
    ESP_LOGI(TAG, "Current timestamp: %lld", now);
    ESP_LOGI(TAG, "Alarm timestamp: %lld", alarm_time);
    ESP_LOGI(TAG, "Time difference: %lld", now - alarm_time);
    if(abs(now - alarm_time) < 60) {
        return true;
    } else {
        return false;
    }
}

extern void alarm_ring(alarm_t* alarm, static_vars_t* static_vars_container) {
    // alarm ring|E
    ESP_LOGI(TAG, "Alarm ring triggered.");
    while (1)
    {
        ESP_LOGI(TAG, "Alarm ringing blocked.");
        // check if alarm is confirmed  
        int current_key = button_key_check();
        if(current_key == BUTTON_KEY_CONFIRM) {
            ESP_LOGI(TAG, "Alarm ring confirmed.");
            break;
        }
        // do nothing but delay
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern bool check_alarms(static_vars_t* static_vars_container) {
    // check alarms
    alarm_t* alarms = static_vars_container->alarms;
    int alarm_count = *(static_vars_container->alarm_count);
    int alarm_capacity = *(static_vars_container->alarm_capacity);
    ESP_LOGI(TAG, "Checking alarms, current alarm count %d, alarm capacity %d", alarm_count, alarm_capacity);
    for(int i = 0; i < alarm_count; i++) {
        ESP_LOGI(TAG, "Checking alarm %d", i);
        if (check_alarm(&alarms[i]))
        {
            return true;
        }
    }
    return false;
}

// alarm setting services
extern void alarm_set(static_vars_t* static_vars_container, int hh, int mm, int ss) {
    // set alarm
    alarm_t* alarms = static_vars_container->alarms;
    int* alarm_count = static_vars_container->alarm_count;
    int* alarm_capacity = static_vars_container->alarm_capacity;
    if(*alarm_count < *alarm_capacity) {
        alarm_t new_alarm;
        time_t now;
        time(&now);
        new_alarm.alarm_timestamp = hh*3600 + mm*60 + ss + now;
        alarms[*alarm_count] = new_alarm;
        (*alarm_count)++;
        ESP_LOGI(TAG, "Alarm set, current alarm count %d, alarm capacity %d", *alarm_count, *alarm_capacity);
        ESP_LOGI(TAG, "Alarm set, new alarm timestamp: %lld, time converted to HH:MM:SS is %d:%d:%d", new_alarm.alarm_timestamp, hh, mm, ss);
    }
    else {
        ESP_LOGI(TAG, "Alarm capacity full.");
    }
}

extern alarm_t* find_nearest_alarm(static_vars_t* static_vars_container) {
    alarm_t* alarms = static_vars_container->alarms;
    int alarm_count = *(static_vars_container->alarm_count);
    int alarm_capacity = *(static_vars_container->alarm_capacity);
    time_t now;
    time(&now);
    time_t nearest_time = 0;

    alarm_t* nearest_alarm = NULL;
    for(int i = 0; i < alarm_count; i++) {
        if(alarms[i].alarm_timestamp - now < nearest_time) {
            nearest_time = alarms[i].alarm_timestamp - now;
            nearest_alarm = &alarms[i];
        }
    }
    return nearest_alarm;
}

// gpio button services
extern void button_app_main(void)
{
    uint8_t key_value = 0;

    ESP_LOGI(TAG, "Start checking button key...");
    MatrixKey_GPIO_Init();
    // while(1)
    // {
       key_value = Get_Key();
       if( key_value != 'n' )//如果有按键按下
       {
            ESP_LOGI(TAG, "KEY = %c", key_value);
       }
       key_value = 0;
       delay_1ms(500);
    // }
}

// enum BUTTON_KEY_FUNCTIONS {
//     BUTTON_KEY_MAIN_ACTIVITY = 0,
//     BUTTON_KEY_ALARM_ACTIVITY,
//     BUTTON_KEY_ALARM_RINGTONE_ACTIVITY,
//     BUTTON_KEY_ALARM_SETTING_ACTIVITY,
//     BUTTON_KEY_CLICK_UP,
//     BUTTON_KEY_CLICK_DOWN,
//     BUTTON_KEY_CLICK_LEFT,
//     BUTTON_KEY_CLICK_RIGHT,
//     BUTTON_KEY_CONFIRM,
//     BUTTON_KEY_CANCEL,
//     BUTTON_KEY_RESET,
//     BUTTON_KEY_BLANK
// };

extern int button_key_transform(uint8_t key_value) {
    switch (key_value)
    {
    case '1':
        return BUTTON_KEY_MAIN_ACTIVITY;
        break;
    case '2':
        return BUTTON_KEY_ALARM_ACTIVITY;
        break;
    case '3':
        return BUTTON_KEY_ALARM_RINGTONE_ACTIVITY;
        break;
    case '4':
        return BUTTON_KEY_ALARM_SETTING_ACTIVITY;
        break;
    case '5':
        return BUTTON_KEY_CLICK_UP;
        break;
    case '6':
        return BUTTON_KEY_CLICK_DOWN;
        break;
    case '7':
        return BUTTON_KEY_CLICK_LEFT;
        break;
    case '8':
        return BUTTON_KEY_CLICK_RIGHT;
        break;
    case '9':
        return BUTTON_KEY_CONFIRM;
        break;
    case '0':
        return BUTTON_KEY_CANCEL;
        break;
    case 'A':
        return BUTTON_KEY_RESET;
        break;
    case 'B':
        return BUTTON_KEY_LIGHT_ON;
        break;
    case 'C':
        return BUTTON_KEY_LIGHT_OFF;
        break;
    default:
        return BUTTON_KEY_MAIN_ACTIVITY;
        break;
    }
}

extern int button_key_check(void) {
    uint8_t key_value = 0;

    ESP_LOGI(TAG, "button_key_check() running...");
    MatrixKey_GPIO_Init();
    key_value = Get_Key();
    if( key_value != 'n' )//如果有按键按下
    {
        ESP_LOGI(TAG, "KEY = %c", key_value);
        return button_key_transform(key_value);
    }
    key_value = 0;
    return BUTTON_KEY_BLANK;
}

// uart services
#define ECHO_TEST_TXD  (GPIO_NUM_38)
#define ECHO_TEST_RXD  (GPIO_NUM_36)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

extern void clear_string(char* origin, char* dest, int len) {
    int i = 0;
    while(origin[i] != '\n' && i < len) {
        dest[i] = origin[i];
        i++;
    }
    dest[i] = '\0';
}

void echo_task(void *arg)
{
    static_vars_t* static_vars_container = (static_vars_t*)arg;
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    uint8_t *data_dest = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
        char* test_command = "test_command\n";
        // send commands
        if(*(static_vars_container->isCommand)) {
            ESP_LOGI(TAG, "Command detected.");
            // uart_write_bytes(UART_NUM_1, (const char*)static_vars_container->command_last->command_str, sizeof(char)*strlen(static_vars_container->command_last->command_str));
            if (*(static_vars_container->command_last_index) == 1)
            {
                uart_write_bytes(UART_NUM_1, "beep\n", sizeof(char)*sizeof("beep\n"));
            }
            else if(*(static_vars_container->command_last_index) == 2) 
            {
                uart_write_bytes(UART_NUM_1, "alarm\n", sizeof(char)*sizeof("alarm\n"));
            }
            else if(*(static_vars_container->command_last_index) == 3)
            {
                uart_write_bytes(UART_NUM_1, "lighton\n", sizeof(char)*sizeof("lighton\n"));
            }
            else if(*(static_vars_container->command_last_index) == 4)
            {
                uart_write_bytes(UART_NUM_1, "lightoff\n", sizeof(char)*sizeof("lightoff\n"));
            }
            else if(*(static_vars_container->command_last_index) == 5)
            {
                // data sync logics
                uart_write_bytes(UART_NUM_1, static_vars_container->uart_data_tx, sizeof(char)*strlen(static_vars_container->uart_data_tx));
                // waiting for sc32 to read data
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            // uart_write_bytes(UART_NUM_1, "beep\n", sizeof(char)*sizeof("beep\n"));
            *(static_vars_container->isCommand) = false;
        } else {
            uart_write_bytes(UART_NUM_1, (const char*)test_command, sizeof(char)*strlen(test_command));
        }
        // #ifdef UART_COMMAND_INTERACT_FLAG
        // // check if there is command waiting to be send
        // char* commands_list = static_vars_container->commands_list;
        // int commands_count = static_vars_container->commands_count;
        // if(commands_count) {
        //     for(int i = 0; i < commands_count; i++) {
        //         uart_write_bytes(UART_NUM_1, commands_list[i], sizeof(char)*32);
        //         ESP_LOGI(TAG, "UART Sent Command: %s", commands_list[i]);
        //         commands_count--;
        //         // clear comomands_list
        //         commands_list[i] = '';
        //     }
        // }
        // #endif
        ESP_LOGI(TAG, "Data length: %d", len);
        if(len) {
            clear_string((char*)data, (char*)data_dest, len);
            ESP_LOGI(TAG, "Data: %s", data_dest);
            ESP_LOGI(TAG, "Handling command...");
            command_handler((char*)data_dest, static_vars_container);
        }
        uart_flush(UART_NUM_1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

// we save a command list at here
// 1: refresh 

extern void command_handler(char* command, static_vars_t* static_vars_container) {
    // command handler
    if (strcmp(command, "test_command") == 0) {
        ESP_LOGI(TAG, "Test command handled.");
    }
    else if(strcmp(command, "datasync_start") == 0) {
        alarm_t* alarms = static_vars_container->alarms;
        int alarm_count = *(static_vars_container->alarm_count);
        char uart_data_tx_buffer[256] = {};
        for(int i = 0; i<alarm_count; i++) {
            ESP_LOGI(TAG, "Transferring Alarm %d: %lld", i, alarms[i].alarm_timestamp);
            if(i == alarm_count - 1) {
                strncat(uart_data_tx_buffer, 'N', sizeof(char));
                char timestamp_str[32] = {'\0'};
                itoa(alarms[i].alarm_timestamp, timestamp_str, 10);
                strncat(uart_data_tx_buffer, timestamp_str, sizeof(char)*16);
                strncat(uart_data_tx_buffer, 'E', sizeof(char));
                // sprintf(uart_data_tx_buffer, "%sN%dE\n", uart_data_tx_buffer, (int)alarms[i].alarm_timestamp);
                // deprecated
            } else {
                strncat(uart_data_tx_buffer, 'N', sizeof(char));
                char timestamp_str[32] = {'\0'};
                itoa(alarms[i].alarm_timestamp, timestamp_str, 10);
                strncat(uart_data_tx_buffer, timestamp_str, sizeof(char)*16);

                // sprintf(uart_data_tx_buffer, "%sN%d", uart_data_tx_buffer, (int)alarms[i].alarm_timestamp);
                // deprecated
            }
        }
        *(static_vars_container->isCommand) = true;
        *(static_vars_container->command_last_index) = 5;
        strcpy(static_vars_container->uart_data_tx, uart_data_tx_buffer);
        ESP_LOGI(TAG, "Transferring Data Sync Data: %s", static_vars_container->uart_data_tx);
    }
    else {
        ESP_LOGI(TAG, "Unknown command.");
    }
}

extern void alarm_task(void* pvParameters) {
    // #ifdef ALARM_DEFINED
    static_vars_t* static_vars_container = (static_vars_t*)pvParameters;
    alarm_t* alarms = static_vars_container->alarms;
    while (1)
    {
        /* code */
        ESP_LOGI(TAG, "Loop[Checking alarms.]");
        ESP_LOGI(TAG, "Loop[Current Alarm count: %d]", *(static_vars_container->alarm_count));
        for(int i = 0; i < *(static_vars_container->alarm_count); i++) {
            ESP_LOGI(TAG, "Checking alarm...");
            alarm_t* alarm = &alarms[i];
            if(check_alarm(alarm)) {
                ESP_LOGI(TAG, "Alarm ring");
                alarm_ring(alarm, static_vars_container);
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    // #endif
    vTaskDelete(NULL);
}

extern void alarm_app_main(static_vars_t* static_vars_container) {
    xTaskCreate(alarm_task, "alarm_task", 4096, static_vars_container, 10, NULL);
}

extern void uart_app_main(static_vars_t* static_vars_container)
{
    xTaskCreate(echo_task, "uart_echo_task", 4096, static_vars_container, 10, NULL);
}

extern void convert_time_to_hhmmss(time_t time, int* hh, int* mm, int* ss) {
    struct tm timeinfo;
    localtime_r(&time, &timeinfo);
    *hh = timeinfo.tm_hour;
    *mm = timeinfo.tm_min;
    *ss = timeinfo.tm_sec;
}

extern time_t convert_hhmmss_to_timestamp(int hh, int mm, int ss) {
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    timeinfo.tm_hour = hh;
    timeinfo.tm_min = mm;
    timeinfo.tm_sec = ss;

    return mktime(&timeinfo);
}