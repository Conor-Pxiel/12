#include "iot/thing.h"
#include "board.h"
#include "audio_codec.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include "application.h"
#include "motor/motor.h"
#include <freertos/timers.h>

#define TAG "Motor"

namespace iot {
static void StopMotorCallback(TimerHandle_t xTimer);
class Motor : public Thing {
private:
    TimerHandle_t stop_timer_ = nullptr; // 定义一个定时器指针，稍后创建✅ 放在 private 中更合理

public:
    MotorDriver motor_;
	int motor_speed_ = 0;
	int run_time_ = 2;
	
    Motor() : Thing("Motor", "电机控制装置"), motor_(18, 19, 21, 47, 9 ,10){
        motor_.init();
		// 创建定时器（不自动重复，1秒占位）
		stop_timer_ = xTimerCreate("StopMotorTimer", pdMS_TO_TICKS(1000), pdFALSE, this, StopMotorCallback);
		if (!stop_timer_) {
			ESP_LOGE(TAG, "定时器创建失败！");
		}
        // 属性
        properties_.AddNumberProperty("motorSpeed", "电机速度", [this]() {
            return motor_speed_;
        });
        properties_.AddNumberProperty("runTime", "电机执行时间（秒）", [this]() {
            return run_time_;
        });

        ParameterList params({
            Parameter("motorSpeed", "0到255整数", kValueTypeNumber, true),
            Parameter("runTime", "0到100整数", kValueTypeNumber, true)
        });

        methods_.AddMethod("forward", "前进", params, [this](const ParameterList& parameters) {
			motor_speed_ = parameters["motorSpeed"].number();
			run_time_ = parameters["runTime"].number();
			ESP_LOGI(TAG, "status is forward ,time is %d,Output motor_speed: %d", run_time_,motor_speed_);
			motor_.forward(motor_speed_);
			if (stop_timer_) {
			    xTimerStop(stop_timer_, 0);
			    xTimerChangePeriod(stop_timer_, pdMS_TO_TICKS(run_time_ * 1000), 0);
			    xTimerStart(stop_timer_, 0);
			}
        });

        methods_.AddMethod("backward", "后退", params, [this](const ParameterList& parameters) {
			motor_speed_ = parameters["motorSpeed"].number();
			run_time_ = parameters["runTime"].number();
			ESP_LOGI(TAG, "status is backward ,time is %d,Output motor_speed: %d", run_time_,motor_speed_);
			motor_.backward(motor_speed_);
			if (stop_timer_) {
			    xTimerStop(stop_timer_, 0);
			    xTimerChangePeriod(stop_timer_, pdMS_TO_TICKS(run_time_ * 1000), 0);
			    xTimerStart(stop_timer_, 0);
			}
        });

        methods_.AddMethod("turnLeft", "左转", params, [this](const ParameterList& parameters) {
			motor_speed_ = parameters["motorSpeed"].number();
			run_time_ = parameters["runTime"].number();
			ESP_LOGI(TAG, "status is turnLeft ,time is %d,Output motor_speed: %d", run_time_,motor_speed_);
			motor_.turnLeft(motor_speed_);
			if (stop_timer_) {
			    xTimerStop(stop_timer_, 0);
			    xTimerChangePeriod(stop_timer_, pdMS_TO_TICKS(run_time_ * 1000), 0);
			    xTimerStart(stop_timer_, 0);
			}
        });

        methods_.AddMethod("turnRight", "右转", params, [this](const ParameterList& parameters) {
			motor_speed_ = parameters["motorSpeed"].number();
			run_time_ = parameters["runTime"].number();
			ESP_LOGI(TAG, "status is turnRight ,time is %d,Output motor_speed: %d", run_time_,motor_speed_);
			motor_.turnRight(motor_speed_);
			if (stop_timer_) {
			    xTimerStop(stop_timer_, 0);
			    xTimerChangePeriod(stop_timer_, pdMS_TO_TICKS(run_time_ * 1000), 0);
			    xTimerStart(stop_timer_, 0);
			}
        });

        methods_.AddMethod("stop", "停止电机", ParameterList(), [this](const ParameterList&) {
			ESP_LOGI(TAG, "status is stop");
            motor_.stop();
        });
    }
};

static void StopMotorCallback(TimerHandle_t xTimer) {
    // 从定时器中取出指向 Motor 对象的指针
    auto* motor = static_cast<iot::Motor*>(pvTimerGetTimerID(xTimer));
    if (motor) {
        ESP_LOGI("Motor", "时间到了，自动停止电机。");
        motor->motor_.stop();
    }
}

}  // namespace iot

DECLARE_THING(Motor);
