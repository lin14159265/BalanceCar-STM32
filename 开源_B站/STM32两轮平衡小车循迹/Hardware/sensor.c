#include "stm32f10x.h"               
#include "motor.h"
#include "Delay.h"

/*引用主函数的参数*/
extern float err,err0;
extern int16_t circle,flag,turn,flagt1,flagt2,outt;
extern int32_t jc;
	
void Sensor_InitA(void){//传感器初始化(PA0,PA1,PA4,PA5)八路循迹
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Sensor_InitB(void){//传感器初始化(PB0,PB1,PB8,PB9)八路循迹
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Sensor_Init(void)
{
	Sensor_InitA();
	Sensor_InitB();
}

void Sensor_JC(void){
	jc=0;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0)//out1
		jc+=10000000;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==0)//out2
		jc+=1000000;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)==0)//out3
		jc+=100000;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)==0)//out4
		jc+=10000;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)==0)//out5
		jc+=1000;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)==0)//out6
		jc+=100;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)==0)//out7
		jc+=10;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)//out8
		jc+=1;
	err=0;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0)//out1
		err+=1000;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==0)//out2
		err+=500;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)==0)//out3
		err+=300;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)==0)//out4
		err+=100;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)==0)//out5
		err-=100;
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)==0)//out6
		err-=300;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)==0)//out7
		err-=500;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)//out8
		err-=1000;
	
	if(jc==00000000)
		err=err0;
		err0=err;

	/*直角转弯*/
	if(jc==11110000||jc==11111000||jc==11111000)
	{
		err=5000;
		if(turn==0)
		{
			err=0;
			if(flagt1==0)
				flagt2++;
			flagt1=1;
		}
	}
	else if(jc==00001111||jc==0001111||jc==0011111)
	{
		err=-5000;
		if(turn==0)
		{
			err=0;
			if(flagt1==0)
				flagt2++;
			flagt1=1;
		}
		
	}
	else
		{
			if(turn==0)
			{	
				if(flagt1==1)
					flagt2++;
				if(flagt2==2)
						turn=1;
				flagt1=0;
			}
		}
}
void Sensor_JC_circle(void){
	if(jc==11111111||jc==01111110||jc==01111111||jc==11111110)
		{
			if(flag==0) 
			{
				circle++;
				flag=1;
			}
			if(circle==3)
			{
				Motor_Z_SetSpeed(0);
				Motor_Y_SetSpeed(0);
				Delay_s (100);
			}
		}
		else flag=0;
}
