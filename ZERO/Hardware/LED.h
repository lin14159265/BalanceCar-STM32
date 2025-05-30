#ifndef __LED_H
#define __LED_H

// 定义LED模块的函数声明
void LED_Init(void);                   // LED初始化函数
void LED1_ON(void);                    // 打开LED1的函数
void LED1_OFF(void);                   // 关闭LED1的函数
void LED2_ON(void);                    // 打开LED2的函数
void LED2_OFF(void);                   // 关闭LED2的函数
void LED1_Turn(void);                  // 切换LED1状态的函数
void LED2_Turn(void);                  // 切换LED2状态的函数

#endif // __LED_H
