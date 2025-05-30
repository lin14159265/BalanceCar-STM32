/*---------------------------------- 头文件保护 ----------------------------------*/
#ifndef __USART3_H
#define __USART3_H

/*---------------------------------- 包含头文件 ----------------------------------*/
#include "sys.h"
/*---------------------------------- 函数声明 ----------------------------------*/
/* 通过USART3发送字符串 */
void USART3_Send_String(char *String);
/* USART3初始化函数，参数bound为波特率 */
void uart3_init(u32 bound);
/* USART3中断服务函数，处理接收中断 */
void USART3_IRQHandler(void);

#endif

/*---------------------------------- 源文件部分 ----------------------------------*/
#include "usart3.h"  
#include "stdio.h"      // 用于sprintf等格式化函数

/*---------------------------------- 全局变量定义 ----------------------------------*/
u8 uart_buf[5];         // 用于存储格式化数据的缓冲区
u8 Fore,Back,Left,Right;// 方向控制标志位：前进、后退、左转、右转

/*---------------------------------- 函数实现 ----------------------------------*/

/**
  * @brief  USART3初始化函数
  * @param  bound：波特率设置，单位bps
  * @retval 无
  */
void uart3_init(u32 bound)
{
    //----------------------- GPIO配置 -----------------------
    GPIO_InitTypeDef GPIO_InitStructure;    // GPIO配置结构体
    USART_InitTypeDef USART_InitStructure;  // USART配置结构体
  
    // 使能GPIOB和USART3的时钟（USART3挂载在APB1总线）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB属于APB2总线
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);// USART3属于APB1总线
  
    // 配置TX引脚(PB10)：复用推挽输出模式，50MHz速度
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       // 选择PB10引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 最高输出速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 应用配置
  
    // 配置RX引脚(PB11)：浮空输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;        // 选择PB11引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);            // 应用配置
  
    //----------------------- USART参数配置 -----------------------
    USART_InitStructure.USART_BaudRate = bound;       // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;        // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 使能接收和发送模式
    USART_Init(USART3, &USART_InitStructure);         // 应用配置
  
    // 使能USART3接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    // 接收寄存器非空中断
    USART_Cmd(USART3, ENABLE);                        // 使能USART3外设
}

/**
  * @brief  USART3中断服务函数
  * @note   处理接收到的蓝牙数据，更新方向控制标志位
  * @retval 无
  */
extern u8 lock; // 声明外部变量（在其它文件中定义）
void USART3_IRQHandler(void) 
{
    int Bluetooth_data;
    // 检查接收中断标志位
    if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)
    {
        // 从数据寄存器读取接收到的字节
        Bluetooth_data=USART_ReceiveData(USART3);
      
        /* 示例代码（已注释）：
        if(Bluetooth_data==0xf5)lock=0;              // 如果收到解锁指令
        sprintf((char *)uart_buf,"%x",Bluetooth_data); // 格式化接收数据
        OLED_ShowString(0,0,"     ",16);            // 清屏
        OLED_ShowString(0,0,uart_buf,16);            // 显示接收数据
        */
      
        // 方向控制逻辑
        if(Bluetooth_data==0x00)       // 停止指令
            Fore=0,Back=0,Left=0,Right=0;
        else if(Bluetooth_data==0x05)  // 前进指令
            Fore=1,Back=0,Left=0,Right=0;
        else if(Bluetooth_data==0x01)  // 后退指令
            Fore=0,Back=1,Left=0,Right=0;
        else if(Bluetooth_data==0x03)  // 右转指令
            Fore=0,Back=0,Left=0,Right=1;
        else if(Bluetooth_data==0x07)  // 左转指令
            Fore=0,Back=0,Left=1,Right=0;
        else                           // 其它未定义指令
            Fore=0,Back=0,Left=0,Right=0;
    }
}

/**
  * @brief  通过USART3发送单个字节
  * @param  data：要发送的字节数据
  * @retval 无
  */
void USART3_Send_Data(char data)
{
    USART_SendData(USART3,data);             // 写入数据寄存器
    // 等待发送完成（TC：传输完成标志位）
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=1);
}

/**
  * @brief  通过USART3发送字符串
  * @param  String：要发送的字符串指针
  * @retval 无
  */
void USART3_Send_String(char *String)
{
    u16 len,j;
    len=strlen(String);                      // 计算字符串长度
    for(j=0;j<len;j++)                        // 循环发送每个字符
    {
        USART3_Send_Data(*String++);          // 发送当前字符并移动指针
    }
}
