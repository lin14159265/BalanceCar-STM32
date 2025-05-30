/**
  ******************************************************************************
  * @file    main.c
  * @author  刘亮
  * @version V1.0
  * @date    2021-5-17
  * @brief   MPU6050陀螺仪数据解算（卡尔曼滤波）
  ******************************************************************************
  * 硬件平台	 :STM32F103C8T6系统板+MPU6050模块+0.96寸OLED显示屏+USB转TTL电平工具
  * 联系QQ	 :1920108735
  * 本人能力有限，设计难免有问题和漏洞，欢迎大家和我交流。
  ******************************************************************************
  */ 
#include "main.h"//主函数头文件
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	SYS_Init();//系统初始化总函数
	while(1)//主循环
	{		
		DATA_Report();//MPU6050数据上报
	}
}
/**
  * @brief  系统初始化总函数
  * @param  无
  * @retval 无
  */
void SYS_Init(void)
{
	u8 i;//零时变量定义
	delay_init();  //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//中断优先级分组函数
	uart_init(115200); //串口初始化为115200
	LED_Init();//初始化与LED连接的硬件接口
	OLED_Init();//OLED初始化
	OLED_ColorTurn(0);//OLED反色显示设置
	OLED_DisplayTurn(0);//OLED无反转显示设置
	OLED_ShowString(25,8,(unsigned char*)"MPU6050",24);//文字显示：MPU6050
	for(i = 0;i<8;i++)//文字显示循环
	OLED_ShowChinese(16*i,40,i,16);//文字显示：卡尔曼滤波解算法
	OLED_Refresh();//更新OLED显存
	delay_ms(999);delay_ms(999);//显示延迟
	OLED_Clear();//清屏
	MPU_Init();//MPU6050初始化
	delay_ms(500);//初始化延时
	TIM4_Int_Init(199,7199);//10Khz的计数频率，计数到200为20ms  
	for(i = 0;i<4;i++)//文字显示循环
	OLED_ShowChinese(16*i+32,0,i+17,16);//文字显示：俯仰角
	for(i = 0;i<3;i++)//文字显示循环
	OLED_ShowChinese(16*i+8,16,i+8,16);//文字单次显示
	for(i = 0;i<3;i++)//文字显示循环
	OLED_ShowChinese(16*i+8,32,i+11,16);//文字显示：横滚角
	for(i = 0;i<3;i++)//文字显示循环
	OLED_ShowChinese(16*i+8,48,i+14,16);//文字显示：温度值
	OLED_ShowChinese(96,48,21,16);//文字显示：℃
	OLED_Refresh();//更新OLED显存
}
/**
  * @brief  MPU6050数据上报
  * @param  无
  * @retval 无
  */
void DATA_Report(void)
{
	sprintf((char *)tmp_buf,":%.2f' ",Angle_X_Final);//字符串格式化命令
	OLED_ShowString(56,16,(u8 *)tmp_buf,16);//OLED显示当前解算后俯仰角
	sprintf((char *)tmp_buf,":%.2f' ",Angle_Y_Final);//字符串格式化命令
	OLED_ShowString(56,32,(u8 *)tmp_buf,16);//OLED显示当前解算后横滚角	
	sprintf((char *)tmp_buf,":%.1f",(float)temperature/100.0);//字符串格式化命令
	OLED_ShowString(56,48,(u8 *)tmp_buf,16);//OLED显示当前解算后温度值	
	OLED_Refresh();//更新OLED显存
	printf("Pitch:%.4f Roll:%.4f \
	AAC_X:%5d AAC_Y:%5d AAC_Z:%5d \
	GYRO_X:%5d GYRO_Y:%5d GYRO_Z:%5d\r\n",\
	Angle_X_Final,Angle_Y_Final,aacx,aacy,aacz,\
	gyrox,gyroy,gyroz);//串口发送实时俯仰角，横滚角，XYZ三轴角加速度原始值，XYZ三轴角速度原始值
}
