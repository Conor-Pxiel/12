#include "iot/thing.h"
#include "board.h"
#include "audio_codec.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include "application.h"
#include "servo/servo.h"

#define TAG "Servo"

namespace iot {
class Servo : public Thing {
private:

public:
    ServoDriver servo_;
    int angle_ = 90;
    int servo_num_ = 1;
    
    Servo() : Thing("Servo", "舵机控制装置"), servo_(12, 17){
        servo_.init();
        // 属性
        properties_.AddNumberProperty("angle", "舵机角度", [this]() {
            return angle_;
        });
        properties_.AddNumberProperty("servoNum", "舵机编号", [this]() {
            return servo_num_;
        });

        ParameterList params({
            Parameter("angle", "0到180整数", kValueTypeNumber, true),
            Parameter("servoNum", "1或2", kValueTypeNumber, true)
        });

        methods_.AddMethod("setAngle", "设置舵机角度", params, [this](const ParameterList& parameters) {
            angle_ = parameters["angle"].number();
            servo_num_ = parameters["servoNum"].number();
            ESP_LOGI(TAG, "Servo %d set to angle: %d", servo_num_, angle_);
            servo_.setAngle(servo_num_, angle_);
        });

        methods_.AddMethod("stop", "停止舵机", ParameterList(), [this](const ParameterList&) {
            ESP_LOGI(TAG, "Servo stopped");
            servo_.stop();
        });

        methods_.AddMethod("waveHand", "挥手动作", ParameterList(), [this](const ParameterList&) {
            ESP_LOGI(TAG, "Servo wave hand");
            servo_.waveHand();
        });

        methods_.AddMethod("dance", "跳舞动作", ParameterList(), [this](const ParameterList&) {
            ESP_LOGI(TAG, "Servo dance");
            servo_.dance();
        });
    }
};

}  // namespace iot

DECLARE_THING(Servo);