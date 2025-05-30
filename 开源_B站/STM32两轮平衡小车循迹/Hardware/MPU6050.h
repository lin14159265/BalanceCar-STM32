#ifndef __MPU6050_H
#define __MPU6050_H
#include "stm32f10x.h"

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);//MPU6050写寄存器
uint8_t MPU6050_ReadReg(uint8_t RegAddress);//MPU6050读寄存器

void MPU6050_Init(void);//MPU6050初始化
uint8_t MPU6050_GetID(void);//MPU6050获取ID号
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);//MPU6050获取数据

void MPU6050_Angle_Init(void);//获得初始角度值
void MPU6050_Angle(void);//更新角度值，读取MPU6050的数值

#endif
