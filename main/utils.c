//
// Created by 27866 on 2024/12/12.
//
#include <stdio.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "utils.h"

#include <sys/time.h>

// common types definitions
typedef void (*void_callback_t)(void);

const char* TAG = "utils.c";

extern void refresh_time(char *time_s, char *date_s);
extern void get_time(char *time_a, char *date_a);
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

    strftime(time_s, sizeof(time_s)*32, "%H:%M:%S", &timeinfo);
    strftime(date_s, sizeof(date_s)*32, "%Y-%m-%d", &timeinfo);
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

bool check_alarm(alarm_t* alarm) {
    time_t alarm_time = alarm->alarm_timestamp;
    time_t now;
    time(&now);
    if(now - alarm_time < 10) {
        return true;
    } else {
        return false;
    }
}

void alarm_ring(void) {
    // alarm ring
}

void check_alarms(alarm_t* alarms, int alarm_count, int alarm_capacity) {
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


// uart services
#define ECHO_TEST_TXD  (GPIO_NUM_38)
#define ECHO_TEST_RXD  (GPIO_NUM_36)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

static void echo_task(void *arg)
{
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

    while (1) {
        // Read data from the UART
        uart_write_bytes(UART_NUM_1, "hello", sizeof(char)*5);
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
        ESP_LOGI(TAG, "Data: %s", (const char*)data);
        uart_flush(UART_NUM_1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern void uart_app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}