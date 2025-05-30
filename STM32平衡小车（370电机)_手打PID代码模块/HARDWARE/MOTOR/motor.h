/* 电机控制模块头文件 */
#ifndef  _MOTOR_H
#define  _MOTOR_H

#include "sys.h"
/* GPIO引脚宏定义 -------------------------------------------------*/
// 电机A控制引脚（可能连接H桥电路）
#define Ain1  PBout(14)  // A电机方向控制引脚1
#define Ain2  PBout(15)  // A电机方向控制引脚2

// 电机B控制引脚（可能连接H桥电路）
#define Bin1  PBout(13)  // B电机方向控制引脚1（原注释含特殊字符）
#define Bin2  PBout(12)  // B电机方向控制引脚2

/* 函数声明 ------------------------------------------------------*/
void Stop(float *Med_Jiaodu, float *Jiaodu);  // 紧急停止函数
void Motor_Init(void);                        // 电机GPIO初始化函数
void Limit(int *motoA, int *motoB);           // PWM限幅函数
int GFP_abs(int p);                           // 自定义绝对值函数
void Load(int moto1, int moto2);              // 电机加载PWM函数

#endif
