#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "delay.h"
#include "HC_SR04.h"
#include "Motor.h"

char Serial_RxPacket[100];
uint8_t Serial_RxFlag;


//蓝牙遥控相关的变量
uint8_t Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; 
uint8_t Flag_Stop=0,Flag_gensui=0,Flag_sudu=7,Flag_SS=0,Flag_ON=0;


void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开启USART
	
	GPIO_InitTypeDef	GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;//波特率
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//串口模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//字长
	USART_Init(USART1,&USART_InitStructure);//初始化
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启USART的中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级分组设置为2组
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel= USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}

void Serial_SendByte(uint8_t Byte)//发送一个字节
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	
}

void Serial_SendArray( uint8_t *Array, uint16_t Length)//连续发送数组（hex）
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
		
	}
}

void Serial_SendString(char *String)//发送文本
{
	uint8_t i;
	for(i=0; String[i]!='\0'; i++)
	{
		Serial_SendByte(String[i]);
		
	}
}

uint32_t Serial_Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result*=X;
	}
	return Result;
	
}

void Serial_SendNumber(uint32_t Number,uint8_t Length)//连续发数字
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Number/Serial_Pow(10,Length-i-1)%10+'0');
	}
}

int fputc(int ch,FILE *f)//fputc重定向到指定串口
{
	Serial_SendByte(ch);
	return ch;
	
}

void Serial_Printf(char *format, ...)//任意串口可以使用
{
	char String[200];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}
/**************************************************************************
函数功能：蓝牙遥控函数
**************************************************************************/
void Serial_yaokong(void)
{
		if(Serial_RxFlag==1)
		{
//=============遥控前进后退左右=======================//
			if(strcmp(Serial_RxPacket,"LANYA_W")==0)
			{
				Serial_SendString("前进中\r\n");
				Flag_Qian=1;
				Flag_Hou=0;
				Flag_Left=0;
				Flag_Right=0;
			}
			if(strcmp(Serial_RxPacket,"LANYA_S")==0)
			{
				Serial_SendString("后退中\r\n");
				Flag_Qian=0;
				Flag_Hou=1;
				Flag_Left=0;
				Flag_Right=0;
			}
			if(strcmp(Serial_RxPacket,"LANYA_A")==0)
			{
				Serial_SendString("左转中\r\n");
				Flag_Qian=0;
				Flag_Hou=0;
				Flag_Left=1;
				Flag_Right=0;
			}
			if(strcmp(Serial_RxPacket,"LANYA_D")==0)
			{
				Serial_SendString("右转中\r\n");
				Flag_Qian=0;
				Flag_Hou=0;
				Flag_Left=0;
				Flag_Right=1;
			}			
//=============遥控电机开启关闭=======================//			
			if(strcmp(Serial_RxPacket,"LANYA_ON")==0)
			{
				Serial_SendString("\r\n鲤鱼打挺");
				Flag_Stop=0;
				Flag_ON=1;
			}
			if(strcmp(Serial_RxPacket,"LANYA_OFF")==0)
			{
				Serial_SendString("\r\n电机已关闭");
				Flag_Stop=1;
				Flag_ON=0;
			}
//=============遥控电机速度=======================//			
			if(strcmp(Serial_RxPacket,"LANYA_SUDU1")==0)
			{
				Serial_SendString("\r\n速度切换为正常");
				Flag_sudu=7;
			}
			if(strcmp(Serial_RxPacket,"LANYA_SUDU2")==0)
			{
				Serial_SendString("\r\n速度切换为最大");
				Flag_sudu=9;
			}		
//=============遥控超声波跟随开关=======================//			
			if(strcmp(Serial_RxPacket,"LANYA_GENSUI")==0)
			{
				Flag_gensui=1;
				Flag_sudu=4;
				Serial_SendString("小车跟随中\r\n");
			}	
			if(strcmp(Serial_RxPacket,"LANYAGENSUI_STOP")==0)
			{
				Flag_gensui=0;
				Flag_sudu=7;
				Flag_SS=1;				
				Serial_SendString("跟随关闭\r\n");
			}
//=============按键松手检测标志位=======================//			
			if(strcmp(Serial_RxPacket,"LANYASTOP")==0)
			{
				Serial_SendString("待机中\r\n");
				Flag_SS=1;				
				Flag_Qian=0;
				Flag_Hou=0;
				Flag_Left=0;
				Flag_Right=0;
			}
			if(strcmp(Serial_RxPacket,"LANYAZZSTOP")==0)//旋转待机不给惯性抵消
			{
				Serial_SendString("待机中\r\n");
				Flag_Qian=0;
				Flag_Hou=0;
				Flag_Left=0;
				Flag_Right=0;
			}
//=============蓝牙发送状态信息=======================//			
			if(strcmp(Serial_RxPacket,"LANYA_ZHUANGTAI")==0)
			{
				Serial_SendString("目前动力：");
				if(Flag_sudu==7)	Serial_SendString("正常速度");
				else if(Flag_sudu==10)	Serial_SendString("最大速度");
				Serial_SendString("\r\n当前超声波距离：");
				Serial_SendNumber(HC_SR04_D(),2);
				Serial_SendString(".");
				Serial_SendNumber((uint16_t)(HC_SR04_D()*10)%10,1);
				Serial_SendString("cm");
				Serial_SendString("\r\n当前转速：");
				Serial_SendNumber(read_encoder2(),3);
				Serial_SendString("r/min");				
			}
		}
		Serial_RxFlag=0;
}
void HC_SR04_gensui(int16_t HC_SRO4)
{
//=============遥控超声波跟随=====================//					
		if(Flag_gensui==1)
			{
				if(HC_SRO4<=20 && HC_SRO4>=15)
				{
					Flag_Qian=0;
					Flag_Hou=0;
					Flag_Left=0;
					Flag_Right=0;
				}
				else if(HC_SRO4>20)
				{
					Flag_Qian=1;
					Flag_Hou=0;
					Flag_Left=0;
					Flag_Right=0;
				}
				else if(HC_SRO4<15)
				{
					Flag_Qian=0;
					Flag_Hou=1;
					Flag_Left=0;
					Flag_Right=0;
				}
			}
}
	
void USART1_IRQHandler(void)//接收包头判断 @起始，#\n结束
{
	static uint8_t RxState=0;
	static uint8_t pRxPacket=0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		uint8_t RxData=USART_ReceiveData(USART1);
		
		if(RxState==0)
		{
			if(RxData=='@' && Serial_RxFlag==0)
			{
				RxState=1;
				pRxPacket=0;
			}
		}
		else if(RxState==1)
		{
			if(RxData=='#')
			{
				RxState=2;
			}
			else 
			{
				Serial_RxPacket[pRxPacket]=RxData;
				pRxPacket++;			
			}
		}
		else if(RxState==2)
		{
			if(RxData=='\n')
			{
				RxState=0;
				Serial_RxPacket[pRxPacket]='\0';
				Serial_RxFlag=1;
			}			

		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
	
}
