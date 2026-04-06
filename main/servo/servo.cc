#include "servo.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

ServoDriver::ServoDriver(int pin1, int pin2, ledc_channel_t channel1, ledc_channel_t channel2, ledc_timer_t timer)
    : _pin1(pin1), _pin2(pin2), _channel1(channel1), _channel2(channel2), _timer(timer)
{}

void ServoDriver::init()
{
    ESP_LOGI("Servo", "初始化舵机: 引脚1=%d, 引脚2=%d, 通道1=%d, 通道2=%d, 定时器=%d", 
             _pin1, _pin2, _channel1, _channel2, _timer);
    
    // 配置LEDC定时器
    ledc_timer_config_t timer_conf1 = {};
    timer_conf1.speed_mode       = SERVO_MODE;
    timer_conf1.timer_num        = _timer;
    timer_conf1.duty_resolution  = SERVO_RESOLUTION;
    timer_conf1.freq_hz          = SERVO_FREQ_HZ;
    // timer_conf1.clk_cfg          = LEDC_AUTO_CLK;
    timer_conf1.clk_cfg          = LEDC_USE_XTAL_CLK;
    ledc_timer_config(&timer_conf1);
    
    // 配置LEDC通道
    ledc_channel_config_t ch_conf1 = {};
    ch_conf1.speed_mode     = SERVO_MODE;
    ch_conf1.timer_sel      = _timer;
    ch_conf1.intr_type      = LEDC_INTR_DISABLE;
    ch_conf1.duty           = 25;
    ch_conf1.hpoint         = 0;
    
    // 舵机1
    ch_conf1.gpio_num       = _pin1;
    ch_conf1.channel        = _channel1;
    ledc_channel_config(&ch_conf1);
    
    // 舵机2
    ch_conf1.gpio_num       = _pin2;
    ch_conf1.channel        = _channel2;
    ledc_channel_config(&ch_conf1);
    
    // 初始状态：设置为90度中位
    stop();
}

void ServoDriver::setAngle(int servoNum, int angle)
{
    // 限制角度范围 0-180
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    
    uint32_t duty = ((float)angle / 1800 + 0.025 ) * 1024;  
    
    if (servoNum == 1) {
        ledc_set_duty(SERVO_MODE, _channel1, duty);
        ledc_update_duty(SERVO_MODE, _channel1);
        ESP_LOGI("Servo", "设置舵机1角度: %d度, 占空比: %u", angle, duty);
    } else if (servoNum == 2) {
        ledc_set_duty(SERVO_MODE, _channel2, duty);
        ledc_update_duty(SERVO_MODE, _channel2);
        ESP_LOGI("Servo", "设置舵机2角度: %d度, 占空比: %u", angle, duty);
    } else {
        ESP_LOGE("Servo", "无效的舵机编号: %d", servoNum);
    }
}

void ServoDriver::stop()
{
    // 停止舵机，设置为90度中位
    setAngle(1, 90);
    setAngle(2, 90);
    ESP_LOGI("Servo", "舵机已停止，回到中位");
}

void ServoDriver::waveHand()
{
    ESP_LOGI("Servo", "开始挥手动作");
    
    // 控制舵机2来回运动
    for (int i = 0; i < 3; i++) {
        // 从90度移动到150度
        for (int angle = 90; angle <= 150; angle += 10) {
            setAngle(2, angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // 从150度移动到30度
        for (int angle = 150; angle >= 30; angle -= 10) {
            setAngle(2, angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // 从30度移动回90度
        for (int angle = 30; angle <= 90; angle += 10) {
            setAngle(2, angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // 暂停一下
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    
    ESP_LOGI("Servo", "挥手动作完成");
}

void ServoDriver::dance()
{
    ESP_LOGI("Servo", "开始跳舞动作");
    
    // 控制舵机1和舵机2以不同方向来回运动
    for (int i = 0; i < 4; i++) {
        // 舵机1从90度到150度，舵机2从90度到30度
        for (int angle1 = 90, angle2 = 90; angle1 <= 150 && angle2 >= 30; angle1 += 10, angle2 -= 10) {
            setAngle(1, angle1);
            setAngle(2, angle2);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // 舵机1从150度到30度，舵机2从30度到150度
        for (int angle1 = 150, angle2 = 30; angle1 >= 30 && angle2 <= 150; angle1 -= 10, angle2 += 10) {
            setAngle(1, angle1);
            setAngle(2, angle2);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // 舵机1从30度到90度，舵机2从150度到90度
        for (int angle1 = 30, angle2 = 150; angle1 <= 90 && angle2 >= 90; angle1 += 10, angle2 -= 10) {
            setAngle(1, angle1);
            setAngle(2, angle2);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    
    ESP_LOGI("Servo", "跳舞动作完成");
}