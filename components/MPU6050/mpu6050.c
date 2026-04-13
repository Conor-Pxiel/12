#include "mpu6050.h"
#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "MPU6050";

// 向 MPU6050 寄存器写入一个字节
static esp_err_t mpu6050_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

// 从 MPU6050 寄存器读取多个字节
static esp_err_t mpu6050_read_bytes(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

// 初始化 MPU6050
esp_err_t mpu6050_init(void)
{
    esp_err_t ret;

    // 1. 配置 I2C 主机模式参数
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    // 2. 应用配置
    ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C config failed");
        return ret;
    }

    // 3. 安装 I2C 驱动
    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed");
        return ret;
    }

    // 4. 唤醒 MPU6050 (解除休眠模式)
    // 向电源管理寄存器 1 (0x6B) 写入 0x00，解除休眠并使用内部 8MHz 振荡器
    ret = mpu6050_write_byte(MPU6050_REG_PWR_MGMT_1, 0x00);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to wake up MPU6050");
        return ret;
    }

    // 5. 配置采样率为1000Hz (1ms采样)
    // 采样率 = 8kHz / (1 + SMPLRT_DIV)
    // 1000 = 8000 / (1 + 7)
    ret = mpu6050_write_byte(MPU6050_REG_SMPLRT_DIV, 7);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set sample rate");
        return ret;
    }

    // 6. 禁用数字低通滤波器 (DLPF)
    // DLPF_CFG = 0: 禁用DLPF，内部采样率8kHz
    ret = mpu6050_write_byte(MPU6050_REG_CONFIG, 0x00);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disable DLPF");
        return ret;
    }

    // 7. 配置陀螺仪量程为±2000°/s (最大灵敏度)
    // FS_SEL = 3: ±2000°/s
    ret = mpu6050_write_byte(MPU6050_REG_GYRO_CONFIG, 0x18);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set gyro range");
        return ret;
    }

    // 8. 配置加速度计量程为±8g
    // AFS_SEL = 2: ±8g
    ret = mpu6050_write_byte(MPU6050_REG_ACCEL_CONFIG, 0x10);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set accel range");
        return ret;
    }

    ESP_LOGI(TAG, "MPU6050 initialized successfully (1000Hz, DLPF disabled)");
    return ESP_OK;
}

// 读取加速度和陀螺仪的原始数据
esp_err_t mpu6050_read_data(mpu6050_data_t *data)
{
    uint8_t raw_data[14]; // MPU6050 从 0x3B 开始连续 14 个寄存器包含 加速度(6) + 温度(2) + 陀螺仪(6)
    
    // 从加速度计起始地址连续读取 14 个字节
    esp_err_t ret = mpu6050_read_bytes(MPU6050_REG_ACCEL_XOUT_H, raw_data, 14);
    if (ret != ESP_OK) {
        return ret;
    }

    // 解析加速度计数据 (高低字节拼接)
    data->accel_x = (raw_data[0] << 8) | raw_data[1];
    data->accel_y = (raw_data[2] << 8) | raw_data[3];
    data->accel_z = (raw_data[4] << 8) | raw_data[5];

    // 解析陀螺仪数据 (跳过温度数据 raw_data[6] 和 raw_data[7])
    data->gyro_x = (raw_data[8] << 8) | raw_data[9];
    data->gyro_y = (raw_data[10] << 8) | raw_data[11];
    data->gyro_z = (raw_data[12] << 8) | raw_data[13];

    return ESP_OK;
}