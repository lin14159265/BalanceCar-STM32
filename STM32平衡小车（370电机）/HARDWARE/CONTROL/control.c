#include "control.h"  // 包含自定义控制头文件
/*
控制指令说明：
0x00：刹车
0x01：前进
0x05：后退
0x03：左转
0x07：右转
*/

/* 全局变量定义 */
float Med_Angle = -5.8;    // 机械中值角度（平衡点角度），需根据实际硬件调整
float Target_Speed = 0;     // 期望速度（前后方向）
float Turn_Speed = 0;       // 期望转向速度（左右方向）

/* PID控制参数 */
float Vertical_Kp = 250,   // 直立环比例系数
      Vertical_Kd = 1.2;   // 直立环微分系数
float Velocity_Kp = 0.3,   // 速度环比例系数
      Velocity_Ki = 0.002; // 速度环积分系数
float Turn_Kd = 0.6,       // 转向环微分系数
      Turn_Kp = 20;        // 转向环比例系数

extern int length;         // 外部变量声明（可能用于距离检测）

/* 速度限制宏定义 */
#define SPEED_Y 15   // 前后方向最大速度
#define SPEED_Z 80   // 左右转向最大速度

/* 控制环输出变量 */
int Vertical_out, Velocity_out, Turn_out; // 分别存储三个控制环的输出

/* 函数声明 */
int Vertical(float Med, float Angle, float gyro_Y);   // 直立环控制函数
int Velocity(int Target, int encoder_left, int encoder_right); // 速度环控制函数
int Turn(int gyro_Z, int RC);    // 转向环控制函数

/* 外部中断处理函数（PB5引脚中断） */
void EXTI9_5_IRQHandler(void) {
    int PWM_out;
    if(EXTI_GetITStatus(EXTI_Line5) != 0) {  // 检测是否PB5触发中断
        if(PBin(5) == 0) {                   // 二次确认引脚状态（防抖动）
            EXTI_ClearITPendingBit(EXTI_Line5); // 清除中断标志位
          
            /*-- 数据采集部分 --*/
            // 读取左右电机编码器值（注意电机安装方向差异）
            Encoder_Left = Read_Speed(4);  // 左编码器（可能反向安装）
            Encoder_Right = Read_Speed(2); // 右编码器
          
            // 获取MPU6050传感器数据
            mpu_dmp_get_data(&Roll, &Pitch, &Yaw);       // 获取欧拉角
            MPU_Get_Gyroscope(&gyroy, &gyrox, &gyroz);   // 获取陀螺仪角速度
            MPU_Get_Accelerometer(&aacx, &aacy, &aacz);  // 获取加速度计数据

            /*-- 二开部分：处理遥控指令 --*/
            // 前后方向控制
            if((Fore == 0) && (Back == 0)) 
                Target_Speed = 0;  // 无指令时速度归零
            if(Fore == 1)          // 前进指令
                Target_Speed--;    // 减少目标速度（实际方向需根据电机极性确认）
            if(Back == 1) {       // 后退指令
                if(length < 50)    // 可能的安全距离检测
                    Target_Speed--;
                else 
                    Target_Speed++;
            }
            // 速度限幅（-SPEED_Y到SPEED_Y之间）
            Target_Speed = Target_Speed > SPEED_Y ? SPEED_Y : 
                          (Target_Speed < -SPEED_Y ? -SPEED_Y : Target_Speed);

            // 转向控制
            if((Left == 0) && (Right == 0)) 
                Turn_Speed = 0;   // 无转向指令时归零
            if(Left == 1)         // 左转指令
                Turn_Speed += 30; // 增加左转速度
            if(Right == 1)        // 右转指令
                Turn_Speed -= 30; // 减少转向速度（负值表示右转）
            // 转向速度限幅
            Turn_Speed = Turn_Speed > SPEED_Z ? SPEED_Z : 
                        (Turn_Speed < -SPEED_Z ? -SPEED_Z : Turn_Speed);

            // 转向阻尼调整（无指令时启用阻尼，有指令时关闭）
            if((Left == 0) && (Right == 0)) 
                Turn_Kd = -0.6;   // 转向阻尼系数
            else 
                Turn_Kd = 0;      // 关闭阻尼

            /*-- 控制环计算 --*/
            Velocity_out = Velocity(Target_Speed, Encoder_Left, Encoder_Right); // 速度环
            Vertical_out = Vertical(Velocity_out + Med_Angle, Pitch, gyroy);    // 直立环
            Turn_out = Turn(gyroz, Turn_Speed);                                  // 转向环

            /*-- 电机输出处理 --*/
            PWM_out = Vertical_out;              // 综合控制量
            MOTO1 = PWM_out - Turn_out;          // 左电机输出（叠加转向量）
            MOTO2 = PWM_out + Turn_out;          // 右电机输出
            Limit(&MOTO1, &MOTO2);               // PWM限幅保护（防止过压）
            Load(MOTO1, MOTO2);                  // 加载PWM到电机驱动
          
            Stop(&Med_Angle, &Pitch);            // 安全检测（如倾角过大时停止）
        }
    }
}

/* 直立环PD控制器 */
int Vertical(float Med, float Angle, float gyro_Y) {
    int PWM_out;
    // PD计算：比例项*(当前角度-平衡角度) + 微分项*角速度Y轴分量
    PWM_out = Vertical_Kp * (Angle - Med) + Vertical_Kd * (gyro_Y - 0);
    return PWM_out;
}

/* 速度环PI控制器 */
int Velocity(int Target, int encoder_left, int encoder_right) {
    static int Encoder_S, EnC_Err_Lowout_last, PWM_out, Encoder_Err, EnC_Err_Lowout;
    float a = 0.7;  // 低通滤波系数
  
    // 计算速度误差（实际速度与目标速度之差）
    Encoder_Err = ((encoder_left + encoder_right) - Target);
  
    // 低通滤波处理误差信号
    EnC_Err_Lowout = (1 - a) * Encoder_Err + a * EnC_Err_Lowout_last;
    EnC_Err_Lowout_last = EnC_Err_Lowout;
  
    // 误差积分（带限幅）
    Encoder_S += EnC_Err_Lowout;
    Encoder_S = Encoder_S > 10000 ? 10000 : 
               (Encoder_S < -10000 ? -10000 : Encoder_S);
  
    // 急停时清空积分
    if(stop == 1) {
        Encoder_S = 0;
        stop = 0;
    }
  
    // PI计算：比例项 + 积分项
    PWM_out = Velocity_Kp * EnC_Err_Lowout + Velocity_Ki * Encoder_S;
    return PWM_out;
}

/* 转向环控制器 */
int Turn(int gyro_Z, int RC) {
    int PWM_out;
    // 转向控制：陀螺仪Z轴分量*Kd + 遥控指令*Kp
    PWM_out = Turn_Kd * gyro_Z + Turn_Kp * RC;
    return PWM_out;
}
