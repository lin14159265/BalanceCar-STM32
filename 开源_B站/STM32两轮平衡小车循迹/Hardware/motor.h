#include "stm32f10x.h"                  // Device header

#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Z_Init(void);//������ʼ��
void Motor_Z_SetSpeed(int16_t Speed);//���������ٶ�
void Motor_Y_Init(void);//�ҵ����ʼ��
void Motor_Y_SetSpeed(int16_t Speed);//�ҵ�������ٶ�

#endif
