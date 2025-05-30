#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_AW(void) //电机A前
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}

void Motor_AS(void) //电机A后
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);

}

void Motor_BW(void) //电机B前
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);

}

void Motor_BS(void) //电机B后
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
	if(MOTOA>0) //电机A正转
	{
		Motor_AW();
		TIM_SetCompare1(TIM1,MOTOA);    
	}
	else      	//电机A反转
	{
		Motor_AS();
		TIM_SetCompare1(TIM1,-MOTOA );       
	}	
	if(MOTOB>0) //电机B正转
	{
		Motor_BW();
		TIM_SetCompare4(TIM1,MOTOB);     
	}
	else      	//电机B反转
	{
		Motor_BS();  
		TIM_SetCompare4(TIM1,-MOTOB );      
	}	

}


void PWM_Xianfu(int32_t max,int32_t *MOTOA,int32_t *MOTOB)//电机最大值限幅
{
	if(*MOTOA>max)  *MOTOA = max;
	if(*MOTOA<-max) *MOTOA =-max;
	if(*MOTOB>max)  *MOTOB = max;
	if(*MOTOB<-max) *MOTOB =-max;
} 

void PWM_XIANFU(int32_t max,int32_t *MOTOA,int32_t *MOTOB)//电机死区限幅
{
	if(*MOTOA>max-400 && *MOTOA<max)  *MOTOA = max;
	else if(*MOTOA<-(max-400) && *MOTOA>-max)  *MOTOA = -max;
	if(*MOTOB>max-400 && *MOTOB<max)  *MOTOB = max;
	else if(*MOTOB<-(max-400) && *MOTOB>-max)  *MOTOB = -max;

}


int32_t read_encoder2(void)
{
	int32_t speed;
	speed = (short)TIM_GetCounter(TIM2); //1.采集编码器的计数值并保存
	TIM_SetCounter(TIM2,0);              //2.将定时器的计数值清零
	
	return speed;
}


int32_t read_encoder3(void)
{
	int32_t speed;
	speed = (short)TIM_GetCounter(TIM3); //1.采集编码器的计数值并保存
	TIM_SetCounter(TIM3,0);              //2.将定时器的计数值清零
	
	return speed;
}
