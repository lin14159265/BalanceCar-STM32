#include "sys.h"//ϵͳ�жϷ����
#include "delay.h"//��ʱ������
#include "usart.h"//�������ÿ�
#include "stdio.h"//��׼���������
#include "string.h"//�ַ�����
#include "LED.h"//LED������
#include "oled.h"//OLED������
#include "TIMER.h"//��ʱ��������
#include "mpu6050.h"//MPU6050������
#include "kalman.h"//���������㷨��

u8 tmp_buf[20];				//�ַ�������

int main(void);				//������
void SYS_Init(void);		//ϵͳ��ʼ���ܺ���	
void DATA_Report(void);		//MPU6050�����ϱ�	
