#include "pwm.h"
#include "driver/ledc.h"

void pwm_init(void)
{
    // 1. 配置AIN1和AIN2为输出引脚，用于控制方向
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_AIN1) | (1ULL << GPIO_AIN2) | (1ULL << GPIO_BIN1) | (1ULL << GPIO_BIN2), // 要配置的引脚
        .mode = GPIO_MODE_OUTPUT,          // 设置为输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE, // 不上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // 不下拉
        .intr_type = GPIO_INTR_DISABLE     // 禁用中断
    };
    gpio_config(&io_conf); // 应用配置

    // 2. 初始化并配置PWM定时器
    ledc_timer_config_t ledctimer_structure = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 50,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
    };
    ledc_timer_config(&ledctimer_structure);

    ledc_timer_config_t ledctimer_structure2 = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 50,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
    };
    ledc_timer_config(&ledctimer_structure2);

    ledc_channel_config_t   ledcchannel_structure = {
        .channel = LEDC_CHANNEL_1,
        .duty = 25,
        .flags.output_invert = 0,
        .gpio_num = GPIO_SML,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_1,
    };
    ledc_channel_config(&ledcchannel_structure);

    ledc_channel_config_t   ledcchannel_structure2 = {
        .channel = LEDC_CHANNEL_2,
        .duty = 25,
        .flags.output_invert = 0,
        .gpio_num = GPIO_SMR,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_1,
    };
    ledc_channel_config(&ledcchannel_structure2);

    // 3. 初始化并配置PWM通道，绑定到指定GPIO
    ledc_channel_config_t ledc_channel = {
        .channel = PWMA_CHANNEL,           // 使用通道0
        .duty = 0,                         // 初始占空比为0（电机不转）
        .flags.output_invert = 0,
        .gpio_num = GPIO_PWMA,             // 输出到GPIO
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,    // 禁用中断
        .speed_mode = PWMA_MODE,           // 通道模式   
        .timer_sel = PWMA_TIMER,           // 绑定到定时器
    };
    ledc_channel_config(&ledc_channel); // 应用通道配置

    ledc_channel_config_t ledc_channel_B = {
        .channel = PWMB_CHANNEL,           // 使用通道3
        .duty = 0,                         // 初始占空比为0（电机不转）
        .flags.output_invert = 0,
        .gpio_num = GPIO_PWMB,             // 输出到GPIO
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,    // 禁用中断
        .speed_mode = PWMB_MODE,           // 通道模式   
        .timer_sel = PWMB_TIMER,           // 绑定到定时器 
    };
    ledc_channel_config(&ledc_channel_B); // 应用通道配置
}

void duty_set(uint16_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty); 
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);        
}

void duty_set2(uint16_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, duty); 
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);        
}