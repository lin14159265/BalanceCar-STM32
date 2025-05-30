#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "sys.h"
#include "MPU6050_I2C.h"

#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG		  	0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B //��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I	  	0x75	
#define	SlaveAddress	0x68 //IIC��ַ�Ĵ���0x68

#define INT_PIN_CFG     0x37   // �ж��������üĴ���
#define INT_ENABLE      0x38   // �ж�ʹ�ܼĴ���
#define INT_STATUS      0x3A   // �ж�״̬�Ĵ���


// �ж����ò���
#define INT_LEVEL       0x80   // �ж����ŵ͵�ƽ��Ч
#define INT_OPEN_DRAIN  0x40   // ��©���
#define INT_RD_CLEAR    0x10   // ��ȡ״̬�Ĵ���ʱ����ж�
#define DATA_RDY_EN     0x01   // ���ݾ����ж�ʹ��

u8   mpu6050_write(u8 addr, u8 reg, u8 len, u8* buf);//����ֵ 0�����ɹ�  -1����ʧ��
u8   mpu6050_read (u8 addr, u8 reg, u8 len, u8 *buf);//����ֵ 0�����ɹ�  -1����ʧ��
void mpu6050_write_reg(u8 reg, u8 dat);
u8   mpu6050_read_reg (u8 reg);
void MPU6050_Init(void);

void MPU6050_Interrupt_Config(void);      // MPU6050�ж�����
void MPU6050_EXTI_Init(void);             // �ⲿ�жϳ�ʼ��

#endif
