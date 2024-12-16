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

#include <sys/time.h>

// common types definitions
typedef void (*void_callback_t)(void);

const char* TAG = "utils.c";

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);
extern bool check_alarm(alarm_t* alarm);
extern void alarm_ring(void);
extern void check_alarms(static_vars_t* static_vars_container);
extern void button_app_main(void);
static void echo_task(void *arg);
extern void alarm_task(void* pvParameters);
extern void uart_app_main(static_vars_t* static_vars_container);
extern void command_handler(char* command, static_vars_t* static_vars_container);

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
        ESP_LOGI(TAG, "Alarm time string: %s", alarm->alarm_time_string);
    }
    time_t alarm_time = alarm->alarm_timestamp;
    time_t now;
    time(&now);
    if(now - alarm_time < 10) {
        return true;
    } else {
        return false;
    }
}

extern void alarm_ring(void) {
    // alarm ring
}

extern void check_alarms(static_vars_t* static_vars_container) {
    int alarm_count = *(static_vars_container->alarm_count);
    int alarm_capacity = *(static_vars_container->alarm_capacity);
    alarm_t* alarms = static_vars_container->alarms;

    for(int i = 0; i < alarm_count; i++) {
        bool alarm_status = check_alarm(&alarms[i]);
        ESP_LOGI(TAG, "Alarm status: %d", alarm_status);
        if(alarm_status) {
            ESP_LOGI(TAG, "Alarm alarm");
            alarm_ring();
        }
    }
}

// gpio button services
extern void button_app_main(void)
{
    uint8_t key_value = 0;

    printf("start\r\n");
    MatrixKey_GPIO_Init();
    while(1)
    {
       key_value = Get_Key();
       if( key_value != 'n' )//如果有按键按下
       {
            printf("KEY = %c\r\n",key_value);
       }
       key_value = 0;
       delay_1ms(500);
    }
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

static void echo_task(void *arg)
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
        char* test_command = "hello_command\n";
        uart_write_bytes(UART_NUM_1, (const char*)test_command, sizeof(char)*strlen(test_command));
        #ifdef UART_COMMAND_INTERACT_FLAG
        // check if there is command waiting to be send
        char* commands_list = static_vars_container->commands_list;
        int commands_count = static_vars_container->commands_count;
        if(commands_count) {
            for(int i = 0; i < commands_count; i++) {
                uart_write_bytes(UART_NUM_1, commands_list[i], sizeof(char)*32);
                ESP_LOGI(TAG, "UART Sent Command: %s", commands_list[i]);
                commands_count--;
                // clear comomands_list
                commands_list[i] = '';
            }
        }
        #endif
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
}

// we save a command list at here
// 1: refresh 

extern void command_handler(char* command, static_vars_t* static_vars_container) {
    // command handler
    if (strcmp(command, "test_command") == 0) {
        ESP_LOGI(TAG, "Test command handled.");
    }
    else if(strcmp(command, "check_alarms") == 0) {
        ESP_LOGI(TAG, "Checking alarm...");
        check_alarms(&static_vars_container);
    }
    else {
        ESP_LOGI(TAG, "Unknown command.");
    }
}

extern void alarm_task(void* pvParameters) {
    #ifdef ALARM_DEFINED
    static_vars_t* static_vars_container = (static_vars_t*)pvParameters;
    alarm_t* alarms = static_vars_container->alarms;
    for(int i = 0; i < *(static_vars_container->alarm_count); i++) {
        ESP_LOGI(TAG, "Checking alarm...");
        alarm_t* alarm = &alarms[i];
        if(check_alarm(alarm)) {
            ESP_LOGI(TAG, "Alarm ring");
        }
    }
    #endif
}

extern void alarm_app_main(static_vars_t* static_vars_container) {
    xTaskCreate(alarm_task, "alarm_task", 1024, static_vars_container, 10, NULL);
}

extern void uart_app_main(static_vars_t* static_vars_container)
{
    xTaskCreate(echo_task, "uart_echo_task", 1024, static_vars_container, 10, NULL);
}