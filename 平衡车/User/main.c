#include "stm32f10x.h"                  // Device header"
#include "inv_mpu.h"
#include "delay.h"
#include "OLED.h"
#include "Motor.h"
#include "PID.h"
#include "MPU6050.h"
#include "Serial.h"
#include "HC_SR04.h"

float zhongzhi=4.5;													//机械中值
float balance_KP=-600,balance_KD=-0.9;			//直立环参数  -600 	 -0.9
float velocity_KP=140,velocity_KI=0.7;		//速度环参数   140  	  0.7
	
float Pitch,Roll,Yaw; 										//俯仰角 翻滚角 偏航角
int16_t GX,GY,GZ;													//陀螺仪原始数据接收

int32_t Encoder_Left,Encoder_Right; 			//编码器值
int32_t Balance_Pwm,Velocity_Pwm,Turn_Pwm;//直立环，速度环，转向环PWM值
int32_t MOTOA,MOTOB;         				 			//电机PWM值

//限制超声波刷新时间
uint8_t sj=30,wd;

int main(void)
{
	delay_init(); 
	OLED_Init();
	Serial_Init();
	Motor_Init();
	HC_SR04_Init();
	MPU_Init();
	
	OLED_ShowString(1,1,"DMP...");
	OLED_ShowString(2,1,"Loading.....");
	MPU6050_DMP_Init();
	OLED_Clear();
	MPU_Exti_Init();
	
	OLED_ShowString(1,4,".C");
	OLED_ShowString(1,10,".");
	OLED_ShowString(1,12,"cm");
	while(1)
	{	
		OLED_ShowSignedNum(1,1,wd,2);
		OLED_ShowSignedNum(2,1,Pitch*10,3);
		OLED_ShowSignedNum(3,1,Roll,3);
		OLED_ShowSignedNum(4,1,Yaw,3);
		OLED_ShowSignedNum(3,8,MOTOA,4);
		OLED_ShowSignedNum(4,8,MOTOB,4);


		Serial_yaokong();//蓝牙遥控函数
		if(sj>=20)//限制超声波刷新时间
		{
			int16_t HC_SRO4;
			HC_SRO4=(uint16_t)HC_SR04_D();
			OLED_ShowNum(1,7,HC_SRO4,3);				//超声波显示
			OLED_ShowNum(1,11,HC_SRO4*10%10,1); //超声波显示小数部分
			HC_SR04_gensui(HC_SRO4);
			sj=0;
		}			
	}
}

/**************************************************************************
函数功能：中断PID   5MS进入一次
**************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12)==SET)
	{
		sj++;//限制超声波刷新时间
		if(MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw)==0)						//读取俯仰角 翻滚角 偏航角
		{ 		
			MPU_Get_Gyroscope(&GX,&GY,&GZ);									//陀螺仪原始数据
			wd=MPU_Get_Temperature();
			Encoder_Left=-read_encoder2();									//编码器值读取
			Encoder_Right=read_encoder3();	
						
			Balance_Pwm =balance(Pitch,zhongzhi,GY);  						//===直立环PID控制	
			Velocity_Pwm =velocity(Pitch,Encoder_Left,Encoder_Right);		//===速度环PID控制
			Turn_Pwm =turn(Encoder_Left,Encoder_Right,GZ); 		  			//===转向环PID控制     
	
			MOTOA=Balance_Pwm+Velocity_Pwm-Turn_Pwm;           				//===计算A电机最终PWM
			MOTOB=Balance_Pwm+Velocity_Pwm+Turn_Pwm;          				//===计算B电机最终PWM
				
			PWM_Xianfu(7200,&MOTOA,&MOTOB); 								//===PWM最大值限幅
			PWM_XIANFU(600,&MOTOA,&MOTOB);									//===PWM死区限幅		
			if(Stop_Off(Pitch))		SETPWM(MOTOA,MOTOB);						//===拿起检测±40°
		}	
	}	
	EXTI_ClearITPendingBit(EXTI_Line12); 													//清除LINE12上的中断标志位  
}
