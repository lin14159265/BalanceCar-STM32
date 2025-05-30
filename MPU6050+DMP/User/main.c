#include "stm32f10x.h"                  // Device header"
#include "OLED.h"
#include "Delay.h"
#include "inv_mpu.h"
#include "MPU6050.h"


float Pitch,Roll,Yaw;
volatile uint8_t data_ready = 0;


int main(void)
{
	delay_init();
	OLED_Init();
	MPU6050_Init();
	MPU6050_Interrupt_Config();
	MPU6050_DMP_Init();
	MPU6050_EXTI_Init();
	
	
	while(1)
	{
		if(data_ready)
        {
            MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw);
            OLED_ShowString(1,1,"Pitch:");
            OLED_ShowFloat(1, 7, Pitch, 5);
            OLED_ShowString(2,1,"Roll:");
            OLED_ShowFloat(2, 6, Roll, 5);
            OLED_ShowString(3,1,"Yaw:");
            OLED_ShowFloat(3, 5, Yaw, 5);
            data_ready = 0;
        }


	}
}


void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
        data_ready = 1;
    }
}
