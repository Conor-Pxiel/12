#ifndef __PWM_H_
#define __PWM_H_

#include <stdint.h>

#define GPIO_AIN1                18   // AIN1 连接 ESP32-S3 的 GPIO
#define GPIO_AIN2                19   // AIN2 连接 ESP32-S3 的 GPIO
#define GPIO_PWMA                9   // PWMA 连接 ESP32-S3 的 GPIO
#define GPIO_BIN1                20   // BIN1 连接 ESP32-S3 的 GPIO
#define GPIO_BIN2                21   // BIN2 连接 ESP32-S3 的 GPIO
#define GPIO_PWMB                10   // PWMB 连接 ESP32-S3 的 GPIO
#define GPIO_SML                 12
#define GPIO_SMR                 17

#define PWMA_TIMER               LEDC_TIMER_0          // 使用LEDC定时器0
#define PWMA_CHANNEL             LEDC_CHANNEL_0        // 使用LEDC通道0
#define PWMA_MODE                LEDC_LOW_SPEED_MODE   // 低速模式（对于电机控制足够）.... , , *96
#define PWMB_TIMER               LEDC_TIMER_0          // 使用LEDC定时器0
#define PWMB_CHANNEL             LEDC_CHANNEL_3        // 使用LEDC通道3
#define PWMB_MODE                LEDC_LOW_SPEED_MODE   // 低速模式（对于电机控制足够）
#define LEDC_DUTY_RES            LEDC_TIMER_10_BIT     // PWM占空比分辨率设为13位 (0-8191)
#define LEDC_FREQUENCY           (1000)                // PWM频率设为5kHz

void pwm_init(void);
void duty_set(uint16_t duty);
void duty_set2(uint16_t duty);

#endif
