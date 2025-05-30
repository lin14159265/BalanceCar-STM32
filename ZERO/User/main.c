#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "inv_mpu.h"
float pitch,roll,yaw;
int16_t gx,gy,gz,ax,ay,az;
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	MPU6050_DMP_Init();

	while (1)
	{
		MPU6050_DMP_Get_Data(&pitch,&roll,&yaw);
		MPU_Get_Gyroscope(&gx,&gy,&gz);
		MPU_Get_Accelerometer(&ax,&ay,&az);

		
		OLED_ShowSignedNum(2, 1, pitch, 5);				
		OLED_ShowSignedNum(3, 1, roll, 5);
		OLED_ShowSignedNum(4, 1, yaw, 5);
		OLED_ShowSignedNum(2, 8, gx, 5);
		OLED_ShowSignedNum(3, 8, gy, 5);
		OLED_ShowSignedNum(4, 8, gz, 5);
	}
}
