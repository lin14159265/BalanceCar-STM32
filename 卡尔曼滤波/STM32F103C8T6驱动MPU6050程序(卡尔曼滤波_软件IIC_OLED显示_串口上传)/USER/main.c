/**
  ******************************************************************************
  * @file    main.c
  * @author  ����
  * @version V1.0
  * @date    2021-5-17
  * @brief   MPU6050���������ݽ��㣨�������˲���
  ******************************************************************************
  * Ӳ��ƽ̨	 :STM32F103C8T6ϵͳ��+MPU6050ģ��+0.96��OLED��ʾ��+USBתTTL��ƽ����
  * ��ϵQQ	 :1920108735
  * �����������ޣ���������������©������ӭ��Һ��ҽ�����
  ******************************************************************************
  */ 
#include "main.h"//������ͷ�ļ�
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	SYS_Init();//ϵͳ��ʼ���ܺ���
	while(1)//��ѭ��
	{		
		DATA_Report();//MPU6050�����ϱ�
	}
}
/**
  * @brief  ϵͳ��ʼ���ܺ���
  * @param  ��
  * @retval ��
  */
void SYS_Init(void)
{
	u8 i;//��ʱ��������
	delay_init();  //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ж����ȼ����麯��
	uart_init(115200); //���ڳ�ʼ��Ϊ115200
	LED_Init();//��ʼ����LED���ӵ�Ӳ���ӿ�
	OLED_Init();//OLED��ʼ��
	OLED_ColorTurn(0);//OLED��ɫ��ʾ����
	OLED_DisplayTurn(0);//OLED�޷�ת��ʾ����
	OLED_ShowString(25,8,(unsigned char*)"MPU6050",24);//������ʾ��MPU6050
	for(i = 0;i<8;i++)//������ʾѭ��
	OLED_ShowChinese(16*i,40,i,16);//������ʾ���������˲����㷨
	OLED_Refresh();//����OLED�Դ�
	delay_ms(999);delay_ms(999);//��ʾ�ӳ�
	OLED_Clear();//����
	MPU_Init();//MPU6050��ʼ��
	delay_ms(500);//��ʼ����ʱ
	TIM4_Int_Init(199,7199);//10Khz�ļ���Ƶ�ʣ�������200Ϊ20ms  
	for(i = 0;i<4;i++)//������ʾѭ��
	OLED_ShowChinese(16*i+32,0,i+17,16);//������ʾ��������
	for(i = 0;i<3;i++)//������ʾѭ��
	OLED_ShowChinese(16*i+8,16,i+8,16);//���ֵ�����ʾ
	for(i = 0;i<3;i++)//������ʾѭ��
	OLED_ShowChinese(16*i+8,32,i+11,16);//������ʾ�������
	for(i = 0;i<3;i++)//������ʾѭ��
	OLED_ShowChinese(16*i+8,48,i+14,16);//������ʾ���¶�ֵ
	OLED_ShowChinese(96,48,21,16);//������ʾ����
	OLED_Refresh();//����OLED�Դ�
}
/**
  * @brief  MPU6050�����ϱ�
  * @param  ��
  * @retval ��
  */
void DATA_Report(void)
{
	sprintf((char *)tmp_buf,":%.2f' ",Angle_X_Final);//�ַ�����ʽ������
	OLED_ShowString(56,16,(u8 *)tmp_buf,16);//OLED��ʾ��ǰ���������
	sprintf((char *)tmp_buf,":%.2f' ",Angle_Y_Final);//�ַ�����ʽ������
	OLED_ShowString(56,32,(u8 *)tmp_buf,16);//OLED��ʾ��ǰ���������	
	sprintf((char *)tmp_buf,":%.1f",(float)temperature/100.0);//�ַ�����ʽ������
	OLED_ShowString(56,48,(u8 *)tmp_buf,16);//OLED��ʾ��ǰ������¶�ֵ	
	OLED_Refresh();//����OLED�Դ�
	printf("Pitch:%.4f Roll:%.4f \
	AAC_X:%5d AAC_Y:%5d AAC_Z:%5d \
	GYRO_X:%5d GYRO_Y:%5d GYRO_Z:%5d\r\n",\
	Angle_X_Final,Angle_Y_Final,aacx,aacy,aacz,\
	gyrox,gyroy,gyroz);//���ڷ���ʵʱ�����ǣ�����ǣ�XYZ����Ǽ��ٶ�ԭʼֵ��XYZ������ٶ�ԭʼֵ
}
