#ifndef __MOTOR2_H
#define __MOTOR2_H

#include "driver/ledc.h"
#include "pwm.h"

void A0_Control(uint8_t dir_L, uint32_t speed);
void A1_Control(uint8_t dir_L, uint32_t speed);


#endif