#include "motor2.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

void A0_Control(uint8_t dir_L, uint32_t speed)
{
    // 1. 设置方向
    if(dir_L == 1) // 正转
    {
        gpio_set_level(GPIO_AIN1, 1); // AIN1 = 0
        gpio_set_level(GPIO_AIN2, 0); // AIN2 = 1
    }
    else // 反转
    {
        gpio_set_level(GPIO_AIN1, 0); // AIN1 = 1
        gpio_set_level(GPIO_AIN2, 1); // AIN2 = 0
    }

    if(speed > ((1 << LEDC_DUTY_RES) - 1)) {
        speed = ((1 << LEDC_DUTY_RES) - 1); // 防止越界
    }
    ledc_set_duty(PWMA_MODE, PWMA_CHANNEL, speed); // 设置占空比
    ledc_update_duty(PWMA_MODE, PWMA_CHANNEL);     // 立即更新输出

}

void A1_Control(uint8_t dir_R, uint32_t speed)
{
    // 1. 设置方向
    if(dir_R == 1) // 正转
    {
        gpio_set_level(GPIO_BIN1, 1);
        gpio_set_level(GPIO_BIN2, 0);
    }
    else // 反转
    {
        gpio_set_level(GPIO_BIN1, 0);
        gpio_set_level(GPIO_BIN1, 1);
    }

    if(speed > ((1 << LEDC_DUTY_RES) - 1)) {
        speed = ((1 << LEDC_DUTY_RES) - 1); // 防止越界
    }
    ledc_set_duty(PWMB_MODE, PWMB_CHANNEL, speed); // 设置占空比
    ledc_update_duty(PWMB_MODE, PWMB_CHANNEL);     // 立即更新输出

}