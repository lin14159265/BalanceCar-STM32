#include "control.h"



/*定义全局变量*/
float Med_Angle=-5.8;
float Target_Speed=0;
float Turn_Speed=0;

/*定义PID控制参数变量*/
//直立环
float Vertical_Kp=280;
float Vertical_Kd=1.15;
//速度环
float Velocity_Kp=0.3;
float Velocity_Ki=0.0015;
//转向环
float Turn_Kp=0;
float Turn_Kd=0;

/*定义外部距离变量*/
extern uint8_t Length;

/* 速度限制宏定义 */
#define SPEED_Y 15    //前后
#define SPEED_Z 80    //左右

/* 控制环输出变量 */
int Vertical_PD_Out, Velocity_PI_Out, Turn_PD_Out;

/* 三环函数声明 */
int Vertical(float Med, float Angle, float gyro_Y);
int Velocity(int Target, int encoder_left, int encoder_right);
int Turn(int gyro_Z, int RC);

/* 外部中断处理函数（PB5引脚中断） */
void EXTI9_5_IRQHandler(void)
{
	int PWM_Out;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)
	{
		if(PBin(5)==0)
		{
			//清除中断标志位
			EXTI_ClearITPendingBit(EXTI_Line5);
			
			/*-- 数据采集部分 --*/
			// 读取左右电机编码器值
			Encoder_Left=Read_Speed(4);
			Encoder_Right=Read_Speed(2);
			
			// 获取MPU6050传感器数据
			mpu_dmp_get_data(&Roll, &Pitch, &Yaw);  //获取欧拉角
			MPU_Get_Gyroscope(&gyroy,&gyrox,&gyroz);//获取陀螺仪角速度
			MPU_Get_Accelerometer(&aacx, &aacy, &aacz);// 获取加速度计数据
			
			/*-- 二开部分：处理遥控指令 --*/
			//前后方向控制
			if((Fore==0)&&(Back==0))
			{
				Target_Speed=0;//无前进后退指令则速度归零
			}
			if((Fore==1)&&(Back!=1))//前进标志位置1，则前进
			{
				Target_Speed--;  //减少目标速度
			}
			if((Back==1)&&(Fore!=1))//后退标志位置1，则后退
			{
				if(Length<50)
				{
					Target_Speed--;
				}
				else
				{
					Target_Speed++;
				}
			}
			
			
			// 速度限幅（-SPEED_Y到SPEED_Y之间）
			Target_Speed=Target_Speed>SPEED_Y?SPEED_Y:(Target_Speed<-SPEED_Y?-SPEED_Y:Target_Speed);
//			CLAMP_SPEED(Target_Speed, SPEED_Y);  // 自动处理正负限幅
			
			
			//转向控制
			if((Left==0)&&(Right==0))
			{
				Turn_Speed=0;          // 无转向指令时归零
			}
			if(Left==1)                // 左转指令
			{
				Turn_Speed+=30;        // 增加左转速度
			}
			if(Right==1)               // 右转指令
			{
				Turn_Speed-=30;        // 减少转向速度（负值表示右转）
			}
			
			// 转向速度限幅
			Turn_Speed = Turn_Speed > SPEED_Z ? SPEED_Z : (Turn_Speed < -SPEED_Z ? -SPEED_Z : Turn_Speed);
//			CLAMP_SPEED(Turn_Speed, SPEED_Z);  // 自动处理正负限幅
			
			
			// 转向阻尼调整（无指令时启用阻尼，有指令时关闭）
			if((Left==0)&&(Right==0))
			{
				Turn_Kd=0;//转向阻尼系数,实际值待定
			}
			else
			{
				Turn_Kd=0;// 关闭阻尼
			}
			
			/*-- 控制环计算 --*/
			Velocity_PI_Out=Velocity(Target_Speed, Encoder_Left, Encoder_Right);// 直立环控制函数
			Vertical_PD_Out=Vertical(Velocity_PI_Out + Med_Angle, Pitch, gyroy);// 速度环控制函数
			Turn_PD_Out=Turn(gyroz, Turn_Speed);                                // 转向环控制函数
			
			/*-- 电机输出处理 --*/
			PWM_Out=Vertical_PD_Out;       //综合控制量
			MOTO1=PWM_Out-Turn_PD_Out;
			MOTO2=PWM_Out+Turn_PD_Out;     
			Limit(&MOTO1,&MOTO2);          //PWM限幅保护(防止过压)
			Load(MOTO1,MOTO2);             //将PWM输入电机驱动
			
			Stop(&Med_Angle, &Pitch);
			
		}
	}
}


/* 以下是三个控制器 */

/* 一：直立环PD控制器 */
int Vertical(float Med, float Angle, float Gyro_Y)
{
	int PWM_Out;
	//PD计算：比例项*(当前实际角度-目标平衡角度)+微分项*角加速度Y分量(为什么不用角度变化率？ 陀螺仪直接输出角速度，相当于角度的微分，因此无需额外计算)
	PWM_Out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(Gyro_Y-0);//这里的角速度Y轴分量为什么要减去0？
	return PWM_Out;
}

/* 二：速度环PI控制器 */
/*
为什么用PI而不是PID？
速度环需要消除稳态误差（如车体长期轻微偏移），积分项（I）可以解决此问题。微分项（D）容易引入高频噪声，因此省略。
*/
int Velocity(int Target, int encoder_left, int encoder_right)
{
	static int Encoder_Err, EnC_Err_Lowout, EnC_Err_Lowout_last, Encoder_S, PWM_Out;
	float a=0.7;//低通滤波系数
	
	// 计算速度误差（实际速度与目标速度之差）	
	Encoder_Err=((encoder_left+encoder_right)-Target);
	
	// 低通滤波处理误差信号
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;
	EnC_Err_Lowout_last=EnC_Err_Lowout;
	
	// 误差积分（带限幅）
	Encoder_S+=EnC_Err_Lowout;
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<-10000?-10000:Encoder_S);
	
	// 急停时清空积分
	if(stop==1)
	{
		Encoder_S=0;
		stop=0;
	}
	
	//速度环PI计算：比例项+积分项
	PWM_Out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;
	return PWM_Out;
}
	
/* 三：转向环控制器 */
int Turn(int gyro_Z, int RC)
{
	int PWM_Out;
	// 转向控制：陀螺仪Z轴分量*Kd + 遥控指令*Kp
	PWM_Out=Turn_Kd*gyro_Z+Turn_Kp*RC;
	return PWM_Out;
}


