#include "stm32f10x.h"                  // Device header"
#include "inv_mpu.h"
#include "delay.h"
#include "OLED.h"
#include "Motor.h"
#include "PID.h"
#include "MPU6050.h"
#include "Serial.h"
#include "HC_SR04.h"

float zhongzhi=4.5;													//��е��ֵ
float balance_KP=-600,balance_KD=-0.9;			//ֱ��������  -600 	 -0.9
float velocity_KP=140,velocity_KI=0.7;		//�ٶȻ�����   140  	  0.7
	
float Pitch,Roll,Yaw; 										//������ ������ ƫ����
int16_t GX,GY,GZ;													//������ԭʼ���ݽ���

int32_t Encoder_Left,Encoder_Right; 			//������ֵ
int32_t Balance_Pwm,Velocity_Pwm,Turn_Pwm;//ֱ�������ٶȻ���ת��PWMֵ
int32_t MOTOA,MOTOB;         				 			//���PWMֵ

//���Ƴ�����ˢ��ʱ��
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


		Serial_yaokong();//����ң�غ���
		if(sj>=20)//���Ƴ�����ˢ��ʱ��
		{
			int16_t HC_SRO4;
			HC_SRO4=(uint16_t)HC_SR04_D();
			OLED_ShowNum(1,7,HC_SRO4,3);				//��������ʾ
			OLED_ShowNum(1,11,HC_SRO4*10%10,1); //��������ʾС������
			HC_SR04_gensui(HC_SRO4);
			sj=0;
		}			
	}
}

/**************************************************************************
�������ܣ��ж�PID   5MS����һ��
**************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12)==SET)
	{
		sj++;//���Ƴ�����ˢ��ʱ��
		if(MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw)==0)						//��ȡ������ ������ ƫ����
		{ 		
			MPU_Get_Gyroscope(&GX,&GY,&GZ);									//������ԭʼ����
			wd=MPU_Get_Temperature();
			Encoder_Left=-read_encoder2();									//������ֵ��ȡ
			Encoder_Right=read_encoder3();	
						
			Balance_Pwm =balance(Pitch,zhongzhi,GY);  						//===ֱ����PID����	
			Velocity_Pwm =velocity(Pitch,Encoder_Left,Encoder_Right);		//===�ٶȻ�PID����
			Turn_Pwm =turn(Encoder_Left,Encoder_Right,GZ); 		  			//===ת��PID����     
	
			MOTOA=Balance_Pwm+Velocity_Pwm-Turn_Pwm;           				//===����A�������PWM
			MOTOB=Balance_Pwm+Velocity_Pwm+Turn_Pwm;          				//===����B�������PWM
				
			PWM_Xianfu(7200,&MOTOA,&MOTOB); 								//===PWM���ֵ�޷�
			PWM_XIANFU(600,&MOTOA,&MOTOB);									//===PWM�����޷�		
			if(Stop_Off(Pitch))		SETPWM(MOTOA,MOTOB);						//===�������40��
		}	
	}	
	EXTI_ClearITPendingBit(EXTI_Line12); 													//���LINE12�ϵ��жϱ�־λ  
}
