#include "stm32f10x.h"                  // Device header
#define FILTERING_TIMES  10

extern float balance_KP,balance_KD;
extern float velocity_KP,velocity_KI;
extern uint8_t Flag_Qian,Flag_Hou,Flag_Left,Flag_Right;	//蓝牙方向遥控
extern uint8_t Flag_Stop,Flag_sudu,Flag_gensui,Flag_SS,Flag_ON; //蓝牙控制标志位
	
/**************************************************************************
函数功能：异常关闭电机
入口参数：倾角和电压
返回  值：1：异常  0：正常 -70,+89
**************************************************************************/
uint8_t Stop_Off(float angle)
{
	uint8_t temp; //0 正常标志 1 停止运行标志
	if(((angle<-40||angle>40)||Flag_Stop==1) && Flag_ON==0 )		//===倾角大于40度关闭电机
	{	                                                 
		temp=0; 										
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);	
	}
	else if((angle>-5 && angle<5) && Flag_ON==1)	Flag_ON=0;
	else
	temp=1;	
	return temp;				
}

/**************************************************************************
函数功能：速度滤波
入口参数：速度
返回  值：滤波后的速度
**************************************************************************/
int32_t Mean_Filter(int32_t MOTO)
{
  u8 i;
  s32 Sum_Speed = 0; 
	s16 Filter_Speed;
  static  s16 Speed_Buf[FILTERING_TIMES]={0};
  for(i = 1 ; i<FILTERING_TIMES; i++)
  {
    Speed_Buf[i - 1] = Speed_Buf[i];
  }
  Speed_Buf[FILTERING_TIMES - 1] =MOTO;

  for(i = 0 ; i < FILTERING_TIMES; i++)
  {
    Sum_Speed += Speed_Buf[i];
  }
  Filter_Speed = (s16)(Sum_Speed / FILTERING_TIMES);
	return Filter_Speed;
}

/**************************************************************************
函数功能：直立PID控制
入口参数：角度，机械中值，角速度
返回  值：直立控制PWM
**************************************************************************/
int32_t balance(float Angle,float zhongzhi,float Gyro)
{  
	 float Bias;
	 int32_t balance;
	 Bias=Angle-zhongzhi;             					 //===求出平衡的角度中值
	 balance=balance_KP*Bias+Gyro*balance_KD;   //===计算平衡控制的电机PWM 
	 return balance;
}

/**************************************************************************
函数功能：速度PID控制 修改前进后退速度，请修改Movement的值。
入口参数：角度，左轮编码器、右轮编码器
返回  值：速度控制PWM
**************************************************************************/
int32_t velocity(float Pitch,int32_t encoder_left,int32_t encoder_right)
{  
	static float Velocity,Encoder_Least,Encoder;
	static float Encoder_Integral;
	int16_t Movement=0;
//=============遥控前进后退部分=======================//
	if(Flag_Qian==1)						Movement=4*Flag_sudu;
	else if(Flag_Hou==1)				Movement=-4*Flag_sudu;
	else if(Flag_SS==1)   			
	{
		if(encoder_left>1)				Movement=-4*Flag_sudu;
		else if(encoder_left<-1)	Movement=4*Flag_sudu;
		else if(encoder_left<=2 && encoder_right>=-2)			Flag_SS=0;
	}
//=============速度PI控制器=======================//	
	Encoder_Least =Mean_Filter((encoder_left+encoder_right)-Movement); //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度 
	Encoder *= 0.8;		                                                //===一阶低通滤波器       
	Encoder += Encoder_Least*0.2;	                                    //===一阶低通滤波器    	
	Encoder_Integral +=Encoder;											//===积分出位移 积分时间：10ms
	
//遥控时不允许积分偏移
	if(Flag_Qian==1||Flag_Hou==1|| Flag_Left==1 || Flag_Right==1 ||Flag_SS==1 || Flag_gensui==1)	Encoder_Integral=0;
	if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===积分限幅
	if(Encoder_Integral<-10000)		Encoder_Integral=-10000;            //===积分限幅
	Velocity=Encoder*velocity_KP+Encoder_Integral*velocity_KI;        //===速度控制
	if(Pitch<-40||Pitch>40||Flag_Stop==1) 			Encoder_Integral=0;		//===电机关闭后清除积分
	return Velocity;
}

/**************************************************************************
函数功能：转向PD控制
入口参数：左轮编码器、右轮编码器、Z轴陀螺仪
返回  值：转向控制PWM
**************************************************************************/
int32_t turn(int32_t encoder_left,int32_t encoder_right,float gyro)
{
  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
  float Turn_Amplitude=10*Flag_sudu;//旋转速度调节
	float Kp=70,Kd=0;									//KD为陀螺仪纠正

//=============遥控左右旋转部分=======================//
  if(Flag_Left==1||Flag_Right==1)//这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
	{
		if(++Turn_Count==1)
		Encoder_temp=encoder_left+encoder_right;
		Turn_Convert=50/Encoder_temp;
		if(Turn_Convert<0.6)Turn_Convert=0.6;
		if(Turn_Convert>3)Turn_Convert=3;
	}	
  else
	{
		Turn_Convert=0.9;
		Turn_Count=0;
		Encoder_temp=0;
	}			
	if(Flag_Right==1)	         	 Turn_Target+=Turn_Convert;
	else if(Flag_Left==1)	    	 Turn_Target-=Turn_Convert; 
	else Turn_Target=0;

	if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===转向速度限幅
	if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
	
	if(Flag_Qian==1||Flag_Hou==1)  Kd=1;       		  //转向的时候取消陀螺仪的纠正 有点模糊PID的思想

//=============转向PD控制器=======================//
	Turn=-Turn_Target*Kp -gyro*Kd;                 //===结合Z轴陀螺仪进行PD控制
	return Turn;
}
