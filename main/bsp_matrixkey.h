/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：club.szlcsc.com
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-01-09     LCKFB-lp    first version
 */

#ifndef _BSP_MATRIXKEY_H_
#define _BSP_MATRIXKEY_H_

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "rom/ets_sys.h"
#include "esp_system.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "nvs_flash.h"

#define GPIO_IN1    9
#define GPIO_IN2    10
#define GPIO_IN3    11
#define GPIO_IN4    12
#define GPIO_IN5    39
#define GPIO_IN6    40
#define GPIO_IN7    41
#define GPIO_IN8    42

uint8_t Get_Key(void);
void MatrixKey_GPIO_Init(void);
uint8_t key_scan(void);

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);
void delay_1us(unsigned int us);
void delay_1ms(unsigned int ms);

#endif