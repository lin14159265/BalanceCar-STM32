#include "stm32f10x.h"                  // 包含STM32F10x系列的设备头文件

void LED_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);// 禁用JTAG功能
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_6);
}


void LED1_ON(void)                     // 打开LED1的函数
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_4);  // 将GPIOB的Pin4设置为低电平，打开LED1
}

void LED1_OFF(void)                    // 关闭LED1的函数
{
    GPIO_SetBits(GPIOB, GPIO_Pin_4);    // 将GPIOB的Pin4设置为高电平，关闭LED1
}

void LED1_Turn(void)                   // 切换LED1状态的函数
{
    if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_4) == 0)
    {                                   // 如果LED1当前是关闭状态
        GPIO_SetBits(GPIOB, GPIO_Pin_4);  // 设置为高电平，关闭LED1
    }
    else
    {                                   // 如果LED1当前是打开状态
        GPIO_ResetBits(GPIOB, GPIO_Pin_4);  // 设置为低电平，打开LED1
    }
}

void LED2_ON(void)                     // 打开LED2的函数
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // 将GPIOB的Pin6设置为低电平，打开LED2
}

void LED2_OFF(void)                    // 关闭LED2的函数
{
    GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 将GPIOB的Pin6设置为高电平，关闭LED2
}

void LED2_Turn(void)                   // 切换LED2状态的函数
{
    if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6) == 0)
    {                                   // 如果LED2当前是关闭状态
        GPIO_SetBits(GPIOB, GPIO_Pin_6);  // 设置为高电平，关闭LED2
    }
    else
    {                                   // 如果LED2当前是打开状态
        GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // 设置为低电平，打开LED2
    }
}
