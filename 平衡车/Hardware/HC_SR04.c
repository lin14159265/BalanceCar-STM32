#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "oled.h"

uint8_t  	TIM4CH1_CAPTURE_STA;	//���벶��״̬		    				
uint16_t	TIM4CH1_CAPTURE_VAL;	//���벶��ֵ

void HC_SR04_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	TIM_InternalClockConfig(TIM4); //�ڲ�ʱ�ӿ���
	
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
	
	TIM_TimeBaseInitTypeDef TIM_Time;			//ʱ����Ԫ��ʼ��
	TIM_Time.TIM_ClockDivision=TIM_CKD_DIV1;	//����Ԥ��ƵΪ����Ƶ
	TIM_Time.TIM_CounterMode=TIM_CounterMode_Up;//����ģʽ
	TIM_Time.TIM_Period=65536-1;				//����ARR
	TIM_Time.TIM_Prescaler=72-1;				//Ԥ��Ƶ��PSC
	TIM_Time.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4,&TIM_Time);			//��ʼ��

	TIM_ICInitTypeDef IC_Init;
	IC_Init.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	IC_Init.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	IC_Init.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	IC_Init.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	IC_Init.TIM_ICFilter = 0x00;			//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM4, &IC_Init);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//���ȼ���������Ϊ2��
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;//�ж�ͨ��ѡ��
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC1IE�����ж�	
	TIM_Cmd(TIM4,ENABLE ); 								//ʹ�ܶ�ʱ��4	
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
		if(TIM4CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
		{	
			temp=TIM4CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//���ʱ���ܺ�
			temp+=TIM4CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
			TIM4CH1_CAPTURE_STA=0;			//������һ�β���
			//�����Echo�ڸߵ�ƽ��ʱ�䣬��λ��us
			//d = 340m/s*time/2,�����Ĵ���������340m/s��Ҫͳһ��λ
			//340m/s = 34000cm/s = 34cm/ms = 0.034cm/us	,	0.034cm/us/2 =0.017cm/us; 
			ds+=(float)temp*0.017;
			delay_ms(10);
		}
	}	
	ds/=3;
	
	return ds;
}

//��ʱ��5�жϷ������	 
void TIM4_IRQHandler(void)
{
 	if((TIM4CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM4CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM4CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM4CH1_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
					TIM4CH1_CAPTURE_VAL=0XFFFF;
				}else TIM4CH1_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//����1���������¼�
		{	
			if(TIM4CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM4CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ��������
				TIM4CH1_CAPTURE_VAL=TIM_GetCapture1(TIM4);
		   	TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}
			else			//��δ��ʼ,��һ�β���������
			{
				TIM4CH1_CAPTURE_STA=0;			//���
				TIM4CH1_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM4,0);
				TIM4CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
		   	TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			}		    
		}			     	    					   
 	}
 
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update);//����жϱ�־λ 
}
