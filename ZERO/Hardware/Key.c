#include "stm32f10x.h"                  // 包含STM32F10x系列的设备头文件
#include "Delay.h"                     // 包含延时函数库

void Key_Init(void)                    // 初始化按键模块的函数
{
    // 第一步：开启GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    // 这行代码开启了GPIOB的时钟，使能对应引脚的外设功能
    
    // 第二步：定义一个GPIO初始化结构体变量
    GPIO_InitTypeDef GPIO_DefStructure;
    GPIO_DefStructure.GPIO_Mode = GPIO_Mode_IPU;  // 设置GPIO模式为上拉输入
    GPIO_DefStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5;  // 配置GPIOB的Pin5和Pin7
    GPIO_DefStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 设置GPIO速度为50MHz
    
    // 第三步：配置GPIO口
    GPIO_Init(GPIOB , &GPIO_DefStructure);  // 初始化GPIOB
}

uint8_t Key_GetNum(void)               // 获取按键编号的函数
{
    uint8_t KeyNum = 0;                // 定义一个变量，初始值为0
    
    // 检测GPIOB的Pin5是否被按下
    if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_5) == 0)
    {
        Delay_ms(20);                  // 按键消抖，延时20ms
        while(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_5) == 0);  // 等待按键释放
        Delay_ms(20);                  // 再次消抖，延时20ms
        KeyNum = 1;                    // 如果按键被按下，设置KeyNum为1
    }
    
    // 检测GPIOB的Pin7是否被按下
    if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) == 0)
    {
        Delay_ms(20);                  // 按键消抖，延时20ms
        while(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) == 0);  // 等待按键释放
        Delay_ms(20);                  // 再次消抖，延时20ms
        KeyNum = 2;                    // 如果按键被按下，设置KeyNum为2
    }
    
    return KeyNum;                     // 返回按键编号
}
