#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_AW(void) //���Aǰ
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}

void Motor_AS(void) //���A��
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);

}

void Motor_BW(void) //���Bǰ
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);

}

void Motor_BS(void) //���B��
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	PWM_Init();
	TIM2_encoder_init();
	TIM3_encoder_init();
}

void SETPWM(int32_t MOTOA,int32_t MOTOB)
{
	if(MOTOA>0) //���A��ת
	{
		Motor_AW();
		TIM_SetCompare1(TIM1,MOTOA);    
	}
	else      	//���A��ת
	{
		Motor_AS();
		TIM_SetCompare1(TIM1,-MOTOA );       
	}	
	if(MOTOB>0) //���B��ת
	{
		Motor_BW();
		TIM_SetCompare4(TIM1,MOTOB);     
	}
	else      	//���B��ת
	{
		Motor_BS();  
		TIM_SetCompare4(TIM1,-MOTOB );      
	}	

}


void PWM_Xianfu(int32_t max,int32_t *MOTOA,int32_t *MOTOB)//������ֵ�޷�
{
	if(*MOTOA>max)  *MOTOA = max;
	if(*MOTOA<-max) *MOTOA =-max;
	if(*MOTOB>max)  *MOTOB = max;
	if(*MOTOB<-max) *MOTOB =-max;
} 

void PWM_XIANFU(int32_t max,int32_t *MOTOA,int32_t *MOTOB)//��������޷�
{
	if(*MOTOA>max-400 && *MOTOA<max)  *MOTOA = max;
	else if(*MOTOA<-(max-400) && *MOTOA>-max)  *MOTOA = -max;
	if(*MOTOB>max-400 && *MOTOB<max)  *MOTOB = max;
	else if(*MOTOB<-(max-400) && *MOTOB>-max)  *MOTOB = -max;

}


int32_t read_encoder2(void)
{
	int32_t speed;
	speed = (short)TIM_GetCounter(TIM2); //1.�ɼ��������ļ���ֵ������
	TIM_SetCounter(TIM2,0);              //2.����ʱ���ļ���ֵ����
	
	return speed;
}


int32_t read_encoder3(void)
{
	int32_t speed;
	speed = (short)TIM_GetCounter(TIM3); //1.�ɼ��������ļ���ֵ������
	TIM_SetCounter(TIM3,0);              //2.����ʱ���ļ���ֵ����
	
	return speed;
}
