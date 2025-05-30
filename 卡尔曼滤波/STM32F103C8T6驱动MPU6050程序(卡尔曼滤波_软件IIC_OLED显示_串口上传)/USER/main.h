#include "sys.h"//系统中断分组库
#include "delay.h"//延时函数库
#include "usart.h"//串口设置库
#include "stdio.h"//标准输入输出库
#include "string.h"//字符串库
#include "LED.h"//LED驱动库
#include "oled.h"//OLED驱动库
#include "TIMER.h"//定时器驱动库
#include "mpu6050.h"//MPU6050驱动库
#include "kalman.h"//卡尔曼解算法库

u8 tmp_buf[20];				//字符串数组

int main(void);				//主函数
void SYS_Init(void);		//系统初始化总函数	
void DATA_Report(void);		//MPU6050数据上报	
