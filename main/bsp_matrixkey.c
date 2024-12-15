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

#include "bsp_matrixkey.h"
#include "stdio.h"

//行
uint32_t gpio_row[4] = {GPIO_IN1,GPIO_IN2,GPIO_IN3,GPIO_IN4};

//列
uint32_t gpio_col[4] = {GPIO_IN5,GPIO_IN6,GPIO_IN7,GPIO_IN8};

void delay_ms(unsigned int ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}
void delay_us(unsigned int us)
{
    ets_delay_us(us);
}
void delay_1ms(unsigned int ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}
void delay_1us(unsigned int us)
{
    ets_delay_us(us);
}
/******************************************************************
 * 函 数 名 称：MatrixKey_GPIO_Init
 * 函 数 说 明：对矩阵按键的初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：必须上拉
******************************************************************/
void MatrixKey_GPIO_Init(void)
{
    gpio_config_t GG1_config = {
        .pin_bit_mask = (1ULL<<GPIO_IN1)|(1ULL<<GPIO_IN2)|(1ULL<<GPIO_IN3)|(1ULL<<GPIO_IN4),    //配置引脚
        .mode =GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,        //使能上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE,    //不使能下拉
        .intr_type = GPIO_INTR_DISABLE            //不使能引脚中断
    };
    gpio_config(&GG1_config);

    gpio_config_t GG2_config = {
        .pin_bit_mask = (1ULL<<GPIO_IN5)|(1ULL<<GPIO_IN6)|(1ULL<<GPIO_IN7)|(1ULL<<GPIO_IN8),    //配置引脚
        .mode =GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,        //使能上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE,    //不使能下拉
        .intr_type = GPIO_INTR_DISABLE            //不使能引脚中断
    };
    gpio_config(&GG2_config);
}

/******************************************************************
 * 函 数 名 称：key_scan
 * 函 数 说 明：矩阵按键扫描
 * 函 数 形 参：无
 * 函 数 返 回：0=没有按键按下     1~16=对应按键按下
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t key_scan(void)
{
    uint8_t i, j, key_val = 0;

    for (i = 0; i < 4; i++)
    {
        // 选中第i行（低电平有效）
        gpio_set_level(gpio_row[i], 0);

        // 检测列的状态，确定按键是否被按下
        for (j = 0; j < 4; j++)
        {
            //如果该列被拉低
            if (!gpio_get_level(gpio_col[j]))
            {
                // 计算按键编号
                key_val = i * 4 + j + 1;
                break;
            }
        }

        // 恢复第i行的输出状态
        gpio_set_level(gpio_row[i], 1);

        // 如果按键被按下，则直接退出扫描
        if (key_val) break;
    }
    // 返回按键编号，0表示无按键被按下
    return key_val;
}

/******************************************************************
 * 函 数 名 称：Get_Key
 * 函 数 说 明：得到按键的具体字符
 * 函 数 形 参：无
 * 函 数 返 回：按键的具体值
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t Get_Key(void)
{
    uint8_t value = key_scan();
    switch (value)
    {
        case 1:  value = '1';   break;
        case 2:  value = '2';   break;
        case 3:  value = '3';   break;
        case 4:  value = 'A';   break;
        case 5:  value = '4';   break;
        case 6:  value = '5';   break;
        case 7:  value = '6';   break;
        case 8:  value = 'B';   break;
        case 9:  value = '7';   break;
        case 10: value = '8';   break;
        case 11: value = '9';   break;
        case 12: value = 'C';   break;
        case 13: value = '*';   break;
        case 14: value = '0';   break;
        case 15: value = '#';   break;
        case 16: value = 'D';   break;

        default: value = 'n';   break;
    }

    return value;
}