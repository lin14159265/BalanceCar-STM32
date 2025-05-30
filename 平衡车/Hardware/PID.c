#include "stm32f10x.h"                  // Device header
#define FILTERING_TIMES  10

extern float balance_KP,balance_KD;
extern float velocity_KP,velocity_KI;
extern uint8_t Flag_Qian,Flag_Hou,Flag_Left,Flag_Right;	//��������ң��
extern uint8_t Flag_Stop,Flag_sudu,Flag_gensui,Flag_SS,Flag_ON; //�������Ʊ�־λ
	
/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������ -70,+89
**************************************************************************/
uint8_t Stop_Off(float angle)
{
	uint8_t temp; //0 ������־ 1 ֹͣ���б�־
	if(((angle<-40||angle>40)||Flag_Stop==1) && Flag_ON==0 )		//===��Ǵ���40�ȹرյ��
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
�������ܣ��ٶ��˲�
��ڲ������ٶ�
����  ֵ���˲�����ٶ�
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
�������ܣ�ֱ��PID����
��ڲ������Ƕȣ���е��ֵ�����ٶ�
����  ֵ��ֱ������PWM
**************************************************************************/
int32_t balance(float Angle,float zhongzhi,float Gyro)
{  
	 float Bias;
	 int32_t balance;
	 Bias=Angle-zhongzhi;             					 //===���ƽ��ĽǶ���ֵ
	 balance=balance_KP*Bias+Gyro*balance_KD;   //===����ƽ����Ƶĵ��PWM 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PID���� �޸�ǰ�������ٶȣ����޸�Movement��ֵ��
��ڲ������Ƕȣ����ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
**************************************************************************/
int32_t velocity(float Pitch,int32_t encoder_left,int32_t encoder_right)
{  
	static float Velocity,Encoder_Least,Encoder;
	static float Encoder_Integral;
	int16_t Movement=0;
//=============ң��ǰ�����˲���=======================//
	if(Flag_Qian==1)						Movement=4*Flag_sudu;
	else if(Flag_Hou==1)				Movement=-4*Flag_sudu;
	else if(Flag_SS==1)   			
	{
		if(encoder_left>1)				Movement=-4*Flag_sudu;
		else if(encoder_left<-1)	Movement=4*Flag_sudu;
		else if(encoder_left<=2 && encoder_right>=-2)			Flag_SS=0;
	}
//=============�ٶ�PI������=======================//	
	Encoder_Least =Mean_Filter((encoder_left+encoder_right)-Movement); //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶ� 
	Encoder *= 0.8;		                                                //===һ�׵�ͨ�˲���       
	Encoder += Encoder_Least*0.2;	                                    //===һ�׵�ͨ�˲���    	
	Encoder_Integral +=Encoder;											//===���ֳ�λ�� ����ʱ�䣺10ms
	
//ң��ʱ���������ƫ��
	if(Flag_Qian==1||Flag_Hou==1|| Flag_Left==1 || Flag_Right==1 ||Flag_SS==1 || Flag_gensui==1)	Encoder_Integral=0;
	if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===�����޷�
	if(Encoder_Integral<-10000)		Encoder_Integral=-10000;            //===�����޷�
	Velocity=Encoder*velocity_KP+Encoder_Integral*velocity_KI;        //===�ٶȿ���
	if(Pitch<-40||Pitch>40||Flag_Stop==1) 			Encoder_Integral=0;		//===����رպ��������
	return Velocity;
}

/**************************************************************************
�������ܣ�ת��PD����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
**************************************************************************/
int32_t turn(int32_t encoder_left,int32_t encoder_right,float gyro)
{
  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
  float Turn_Amplitude=10*Flag_sudu;//��ת�ٶȵ���
	float Kp=70,Kd=0;									//KDΪ�����Ǿ���

//=============ң��������ת����=======================//
  if(Flag_Left==1||Flag_Right==1)//��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
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

	if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
	
	if(Flag_Qian==1||Flag_Hou==1)  Kd=1;       		  //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��

//=============ת��PD������=======================//
	Turn=-Turn_Target*Kp -gyro*Kd;                 //===���Z�������ǽ���PD����
	return Turn;
}
