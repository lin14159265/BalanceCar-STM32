#include "MPU6050.h"

u8 mpu6050_write(u8 addr, u8 reg, u8 len, u8* buf)//返回值 0：读成功  -1：读失败
{ 
		 unsigned char i;
     addr=addr<<1;                     //注意dma库地址不包含最后一位，需要移位
		 MPU6050_IIC_Start();              //启动总线
		 MPU6050_IIC_Send_Byte(addr);      //发送器件地址           
		 MPU6050_IIC_Send_Byte(reg);       //发送器件子地址

		 for(i=0;i<len;i++)            
			 MPU6050_IIC_Send_Byte(*buf++);  //发送数据
		 MPU6050_IIC_Stop();               //结束总线

		 return 0;
}

u8 mpu6050_read(u8 addr, u8 reg, u8 len, u8 *buf)//返回值 0：读成功  -1：读失败
{
		 unsigned char i;
     addr=addr<<1;                     //注意dma库地址不包含最后一位，需要移位
		 MPU6050_IIC_Start();              //启动总线           
		 MPU6050_IIC_Send_Byte(addr);      //发送器件地址            
		 MPU6050_IIC_Send_Byte(reg);       //发送器件子地址

		 MPU6050_IIC_Start();              //重新启动总线
		 MPU6050_IIC_Send_Byte(addr+1);
		 for(i=0;i<len-1;i++)  
			 *buf++=MPU6050_IIC_Read_Byte(0);//发送数据
		 *buf=MPU6050_IIC_Read_Byte(1);
		 MPU6050_IIC_Stop();               //结束总线
	
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
//		mpu6050_write_reg(PWR_MGMT_1,	 0X00);	//唤醒MPU6050 
//    mpu6050_write_reg(SMPLRT_DIV,  0x07); //陀螺仪采样率125Hz
//    mpu6050_write_reg(CONFIG,      0x06); //低通滤波频率5Hz
//	  mpu6050_write_reg(GYRO_CONFIG, 0x18); //陀螺仪自检及测量范围:不自检，2000deg/s
//	  mpu6050_write_reg(ACCEL_CONFIG,0x01); //加速计自检、测量范围及高通滤波频率:不自检，2G，5Hz
//}

void MPU6050_Init(void)
{
    MPU6050_IIC_IO_Init(); 
    
    mpu6050_write_reg(PWR_MGMT_1, 0x01);    // 使用X轴陀螺仪作为时钟源
    mpu6050_write_reg(SMPLRT_DIV, 0x07);    // 采样率125Hz
    mpu6050_write_reg(CONFIG, 0x06);        // 低通滤波5Hz
    mpu6050_write_reg(GYRO_CONFIG, 0x18);    // ±2000dps
    mpu6050_write_reg(ACCEL_CONFIG, 0x01);  // ±2g
    
    // 中断配置（新增）
    mpu6050_write_reg(INT_PIN_CFG, INT_OPEN_DRAIN | INT_LEVEL); // 开漏输出，低电平有效
    mpu6050_write_reg(INT_ENABLE, DATA_RDY_EN);                 // 使能数据就绪中断
    mpu6050_read_reg(INT_STATUS);                               // 清除残留中断
}


void MPU6050_Interrupt_Config(void)
{
    /* 配置中断引脚 */
    mpu6050_write_reg(INT_PIN_CFG, 
                    0x40 |   // INT引脚开漏输出
                    0x20 |   // LATCH_INT_EN=1（中断脉冲宽度）
                    0x10 |   // INT_RD_CLEAR=1（读INT_STATUS时清除中断）
                    0x00);   // 其他位保持默认
    
    /* 使能数据就绪中断 */
    mpu6050_write_reg(INT_ENABLE, 0x01);
    
    /* 清除残留中断标志 */
    mpu6050_read_reg(INT_STATUS);
}


void MPU6050_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* 配置PB5为上拉输入 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 映射EXTI到PB5 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

    /* 配置EXTI */
    EXTI_InitStruct.EXTI_Line = EXTI_Line5;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* 配置NVIC */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


 
