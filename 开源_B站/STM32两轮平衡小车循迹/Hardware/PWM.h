#include "stm32f10x.h"                  // Device header

#ifndef __PWM_H
#define __PWM_H

void PWM_Z_Init(void);//����PWM��ʼ�� ���ֵ:7200
void PWM_Z_SetCompare3(uint16_t Compare);//����PWM��ֵ ���ֵ:7200
void PWM_Y_Init(void);//�ҵ��PWM��ʼ�� ���ֵ:7200
void PWM_Y_SetCompare4(uint16_t Compare);//�ҵ��PWM��ֵ ���ֵ:7200

#endif
