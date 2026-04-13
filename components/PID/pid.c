#include "pid.h"
#include <math.h>

void pid_init(pid_ctrl_t *pid, float kp, float ki, float kd, float out_limit, float setpoint) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->setpoint = setpoint;
    pid->integral = 0.0f;
    pid->output_limit = out_limit;
}

float balance_pd_compute(pid_ctrl_t *pid, float current_angle, float gyro_rate) {
    // 1. 计算角度偏差 (P)
    float error = current_angle - pid->setpoint;
    
    // 2. 计算输出
    // P项：角度偏差越大，恢复力越大
    // D项：直接使用陀螺仪角速度，提供阻尼防止震荡
    float output = (pid->Kp * error) + (pid->Kd * gyro_rate);

    // 3. 限幅
    if (output > pid->output_limit) output = pid->output_limit;
    else if (output < -pid->output_limit) output = -pid->output_limit;

    return output;
}

float imu_get_pitch_angle(int16_t accel_x, int16_t accel_z, int16_t gyro_y, float dt) {
    static float pitch = 0.0f;
    // 加速度计计算角度 (atan2)
    float accel_angle = atan2f(-(float)accel_x, -(float)accel_z) / 3.1415926f * 180; 
    accel_angle += 0;
    // 陀螺仪角速度
    gyro_y -= 1.5;
    float gyro_rate = (float)gyro_y / 32768.0f * 2000.0f; 

    // 互补滤波：98% 信任陀螺仪积分，2% 信任加速度计纠偏
    pitch = 0.98f * (pitch + gyro_rate * dt) + 0.02f * accel_angle;
    return pitch;
}