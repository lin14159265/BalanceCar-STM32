// 防止头文件被重复包含的预处理指令
// _PWM_H 是宏定义标识符，通常建议使用项目相关唯一名称且避免以单下划线开头（C标准保留）
// 此处保持原有代码不变，但实际项目中应考虑命名规范
#ifndef  _PWM_H
#define  _PWM_H  // 定义宏_PWM_H，标识该头文件已被包含

// 包含系统级头文件，可能包含芯片外设寄存器定义、类型别名等基础配置
// "sys.h" 通常包含系统时钟配置、常用宏定义和数据类型声明（如u16类型）
#include "sys.h"
// 函数声明：初始化高级定时器TIM1用于PWM输出
// 参数说明：
// Psc - 预分频系数 (Prescaler)，取值范围0x0000~0xFFFF
//       用于将定时器时钟源分频，公式：实际分频系数 = Psc + 1
//       例如：系统时钟72MHz，Psc=71时，分频后时钟为1MHz
// Per - 自动重装载值 (Period)，取值范围0x0000~0xFFFF
//       决定PWM信号周期，公式：PWM周期 = (Per + 1) * (1 / 分频后时钟频率)
//       例如：分频后时钟1MHz，Per=999时，PWM周期为1ms
// 功能说明：
// 1. 配置TIM1的预分频器和周期寄存器
// 2. 设置TIM1的计数模式（通常为向上计数）
// 3. 使能TIM1的PWM输出通道（需结合具体硬件连接）
// 4. 启动TIM1计数器（需在初始化后调用启动函数）
// 注意：具体通道配置需在.c文件中实现，此处仅声明接口
void PWM_Init_TIM1(u16 Psc,u16 Per);

#endif  // 结束头文件保护宏
