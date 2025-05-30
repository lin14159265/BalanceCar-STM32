/***********************************************
��˾����Ȥ�Ƽ�����ݸ�����޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��5.7
�޸�ʱ�䣺2021-04-29

 
Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update��2021-04-29

All rights reserved
***********************************************/
#include "stm32f10x.h"
#include "sys.h"
u8 Way_Angle=2;                             //��ȡ�Ƕȵ��㷨��1����Ԫ��  2��������  3�������˲� 
u8 Flag_front,Flag_back,Flag_Left,Flag_Right,Flag_velocity=2; //����ң����صı���
u8 Flag_Stop=1,Flag_Show=0;                 //���ֹͣ��־λ����ʾ��־λ  Ĭ��ֹͣ ��ʾ��
int Motor_Left,Motor_Right;                 //���PWM���� Ӧ��Motor�� ��Moto�¾�	
int Temperature;                            //�¶ȱ���
int Voltage;                                //��ص�ѹ������صı���
float Angle_Balance,Gyro_Balance,Gyro_Turn; //ƽ����� ƽ�������� ת��������
u32 Distance;                               //���������
u8 delay_50,delay_flag,PID_Send; 						//��ʱ�͵�����ر���
u8 Flag_follow=0,Flag_avoid=0;							//���������桢���������ϱ�־λ
float Acceleration_Z;                       //Z����ٶȼ�  
float RC_Velocity,RC_Turn_Velocity,Move_X,Move_Z,PS2_ON_Flag;			    //ң�ؿ��Ƶ��ٶ�
float Balance_Kp=25500,Balance_Kd=130,Velocity_Kp=16000,Velocity_Ki=80,Turn_Kp=4200,Turn_Kd=0;//PID�������Ŵ�100����
u8 one_lap_data_success_flag=0;         //�״��������һȦ�Ľ��ձ�־λ
int lap_count=0;//��ǰ�״���һȦ�����ж��ٸ���
int PointDataProcess_count=0,test_once_flag=0,Dividing_point=0;//�״�������ݵ�ļ���ֵ�����յ�һȦ�������һ֡���ݵı�־λ����Ҫ�и����ݵ�������
int main(void)
{ 
  MY_NVIC_PriorityGroupConfig(2);	//�����жϷ���
	delay_init();	    	            //��ʱ������ʼ��	
	JTAG_Set(JTAG_SWD_DISABLE);     //�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //��SWD�ӿ� �������������SWD�ӿڵ���
	LED_Init();                     //��ʼ���� LED ���ӵ�Ӳ���ӿ�
	KEY_Init();                     //������ʼ��
	uart_init(115200);              //����һ��ʼ��
	MiniBalance_PWM_Init(7199,0);   //��ʼ��PWM 10KHZ����Ӳ���ӿڣ������������
	Encoder_Init_TIM3();            //�������ӿ�
	Encoder_Init_TIM4();            //��ʼ��������4
	Adc_Init();
	IIC_Init();                     //IIC��ʼ��
	OLED_Init();                    //OLED��ʼ��
	MPU6050_initialize();           //MPU6050��ʼ��	
	DMP_Init();                     //��ʼ��DMP 
	MiniBalance_EXTI_Init();        //MPU6050 5ms��ʱ�жϳ�ʼ������ʡ��ʱ����Դ������cpu����
	while(1)
	{						
			 oled_show();          			//��ʾ����
		
	}
}

