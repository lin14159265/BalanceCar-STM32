#include "stm32f10x.h"
#include "MPU6050.h"
#include "Attitude.h"
#include "Delay.h"
#include "OLED.h"

Attitude attitude;
float dt = 0.005f; // 5ms采样周期

	
int main()
{
	MPU6050_Init();
	Attitude_Init(&attitude, dt);
	OLED_Init();
	
	OLED_ShowString(1,1,"roll:");
	OLED_ShowString(2,1,"pitch:");
	
	while(1)
	{
		int16_t accel[3], gyro[3];
		MPU6050_GetData(&accel[0], &accel[1], &accel[2], 
                       &gyro[0], &gyro[1], &gyro[2]);
		Attitude_Update(&attitude, accel, gyro);
		
		float roll = Get_Roll(&attitude);
        float pitch = Get_Pitch(&attitude);
		
		OLED_ShowFloat(1,6,roll,8);
		OLED_ShowFloat(2,7,pitch,8);
		
	}
}
