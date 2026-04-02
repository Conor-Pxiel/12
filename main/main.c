#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "pwm.h"
#include "motor.h"
#include "motor2.h"
#include "esp_log.h"
#include "mpu6050.h"
#include "OLED.h"
#include "OLED_Data.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    pwm_init(); 
    OLED_Init();
    A1_Control(1, 6000); // 让电机以一半速度正转
    A0_Control(1, 6000); // 让电机以一半速度正转
    vTaskDelay(1000); // 主循环挂起
    
    set_angle(0);
    vTaskDelay(100);
    set_angle(90);
    vTaskDelay(100);
    set_angle(180);
    vTaskDelay(100);
    set_angle2(0);
    vTaskDelay(100);
    set_angle2(90);
    vTaskDelay(100);
    set_angle2(180);
    vTaskDelay(100);

    esp_err_t err = mpu6050_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050 init failed. System halt.");
        while(1) { vTaskDelay(1000 / portTICK_PERIOD_MS); } // 初始化失败则卡死
    }

    mpu6050_data_t sensor_data;
    while(1)
    {
        OLED_DrawRectangle(56, 8, 16, 40, OLED_FILLED); //方眼
        OLED_Update();
        vTaskDelay(100);
        err = mpu6050_read_data(&sensor_data);
        if (err == ESP_OK) {
            printf("Accel: X:%6d, Y:%6d, Z:%6d | Gyro: X:%6d, Y:%6d, Z:%6d\n",
                   sensor_data.accel_x, sensor_data.accel_y, sensor_data.accel_z,
                   sensor_data.gyro_x, sensor_data.gyro_y, sensor_data.gyro_z);
        } else {
            ESP_LOGE(TAG, "Failed to read data from MPU6050");
        }
        
        // 延时 100 毫秒
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}

