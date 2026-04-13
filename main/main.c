#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "oled.h"
#include "pwm.h"
#include "motor2.h"
#include "mpu6050.h"
#include "pid.h"

static const char *TAG = "MAIN";

// --- 全局共享数据 ---
volatile float g_pitch = 0.0f;
volatile float g_pwm = 0.0f;
volatile float g_gyro_rate = 0.0f;

// --- 调节参数 (根据你的硬件修改) ---
#define TARGET_ANGLE    0.0f     // 机械中值
#define BAL_KP          150.0f    // 比例
#define BAL_KI          0.0f      // 积分项
#define BAL_KD          1.5f      // 阻尼
#define PWM_LIMIT       1023.0f   // 10位PWM
#define DEADZONE        350       // 电机起步死区

// OLED 显示任务：独立运行，不卡主循环
void oled_display_task(void *pvParameters) {
    OLED_Init();
    OLED_Clear();
    //OLED_ShowString(0, 0, "Pitch:", OLED_8X16);
    //OLED_ShowString(0, 16, "PWMOut:", OLED_8X16);
    //OLED_ShowString(0, 32, "GyroRate:", OLED_8X16);
    OLED_DrawCircle(64, 32, 18, OLED_FILLED);
    OLED_Update();

    while (1) {
        // 局部刷新数字区域，防止闪烁
        //OLED_ClearArea(64, 0, 64, 32); 
        //OLED_Printf(64, 0, OLED_8X16, "%.1f", g_pitch);
        //OLED_Printf(64, 16, OLED_8X16, "%.0f", g_pwm);
        //OLED_Printf(64, 32, OLED_8X16, "%.1f", g_gyro_rate);
        //OLED_UpdateArea(64, 0, 64, 32);
        //OLED_Update();
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz 刷新即可
    }
}

void app_main(void) {
    // 1. 硬件基础初始化
    pwm_init();
    
    // 2. MPU6050 唤醒重试逻辑
    esp_err_t err;
    int retry = 0;
    do {
        err = mpu6050_init();
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "MPU6050 retry %d...", ++retry);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    } while (err != ESP_OK && retry < 5);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050 FAIL. Check SDA(GPIO2)/SCL(GPIO1) & Power!");
        return;
    }

    // 3. 启动 OLED 任务
    xTaskCreate(oled_display_task, "oled_task", 4096, NULL, 1, NULL);

    // 4. PID 初始化
    pid_ctrl_t balance_pid;
    pid_init(&balance_pid, BAL_KP, BAL_KI, BAL_KD, PWM_LIMIT, TARGET_ANGLE);

    mpu6050_data_t imu_raw;
    int64_t last_time = esp_timer_get_time();

    while (1) {
        // 计算精确的时间步长 dt
        int64_t now = esp_timer_get_time();
        float dt = (float)(now - last_time) / 1000000.0f;
        last_time = now;

        if (mpu6050_read_data(&imu_raw) == ESP_OK) {
            // 姿态解算
            g_pitch = imu_get_pitch_angle(imu_raw.accel_x, imu_raw.accel_z, imu_raw.gyro_y, dt);
            float gyro_rate_phys = (float)imu_raw.gyro_y / 32768.0f * 2000.0f;
            g_gyro_rate = gyro_rate_phys;
            
            // 调试：检查陀螺仪数据是否有效
            if (isnan(gyro_rate_phys) || isinf(gyro_rate_phys)) {
                g_gyro_rate = 0.0f;  // 防止显示无效值
            }

            // 直立环计算
            float output = balance_pd_compute(&balance_pid, g_pitch, gyro_rate_phys);
            g_pwm = output;

            // 安全防护：角度过大停止电机
            if (fabs(g_pitch) > 30.0f) {
                A0_Control(1, 0);
                A1_Control(1, 0);
            } else {
                uint8_t dir = (output >= 0) ? 1 : 0;
                uint32_t speed = (uint32_t)fabs(output);
                
                // 死区补偿
                if (speed > 10) speed += DEADZONE;
                if (speed > PWM_LIMIT) speed = PWM_LIMIT;

                A0_Control(dir, speed);
                A1_Control(dir, speed);
            }
        }
        
        // 维持 200Hz 的稳定控制频率
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}