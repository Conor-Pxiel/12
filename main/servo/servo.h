// servo.h
#ifndef SERVO_H_
#define SERVO_H_

#include "driver/ledc.h"
#include "driver/gpio.h"

#define SERVO_FREQ_HZ     50      // 舵机标准频率
#define SERVO_RESOLUTION  LEDC_TIMER_10_BIT  // 10位分辨率
#define SERVO_MODE        LEDC_LOW_SPEED_MODE

class ServoDriver {
public:
    // 构造函数：舵机控制引脚, LEDC通道, LEDC定时器
    ServoDriver(int pin1, int pin2, 
                ledc_channel_t channel1 = LEDC_CHANNEL_2, ledc_channel_t channel2 = LEDC_CHANNEL_3,
                ledc_timer_t timer = LEDC_TIMER_1);
    void init();
    void setAngle(int servoNum, int angle);
    void stop();
    void waveHand();
    void dance();

private:
    int _pin1;  // 舵机1控制引脚
    int _pin2;  // 舵机2控制引脚
    ledc_channel_t _channel1;  // LEDC通道1
    ledc_channel_t _channel2;  // LEDC通道2
    ledc_timer_t _timer;  // LEDC定时器
};

#endif // SERVO_H_