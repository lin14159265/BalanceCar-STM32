// 头文件保护，防止重复包含
#ifndef __SYS_H
#define __SYS_H

// 包含STM32标准外设库头文件
#include "stm32f10x.h"
// 包含项目相关驱动头文件
#include "delay.h"         // 延时函数
#include "usart.h"         // 串口通信
#include "inv_mpu.h"       // MPU传感器库
#include "inv_mpu_dmp_motion_driver.h" // DMP运动驱动
#include "mpu6050.h"       // MPU6050驱动
#include "OLED.h"          // OLED显示
#include "control.h"       // 控制系统
#include "encoder.h"       // 编码器处理
#include "exti.h"          // 外部中断
#include "motor.h"         // 电机控制
#include "pwm.h"           // PWM生成
#include "usart3.h"        // USART3串口

// 包含C标准库
#include <string.h>        // 字符串处理
#include <stdio.h>         // 标准输入输出
#include <stdint.h>        // 标准整型定义
#include <stdlib.h>        // 标准库函数
#include <math.h>          // 数学函数

/*----------------------------------------------
位带操作宏定义（Bit-Banding）
用于实现原子级别的位操作，提高IO操作效率
-----------------------------------------------*/
// 位带计算公式：将地址空间映射到位带别名区
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2)) 
// 将地址转换为可访问的指针
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
// 组合位带地址计算和指针转换
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
/*----------------------------------------------
GPIO端口输出数据寄存器（ODR）地址定义
ODR寄存器用于控制端口输出状态
-----------------------------------------------*/
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) // GPIOA输出寄存器地址 0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) // GPIOB输出寄存器地址 0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) // GPIOC输出寄存器地址 0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) // GPIOD输出寄存器地址 0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) // GPIOE输出寄存器地址 0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) // GPIOF输出寄存器地址 0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) // GPIOG输出寄存器地址 0x40011E0C

/*----------------------------------------------
GPIO端口输入数据寄存器（IDR）地址定义
IDR寄存器用于读取端口输入状态
-----------------------------------------------*/
#define GPIOA_IDR_Addr    (GPIOA_BASE+8)  // GPIOA输入寄存器地址 0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)  // GPIOB输入寄存器地址 0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8)  // GPIOC输入寄存器地址 0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8)  // GPIOD输入寄存器地址 0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8)  // GPIOE输入寄存器地址 0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8)  // GPIOF输入寄存器地址 0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8)  // GPIOG输入寄存器地址 0x40011E08

/*----------------------------------------------
IO端口位操作宏
实现单个GPIO引脚的快速读写操作
参数n范围：0-15（对应16个IO引脚）
-----------------------------------------------*/
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  // GPIOA第n位输出控制
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  // GPIOA第n位输入读取

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  // GPIOB第n位输出控制
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  // GPIOB第n位输入读取

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  // GPIOC第n位输出控制
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  // GPIOC第n位输入读取

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  // GPIOD第n位输出控制
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  // GPIOD第n位输入读取

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  // GPIOE第n位输出控制
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  // GPIOE第n位输入读取

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  // GPIOF第n位输出控制
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  // GPIOF第n位输入读取

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  // GPIOG第n位输出控制
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  // GPIOG第n位输入读取

/*----------------------------------------------
中断配置相关定义
-----------------------------------------------*/
#define GPIO_A 0  // GPIOA组编号
#define GPIO_B 1  // GPIOB组编号
#define GPIO_C 2  // GPIOC组编号
#define GPIO_D 3  // GPIOD组编号
#define GPIO_E 4  // GPIOE组编号
#define GPIO_F 5  // GPIOF组编号
#define GPIO_G 6  // GPIOG组编号

#define FTIR   1  // 下降沿触发中断
#define RTIR   2  // 上升沿触发中断

/*----------------------------------------------
JTAG/SWD调试模式配置
-----------------------------------------------*/
#define JTAG_SWD_DISABLE   0X02  // 禁用JTAG和SWD
#define SWD_ENABLE         0X01  // 启用SWD模式
#define JTAG_SWD_ENABLE    0X00  // 启用完整JTAG模式

/*----------------------------------------------
全局变量声明
-----------------------------------------------*/
extern char stop;               // 系统停止标志
extern u8 Fore,Back,Left,Right; // 方向控制标志位
extern int PWM_MAX,PWM_MIN;     // PWM输出极值限制
extern int MOTO1,MOTO2;         // 电机控制量
extern float Pitch,Roll,Yaw;    // 欧拉角（俯仰/横滚/偏航）
extern short gyrox,gyroy,gyroz; // 陀螺仪原始数据
extern short aacx,aacy,aacz;    // 加速度计原始数据
extern int Encoder_Left,Encoder_Right; // 左右编码器计数值

/*----------------------------------------------
函数声明
-----------------------------------------------*/
void NVIC_Config(void);         // 中断优先级配置函数

#endif // 结束头文件保护
