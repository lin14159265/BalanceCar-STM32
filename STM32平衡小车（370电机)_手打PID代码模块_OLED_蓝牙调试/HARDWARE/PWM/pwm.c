#include "pwm.h"

// PWM初始化函数，使用TIM1定时器生成PWM信号
// 参数说明：
// Psc - 预分频值，用于调整定时器时钟频率
// Per - 自动重装载值，决定PWM周期
void PWM_Init_TIM1(u16 Psc, u16 Per)
{
    GPIO_InitTypeDef GPIO_InitStruct;          // GPIO初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; // 定时器时基初始化结构体
    TIM_OCInitTypeDef TIM_OCInitStruct;         // 定时器输出比较初始化结构体

    // 1. 外设时钟使能（包含GPIOA、TIM1和AFIO）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);

    // 2. GPIO初始化配置（PA8和PA11作为PWM输出引脚）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;       // 复用推挽输出模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11; // 配置PA8（TIM1_CH1）和PA11（TIM1_CH4）
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // 输出速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStruct);                // 应用GPIO配置

    // 3. 定时器时基配置（设置PWM频率核心参数）
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);     // 初始化时基结构体
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频（不分频）
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStruct.TIM_Period = Per;            // 自动重装载值（决定PWM周期）
    TIM_TimeBaseInitStruct.TIM_Prescaler = Psc;         // 预分频值（调整定时器时钟）
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);    // 应用时基配置

    // 4. 输出比较配置（PWM模式设置）
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;      // PWM模式1（CNT<CCR时输出有效）
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性高（有效电平为高）
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStruct.TIM_Pulse = 0;                     // 初始占空比为0

    // 5. 初始化输出比较通道（CH1和CH4）
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);  // 初始化TIM1通道1（PA8）
    TIM_OC4Init(TIM1, &TIM_OCInitStruct);  // 初始化TIM1通道4（PA11）

    // 6. 高级定时器专用配置（必须使能主输出）
    TIM_CtrlPWMOutputs(TIM1, ENABLE);      // 使能TIM1主输出（MOE位）

    // 7. 预加载配置（确保参数即时生效）
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能通道1预加载
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能通道4预加载
    TIM_ARRPreloadConfig(TIM1, ENABLE);     // 使能ARR预加载（周期参数即时更新）

    // 8. 启动定时器
    TIM_Cmd(TIM1, ENABLE);                 // 使能TIM1计数器
}

/* 
关键配置说明：
1. GPIO模式选择GPIO_Mode_AF_PP：复用推挽输出，用于定时器PWM输出
2. 定时器频率计算公式：PWM频率 = TIM1_CLK / [(PSC+1)*(PER+1)]
   例如：72MHz/(71+1)*(999+1) = 1KHz
3. TIM_OCMode_PWM1和PWM2区别：
   - PWM1模式：CNT < CCR时输出有效电平
   - PWM2模式：CNT > CCR时输出有效电平
4. 高级定时器必须调用TIM_CtrlPWMOutputs才能输出PWM
5. 预加载机制保证参数修改在更新事件时生效，避免波形异常
*/
