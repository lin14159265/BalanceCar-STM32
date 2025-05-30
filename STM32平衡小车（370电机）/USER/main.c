#include "stm32f10x.h"   // STM32标准库头文件
#include "sys.h"         // 自定义系统级头文件

/* 超声波传感器引脚定义 */
#define TRIG PBout(9)    // 超声波触发引脚(PB9输出模式)
#define ECHO PBin(8)     // 超声波回波引脚(PB8输入模式)

/* 全局变量 */
int overcount = 0;       // 超声波测量超时计数器
int length;              // 存储计算后的距离值

/* MPU6050传感器数据 */
float Pitch, Roll, Yaw;              // 欧拉角
short gyrox, gyroy, gyroz;           // 陀螺仪原始数据
short aacx, aacy, aacz;              // 加速度计原始数据
int Encoder_Left, Encoder_Right;     // 左右电机编码器值

/* 电机PWM限制 */
int PWM_MAX = 5000, PWM_MIN = -5000; // PWM输出范围限制
int MOTO1, MOTO2;                    // 电机PWM输出值

/* 外部声明 */
extern int Vertical_out, Velocity_out, Turn_out; // 来自control.c的控制量

/*********************
TIM3定时器初始化函数
功能：配置TIM3用于超声波测距计时
*********************/
void TIM3_Int_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 时钟使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 使能TIM3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟

    /* GPIO配置 */
    // PA3配置为推挽输出（实际未使用，可能为历史遗留配置）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA2配置为浮空输入（实际未使用，可能为历史遗留配置）
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 定时器基础配置 */
    TIM_TimeBaseStructure.TIM_Period = 999;         // 自动重装载值（1ms计时）
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;     // 预分频系数（72MHz/7200=10KHz）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* 中断配置 */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);     // 使能更新中断
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断优先级分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, DISABLE); // 初始禁用定时器
}

/*********************
超声波测距函数
功能：使用TIM3测量超声波回波时间并计算距离
返回：三次测量的平均距离（单位：cm）
*********************/
int Senor_Using() 
{
    unsigned int sum = 0;
    unsigned int tim;
    unsigned int i = 0;
    unsigned int length;
    u16 cnt_i = 0;

    while(i != 3) // 进行3次测量取平均
    {
        /* 发送触发脉冲 */
        TRIG = 1;          // 触发信号高电平
        delay_us(20);      // 维持20us
        TRIG = 0;          // 触发信号恢复低电平

        /* 等待回波信号 */
        cnt_i = 0;
        while(ECHO == 0)   // 等待回波信号变高
        {
            cnt_i++;
            delay_us(1);
            if(cnt_i > 30000) // 超时处理（30ms）
            {  
                TRIG = 1;     // 重新发送触发
                delay_us(20);  
                TRIG = 0;
                cnt_i = 0;        
            }
        }
        /* 开始计时 */
        TIM_Cmd(TIM3, ENABLE);  // 启动定时器
        i += 1;                 // 成功测量次数+1

        /* 等待回波结束 */
        while(ECHO == 1);       // 等待回波信号变低
      
        /* 停止计时并计算 */
        TIM_Cmd(TIM3, DISABLE);    // 关闭定时器
        tim = TIM_GetCounter(TIM3); // 获取计数值
        length = (tim * 100) / 58.0; // 换算为厘米（声速340m/s）
        sum = length + sum;         // 累计测量值
      
        /* 重置定时器 */
        TIM3->CNT = 0;        // 计数器归零
        overcount = 0;        // 超时计数器重置
        delay_ms(100);        // 测量间隔
    }
    length = sum / 3; // 计算平均值
    return length; 
}

u8 lock = 1; // 初始化锁定标志（实际未使用）

/*********************
主函数
功能：系统初始化与主循环
*********************/
int main(void)	
{
    /* 基础初始化 */
    delay_init();            // 延时函数初始化
    NVIC_Config();           // 中断优先级配置
    uart1_init(115200);      // 串口1初始化（调试用）
    uart3_init(9600);        // 串口3初始化（可能用于通信）
    // while(lock==1);        // 初始化锁定（被注释）

    /* 外设初始化 */
    OLED_Init();             // OLED显示屏初始化
    OLED_Clear();
    MPU_Init();              // MPU6050初始化
    mpu_dmp_init();          // DMP姿态解算初始化
    MPU6050_EXTI_Init();     // MPU6050中断初始化
    Encoder_TIM2_Init();     // 编码器1（TIM2）初始化
    Encoder_TIM4_Init();     // 编码器2（TIM4）初始化
    Motor_Init();            // 电机驱动初始化
    PWM_Init_TIM1(0, 7199);  // PWM定时器初始化（72MHz/7200=10KHz）

    /* 显示初始化 */
    OLED_ShowString(0, 3, "jiao du:", 12);  // 第3行显示"角度："
    OLED_ShowString(0, 4, "ju li:", 12);    // 第4行显示"距离："
    OLED_ShowString(0, 5, "sd:", 12);       // 第5行显示"速度："

    /* 主循环 */
    while(1)	
    {
        length = Senor_Using(); // 获取超声波测距结果
        /* 数据显示更新 */
        OLED_Float(3, 70, Pitch, 1);        // 显示俯仰角
        OLED_Float(4, 64, length, 1);       // 显示距离值
        OLED_Num3(8, 5, Encoder_Left);      // 显示左编码器值（第5行第8列）
        OLED_Num3(8, 6, Encoder_Right);     // 显示右编码器值（第6行第8列）
    }
}

/*********************
TIM3中断服务函数
功能：处理定时器溢出（用于超声波超时检测）
*********************/
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // 检测更新中断
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志
        overcount++; // 超时计数器递增
    }
}
