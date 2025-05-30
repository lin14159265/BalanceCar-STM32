#include "stm32f10x.h"                  // Device header

#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Z_Init(void);//左电机初始化
void Motor_Z_SetSpeed(int16_t Speed);//左电机设置速度
void Motor_Y_Init(void);//右电机初始化
void Motor_Y_SetSpeed(int16_t Speed);//右电机设置速度

#endif
