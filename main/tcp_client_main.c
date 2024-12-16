/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include <sys/time.h>
#include <stdlib.h>
#include "string.h"
#include <sys/unistd.h>
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// clock headers
#include "utils.h"
#include "activity.h"

#define CONFIG_EXAMPLE_CONNECT_WIFI 1
#define CONFIG_EXAMPLE_WIFI_SSID_PWD_FROM_STDIN 0
#define CONFIG_EXAMPLE_WIFI_SSID "YangPC"
#define CONFIG_EXAMPLE_WIFI_PASSWORD "88888888"
// i2c screen driver
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "OLEDDisplay.h"
static const char *TAG = "oled-example";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)
#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */

SemaphoreHandle_t print_mux = NULL;

// static variables definitions
#define ALARM_DEFINED 1
#define STATIC_VARS_CONTAINER static_vars_global

static char time_s[32] = {};
static char date_s[32] = {};
static int alarm_count = 0;
static int alarm_capacity = 8;
static alarm_t alarms[8];

static static_vars_t static_vars_global;

static bool switch_signal = false;
static int before_level = 0;
static int current_activity = 0;

// keys configs
void void_callback_example(void) {
  // void callback example
}

#define GPIO_INPUT_IO_0    14
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)  // 配置GPIO_IN位寄存器

extern void test_screen_task(void *args);
extern void i2c_test_task(void *args);

void switch_activity(void) {
    if(current_activity == 0) {
        vTaskDelete(NULL);
        xTaskCreate(test_screen_task, "test_screen_task_0", 1024 * 2, (void *)0, 10, NULL);
    } else {
        vTaskDelete(NULL);
        xTaskCreate(i2c_test_task, "i2c_test_task_0", 1024 * 2, (void *)0, 10, NULL);
    }
}

void gpio_init(void)
{
    gpio_config_t io_conf;  // 定义一个gpio_config类型的结构体，下面的都算对其进行的配置

    io_conf.intr_type = GPIO_INTR_DISABLE;  // 禁止中断
    io_conf.mode = GPIO_MODE_INPUT;             // 选择输入模式
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;  // 配置GPIO_IN寄存器
    io_conf.pull_down_en = 0;                   // 禁止下拉
    io_conf.pull_up_en = 0;                     // 禁止上拉

    gpio_config(&io_conf);                      // 最后配置使能
}

void check_gpio_key(void_callback_t callback) {
    int level = gpio_get_level(GPIO_INPUT_IO_0);            // 读取这个GPIO电平
    if(level) {
        printf("GPIO KEY UP\n");
    } else {
        printf("GPIO KEY DOWN\n");
    }
    if(level != before_level) {
        (*callback)();
    }
}

extern void refresh_time(char *time_s, char *date_s);

void i2c_app_main(static_vars_t* static_vars_container)
{
    print_mux = xSemaphoreCreateMutex();
    ESP_LOGI(TAG,"Running");
    TaskHandle_t current_task = NULL;

    activity_args_t args;
    OLEDDisplay_t *oled = OLEDDisplay_init(I2C_MASTER_NUM,0x78,I2C_MASTER_SDA_IO,I2C_MASTER_SCL_IO);
    void_callback_t void_callback_example_p = &void_callback_example;

    alarm_t sample_alarm;
    sample_alarm.alarm_timestamp = 0;
    alarms[0] = sample_alarm;
    alarm_capacity = 8;
    alarm_count = 1;

    args.oled_p = oled;
    args.task_name = "test_screen_task_0";
    args.task_handle_p = &current_task;
    args.last_time = 0;
    args.destroyed_callback = &void_callback_example_p;
    args.static_vars = static_vars_container;
    args.print_mux = print_mux;

    xTaskCreate(activity_clock_main, "i2c_test_task_0", 1024 * 2, &args, 10, &current_task);
}

extern void tcp_client(void);

static int timestamp_buf = 0;

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    tcp_client();

	static_vars_global.time_s = &time_s;
	static_vars_global.date_s = &date_s;
	static_vars_global.alarm_capacity = &alarm_capacity;
	static_vars_global.alarm_count = &alarm_count;
	static_vars_global.alarms = &alarms;

    i2c_app_main(&static_vars_global);

    uart_app_main(&static_vars_global);

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG,"main loop.");
    }
}
