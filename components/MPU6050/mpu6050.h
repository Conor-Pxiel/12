#ifndef __MPU6050_H_
#define __MPU6050_H_

#include <stdint.h>
#include "esp_err.h"

// I2C 配置参数
#define I2C_MASTER_SCL_IO           20               // I2C SCL 对应 ESP32-S3 的 GPIO (请根据实际连线修改)
#define I2C_MASTER_SDA_IO           21               // I2C SDA 对应 ESP32-S3 的 GPIO (请根据实际连线修改)
#define I2C_MASTER_NUM              I2C_NUM_0       // 使用 I2C 端口 0
#define I2C_MASTER_FREQ_HZ          400000          // I2C 通信频率 400kHz
#define I2C_MASTER_TX_BUF_DISABLE   0               // 不使用发送缓冲区
#define I2C_MASTER_RX_BUF_DISABLE   0               // 不使用接收缓冲区
#define I2C_MASTER_TIMEOUT_MS       1000            // I2C 超时时间

// MPU6050 硬件参数
#define MPU6050_SENSOR_ADDR         0x68            // MPU6050 的 I2C 地址 (如果 AD0 接高电平则为 0x69)
#define MPU6050_CMD_START           0x00            // 内部寄存器起始地址

// MPU6050 核心寄存器地址
#define MPU6050_REG_PWR_MGMT_1      0x6B            // 电源管理寄存器 1
#define MPU6050_REG_ACCEL_XOUT_H    0x3B            // 加速度计 X 轴高位寄存器
#define MPU6050_REG_GYRO_XOUT_H     0x43            // 陀螺仪 X 轴高位寄存器

// 传感器数据结构体
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} mpu6050_data_t;

// 函数声明
esp_err_t mpu6050_init(void);
esp_err_t mpu6050_read_data(mpu6050_data_t *data);

#endif
