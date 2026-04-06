#include "motor.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_task.h"


#define PWM_FREQ_HZ     1000
#define PWM_RESOLUTION  LEDC_TIMER_8_BIT
#define LEDC_MODE       LEDC_LOW_SPEED_MODE

MotorDriver::MotorDriver(int ain1, int ain2, int bin1, int bin2, int pwma, int pwmb)
    : _ain1(ain1), _ain2(ain2), _bin1(bin1), _bin2(bin2),
      _pwma(pwma), _pwmb(pwmb),
      _pwm_ch_a(LEDC_CHANNEL_0), _pwm_ch_b(LEDC_CHANNEL_1)
{}

void MotorDriver::init()
{
    ESP_LOGI("Motor", "初始化TB6612: AIN1=%d, AIN2=%d, BIN1=%d, BIN2=%d, PWMA=%d, PWMB=%d", 
             _ain1, _ain2, _bin1, _bin2, _pwma, _pwmb);
    
    // 配置方向引脚为普通GPIO输出
    gpio_set_direction((gpio_num_t)_ain1, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_ain2, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_bin1, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_bin2, GPIO_MODE_OUTPUT);
    
    // 初始化PWM定时器
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode       = LEDC_MODE;
    timer_conf.timer_num        = LEDC_TIMER_0;
    timer_conf.duty_resolution  = PWM_RESOLUTION;
    timer_conf.freq_hz          = PWM_FREQ_HZ;
    // timer_conf.clk_cfg          = LEDC_AUTO_CLK;
    timer_conf.clk_cfg          = LEDC_USE_XTAL_CLK;
    ledc_timer_config(&timer_conf);
    
    // 配置PWM通道
    ledc_channel_config_t ch_conf = {};
    ch_conf.speed_mode     = LEDC_MODE;
    ch_conf.timer_sel      = LEDC_TIMER_0;
    ch_conf.intr_type      = LEDC_INTR_DISABLE;
    ch_conf.duty           = 0;
    ch_conf.hpoint         = 0;
    
    // PWMA
    ch_conf.gpio_num   = _pwma;
    ch_conf.channel    = _pwm_ch_a;
    ledc_channel_config(&ch_conf);
    
    // PWMB  
    ch_conf.gpio_num   = _pwmb;
    ch_conf.channel    = _pwm_ch_b;
    ledc_channel_config(&ch_conf);
    
    // 初始状态：停止
    stop();
}

void MotorDriver::setMotorSpeed(ledc_channel_t ch, uint8_t speed)
{
    if(speed > ((1 << PWM_RESOLUTION) - 1)) {
        speed = ((1 << PWM_RESOLUTION) - 1); // 防止越界
    }
    ledc_set_duty(LEDC_MODE, ch, speed);
    ledc_update_duty(LEDC_MODE, ch);
}

void MotorDriver::setMotorDirection(int motor, bool forward)
{
    if (motor == 0) {  // 电机A
        gpio_set_level((gpio_num_t)_ain1, forward ? 1 : 0);
        gpio_set_level((gpio_num_t)_ain2, forward ? 0 : 1);
    } else {  // 电机B
        gpio_set_level((gpio_num_t)_bin1, forward ? 1 : 0);
        gpio_set_level((gpio_num_t)_bin2, forward ? 0 : 1);
    }
}

void MotorDriver::stop()
{
    setMotorSpeed(_pwm_ch_a, 0);
    setMotorSpeed(_pwm_ch_b, 0);
    // 方向引脚置低
    gpio_set_level((gpio_num_t)_ain1, 0);
    gpio_set_level((gpio_num_t)_ain2, 0);
    gpio_set_level((gpio_num_t)_bin1, 0);
    gpio_set_level((gpio_num_t)_bin2, 0);
}

void MotorDriver::forward(uint8_t speed)
{
    setMotorDirection(0, true);   // 电机A正转
    setMotorDirection(1, true);   // 电机B正转
    setMotorSpeed(_pwm_ch_a, 150);
    setMotorSpeed(_pwm_ch_b, 150);
    vTaskDelay(1000); // 逐渐加速，避免电机过载
    stop();
}

void MotorDriver::backward(uint8_t speed)
{
    setMotorDirection(0, false);  // 电机A反转
    setMotorDirection(1, false);  // 电机B反转
    for(int i = 0; i <= speed; i+=10) {
        setMotorSpeed(_pwm_ch_a, i);
        setMotorSpeed(_pwm_ch_b, i);
        vTaskDelay(20); // 逐渐加速，避免电机过载
    }
    stop();
}

void MotorDriver::turnLeft(uint8_t speed)
{
    setMotorDirection(0, false);  // 左轮反转
    setMotorDirection(1, true);   // 右轮正转
    setMotorSpeed(_pwm_ch_a, speed);
    setMotorSpeed(_pwm_ch_b, speed);
}

void MotorDriver::turnRight(uint8_t speed)
{
    setMotorDirection(0, true);   // 左轮正转
    setMotorDirection(1, false);  // 右轮反转
    setMotorSpeed(_pwm_ch_a, speed);
    setMotorSpeed(_pwm_ch_b, speed);
}
