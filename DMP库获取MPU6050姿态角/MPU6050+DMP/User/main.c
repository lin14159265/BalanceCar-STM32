#include "stm32f10x.h"                  // Device header"
#include "OLED.h"
#include "Delay.h"
#include "inv_mpu.h"

float Pitch,Roll,Yaw;

int main(void)
{
	delay_init();
	OLED_Init();
	MPU6050_DMP_Init();
	
	while(1)
	{
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);
		OLED_ShowFloat(1,1,Pitch,3);
		OLED_ShowFloat(2,1,Roll,3);
		OLED_ShowFloat(3,1,Yaw,3);


	}
}
