// motor.h
#ifndef MOTOR_H_
#define MOTOR_H_

#include "driver/ledc.h"
#include "driver/gpio.h"

#define LEDC_MODE LEDC_LOW_SPEED_MODE

class MotorDriver {
public:
    // 构造函数：AIN1, AIN2, BIN1, BIN2, PWMA, PWMB
    MotorDriver(int ain1, int ain2, int bin1, int bin2, int pwma, int pwmb);
    void init();
    void stop();
    void forward(uint8_t speed);
    void backward(uint8_t speed);
    void turnLeft(uint8_t speed);
    void turnRight(uint8_t speed);

private:
    int _ain1, _ain2, _bin1, _bin2;
    int _pwma, _pwmb;  // PWM引脚
    
    ledc_channel_t _pwm_ch_a;  // PWM通道A
    ledc_channel_t _pwm_ch_b;  // PWM通道B
    
    void setMotorSpeed(ledc_channel_t ch, uint8_t speed);
    void setMotorDirection(int motor, bool forward);
};

#endif // MOTOR_H_
