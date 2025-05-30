#ifndef __MPU6050_H
#define __MPU6050_H
#include "stm32f10x.h"

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);//MPU6050д�Ĵ���
uint8_t MPU6050_ReadReg(uint8_t RegAddress);//MPU6050���Ĵ���

void MPU6050_Init(void);//MPU6050��ʼ��
uint8_t MPU6050_GetID(void);//MPU6050��ȡID��
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);//MPU6050��ȡ����

void MPU6050_Angle_Init(void);//��ó�ʼ�Ƕ�ֵ
void MPU6050_Angle(void);//���½Ƕ�ֵ����ȡMPU6050����ֵ

#endif
