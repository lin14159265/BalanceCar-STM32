#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "oled.h"

uint8_t  	TIM4CH1_CAPTURE_STA;	//输入捕获状态		    				
uint16_t	TIM4CH1_CAPTURE_VAL;	//输入捕获值

void HC_SR04_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIM_InternalClockConfig(TIM4); //内部时钟开启
	
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
	
	TIM_TimeBaseInitTypeDef TIM_Time;			//时基单元初始化
	TIM_Time.TIM_ClockDivision=TIM_CKD_DIV1;	//设置预分频为不分频
	TIM_Time.TIM_CounterMode=TIM_CounterMode_Up;//上拉模式
	TIM_Time.TIM_Period=65536-1;				//周期ARR
	TIM_Time.TIM_Prescaler=72-1;				//预分频器PSC
	TIM_Time.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4,&TIM_Time);			//初始化

	TIM_ICInitTypeDef IC_Init;
	IC_Init.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	IC_Init.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	IC_Init.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	IC_Init.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	IC_Init.TIM_ICFilter = 0x00;			//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM4, &IC_Init);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级分组设置为2组
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;//中断通道选择
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;//响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	TIM_Cmd(TIM4,ENABLE ); 								//使能定时器4	
}

void HC_SR04_Strat(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	delay_us(20);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

float HC_SR04_D(void)
{
	uint8_t i;
	uint32_t temp;
	float ds;
	
	
	for(i=0;i<3;i++)
	{
		HC_SR04_Strat();
		if(TIM4CH1_CAPTURE_STA&0X80)//成功捕获到了一次高电平
		{	
			temp=TIM4CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//溢出时间总和
			temp+=TIM4CH1_CAPTURE_VAL;		//得到总的高电平时间
			TIM4CH1_CAPTURE_STA=0;			//开启下一次捕获
			//计算出Echo口高电平的时间，单位是us
			//d = 340m/s*time/2,声音的传播速率是340m/s，要统一单位
			//340m/s = 34000cm/s = 34cm/ms = 0.034cm/us	,	0.034cm/us/2 =0.017cm/us; 
			ds+=(float)temp*0.017;
			delay_ms(10);
		}
	}	
	ds/=3;
	
	return ds;
}

//定时器5中断服务程序	 
void TIM4_IRQHandler(void)
{
 	if((TIM4CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM4CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM4CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM4CH1_CAPTURE_STA|=0X80;//标记成功捕获了一次
					TIM4CH1_CAPTURE_VAL=0XFFFF;
				}else TIM4CH1_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			if(TIM4CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM4CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
				TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4);
		   	TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}
			else			//还未开始,第一次捕获上升沿
			{
				TIM4CH1_CAPTURE_STA=0;			//清空
				TIM4CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
		   	TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update);//清除中断标志位 
}
