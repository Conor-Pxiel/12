#ifndef __PID_H_
#define __PID_H_

#include <stdint.h>

// PID 控制器结构体
typedef struct {
    float Kp;           // 角度比例系数
    float Kd;           // 角速度微分系数 (阻尼)
    float Ki;           // 积分系数 (平衡环通常不需要，可预留)
    
    float setpoint;     // 目标平衡角度 (机械中值)
    float integral;     // 积分累加
    float output_limit; // 最大 PWM 限幅
} pid_ctrl_t;

// 函数声明
void pid_init(pid_ctrl_t *pid, float kp, float ki, float kd, float out_limit, float setpoint);
// 核心：基于角速度的 PD 计算
float balance_pd_compute(pid_ctrl_t *pid, float current_angle, float gyro_rate);
// 互补滤波算法
float imu_get_pitch_angle(int16_t accel_x, int16_t accel_z, int16_t gyro_y, float dt);

#endif
