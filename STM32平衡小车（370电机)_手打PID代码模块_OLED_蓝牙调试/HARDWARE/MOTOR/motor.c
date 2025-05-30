#include "motor.h"

// 电机控制模块初始化函数
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;  // 声明GPIO初始化结构体
  
    // 启用GPIOB外设时钟(APB2总线)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置GPIO参数：
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // 同时初始化12-15号引脚
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  // 最大输出速率50MHz
  
    // 应用配置到GPIOB端口
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// PWM值限幅函数（防止超出允许范围）
void Limit(int *motoA, int *motoB)
{
    // 限制电机A的PWM值在[PWM_MIN, PWM_MAX]之间
    if(*motoA > PWM_MAX) *motoA = PWM_MAX;
    if(*motoA < PWM_MIN) *motoA = PWM_MIN;
  
    // 限制电机B的PWM值在[PWM_MIN, PWM_MAX]之间
    if(*motoB > PWM_MAX) *motoB = PWM_MAX;
    if(*motoB < PWM_MIN) *motoB = PWM_MIN;
}

// 绝对值计算函数（用于PWM信号处理）
int GFP_abs(int p)
{
    int q;  // 临时存储计算结果
    q = p > 0 ? p : (-p);  // 三元运算符实现绝对值计算
    return q;
}

// 电机负载控制函数（核心控制逻辑）
void Load(int moto1, int moto2)
{
    /* 电机1方向控制逻辑 */
    if(moto1 > 0) {
        Ain1 = 1;  // 正转信号线高电平
        Ain2 = 0;  // 正转信号线低电平
    } else {
        Ain1 = 0;  // 反转信号线低电平
        Ain2 = 1;  // 反转信号线高电平
    }
    // 设置TIM1通道1的比较寄存器值（生成PWM波形）
    TIM_SetCompare1(TIM1, GFP_abs(moto1));

    /* 电机2方向控制逻辑 */
    if(moto2 > 0) {
        Bin1 = 0;  // 正转信号线低电平
        Bin2 = 1;  // 正转信号线高电平
    } else {
        Bin1 = 1;  // 反转信号线高电平
        Bin2 = 0;  // 反转信号线低电平
    }
    // 设置TIM1通道4的比较寄存器值（生成PWM波形）
    TIM_SetCompare4(TIM1, GFP_abs(moto2));
}

// 全局变量声明
char PWM_Zero = 0;  // 零PWM值定义
char stop = 0;      // 急停标志位

// 安全停止函数（角度异常保护）
void Stop(float *Med_Jiaodu, float *Jiaodu)
{
    // 当实际角度与目标角度偏差超过60度时触发急停
    if(GFP_abs(*Jiaodu - *Med_Jiaodu) > 60) {
        // 向两个电机发送零PWM信号
        Load(PWM_Zero, PWM_Zero);
        stop = 1;  // 设置急停标志位
    }
}
