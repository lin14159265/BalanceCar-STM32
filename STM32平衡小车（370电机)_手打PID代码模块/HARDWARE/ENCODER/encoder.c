#include "encoder.h"

/* TIM2编码器接口初始化函数 */
void Encoder_TIM2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;       // GPIO配置结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; // 定时器时基配置结构体
    TIM_ICInitTypeDef TIM_ICInitStruct;      // 输入捕获配置结构体

    /* 启用GPIOA和TIM2的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); // 使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  // 使能TIM2时钟

    /* 配置PA0和PA1为浮空输入模式（编码器信号输入） */
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING; // 浮空输入模式
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0 |GPIO_Pin_1; // 配置PA0和PA1引脚
    GPIO_Init(GPIOA,&GPIO_InitStruct);              // 应用GPIO配置

    /* 配置TIM2基础参数 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct); // 初始化时基结构体
    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // 时钟分频（不分频）
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStruct.TIM_Period=65535;         // 自动重装载值（最大计数值）
    TIM_TimeBaseInitStruct.TIM_Prescaler=0;          // 预分频器（不分频）
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);  // 应用时基配置

    /* 配置编码器接口模式 */
    TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising); 
    // 参数说明：
    // TIM_EncoderMode_TI12 - 使用TI1和TI2作为编码器输入
    // 双通道上升沿有效

    /* 配置输入捕获滤波器 */
    TIM_ICStructInit(&TIM_ICInitStruct);     // 初始化输入捕获结构体
    TIM_ICInitStruct.TIM_ICFilter=10;        // 输入滤波器值（抗抖动）
    TIM_ICInit(TIM2,&TIM_ICInitStruct);      // 应用输入捕获配置

    /* 使能定时器更新中断 */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); // 使能定时器更新中断

    /* 复位计数器并启动定时器 */
    TIM_SetCounter(TIM2,0);                  // 清除计数器值
    TIM_Cmd(TIM2,ENABLE);                    // 启动TIM2定时器
}

/* TIM4编码器接口初始化函数（配置与TIM2类似） */
void Encoder_TIM4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_ICInitTypeDef TIM_ICInitStruct;

    /* 启用GPIOB和TIM4的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    /* 配置PB6和PB7为浮空输入模式 */
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6 |GPIO_Pin_7;
    GPIO_Init(GPIOB,&GPIO_InitStruct);

    /* TIM4基础配置 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period=65535;
    TIM_TimeBaseInitStruct.TIM_Prescaler=0;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);

    /* 编码器接口配置 */
    TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

    /* 输入捕获配置 */
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_ICFilter=10;
    TIM_ICInit(TIM4,&TIM_ICInitStruct);

    /* 中断配置 */
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);      // 清除更新标志
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);  // 使能更新中断

    /* 启动定时器 */
    TIM_SetCounter(TIM4,0);
    TIM_Cmd(TIM4,ENABLE);
}

/* 读取编码器速度函数 */
int Read_Speed(int TIMx)
{
    int value_1;
    switch(TIMx)
    {
        case 2:
            value_1=(short)TIM_GetCounter(TIM2); // 读取TIM2的计数值（带符号数）
            TIM_SetCounter(TIM2,0);              // 立即重置计数器
            break;
        case 4:
            value_1=(short)TIM_GetCounter(TIM4); // 读取TIM4的计数值
            TIM_SetCounter(TIM4,0);
            break;
        default:
            value_1=0;
    }
    return value_1; // 返回带符号的计数值（正负表示方向）
}

/* TIM2中断服务函数 */
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=0) // 检查更新中断标志
    {
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); // 清除中断标志
    }
}

/* TIM4中断服务函数 */
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=0)
    {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    }
}
