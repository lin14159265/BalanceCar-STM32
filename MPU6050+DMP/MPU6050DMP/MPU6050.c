#include "MPU6050.h"

u8 mpu6050_write(u8 addr, u8 reg, u8 len, u8* buf)//����ֵ 0�����ɹ�  -1����ʧ��
{ 
		 unsigned char i;
     addr=addr<<1;                     //ע��dma���ַ���������һλ����Ҫ��λ
		 MPU6050_IIC_Start();              //��������
		 MPU6050_IIC_Send_Byte(addr);      //����������ַ           
		 MPU6050_IIC_Send_Byte(reg);       //���������ӵ�ַ

		 for(i=0;i<len;i++)            
			 MPU6050_IIC_Send_Byte(*buf++);  //��������
		 MPU6050_IIC_Stop();               //��������

		 return 0;
}

u8 mpu6050_read(u8 addr, u8 reg, u8 len, u8 *buf)//����ֵ 0�����ɹ�  -1����ʧ��
{
		 unsigned char i;
     addr=addr<<1;                     //ע��dma���ַ���������һλ����Ҫ��λ
		 MPU6050_IIC_Start();              //��������           
		 MPU6050_IIC_Send_Byte(addr);      //����������ַ            
		 MPU6050_IIC_Send_Byte(reg);       //���������ӵ�ַ

		 MPU6050_IIC_Start();              //������������
		 MPU6050_IIC_Send_Byte(addr+1);
		 for(i=0;i<len-1;i++)  
			 *buf++=MPU6050_IIC_Read_Byte(0);//��������
		 *buf=MPU6050_IIC_Read_Byte(1);
		 MPU6050_IIC_Stop();               //��������
	
		 return 0;
}

void mpu6050_write_reg(u8 reg, u8 dat)
{
   mpu6050_write(SlaveAddress,reg,1,&dat);
}

u8   mpu6050_read_reg (u8 reg)
{
	 u8 dat;
   mpu6050_read(SlaveAddress,reg,1,&dat);
	 return dat;
}

//void MPU6050_Init(void)
//{
//    MPU6050_IIC_IO_Init(); 
// 	
//		mpu6050_write_reg(PWR_MGMT_1,	 0X00);	//����MPU6050 
//    mpu6050_write_reg(SMPLRT_DIV,  0x07); //�����ǲ�����125Hz
//    mpu6050_write_reg(CONFIG,      0x06); //��ͨ�˲�Ƶ��5Hz
//	  mpu6050_write_reg(GYRO_CONFIG, 0x18); //�������Լ켰������Χ:���Լ죬2000deg/s
//	  mpu6050_write_reg(ACCEL_CONFIG,0x01); //���ټ��Լ졢������Χ����ͨ�˲�Ƶ��:���Լ죬2G��5Hz
//}

void MPU6050_Init(void)
{
    MPU6050_IIC_IO_Init(); 
    
    mpu6050_write_reg(PWR_MGMT_1, 0x01);    // ʹ��X����������Ϊʱ��Դ
    mpu6050_write_reg(SMPLRT_DIV, 0x07);    // ������125Hz
    mpu6050_write_reg(CONFIG, 0x06);        // ��ͨ�˲�5Hz
    mpu6050_write_reg(GYRO_CONFIG, 0x18);    // ��2000dps
    mpu6050_write_reg(ACCEL_CONFIG, 0x01);  // ��2g
    
    // �ж����ã�������
    mpu6050_write_reg(INT_PIN_CFG, INT_OPEN_DRAIN | INT_LEVEL); // ��©������͵�ƽ��Ч
    mpu6050_write_reg(INT_ENABLE, DATA_RDY_EN);                 // ʹ�����ݾ����ж�
    mpu6050_read_reg(INT_STATUS);                               // ��������ж�
}


void MPU6050_Interrupt_Config(void)
{
    /* �����ж����� */
    mpu6050_write_reg(INT_PIN_CFG, 
                    0x40 |   // INT���ſ�©���
                    0x20 |   // LATCH_INT_EN=1���ж������ȣ�
                    0x10 |   // INT_RD_CLEAR=1����INT_STATUSʱ����жϣ�
                    0x00);   // ����λ����Ĭ��
    
    /* ʹ�����ݾ����ж� */
    mpu6050_write_reg(INT_ENABLE, 0x01);
    
    /* ��������жϱ�־ */
    mpu6050_read_reg(INT_STATUS);
}


void MPU6050_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* ʹ��ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* ����PB5Ϊ�������� */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ӳ��EXTI��PB5 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

    /* ����EXTI */
    EXTI_InitStruct.EXTI_Line = EXTI_Line5;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* ����NVIC */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


 
