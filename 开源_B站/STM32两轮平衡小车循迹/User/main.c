#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "motor.h"
#include "encoder.h"
#include "control.h"
#include "sensor.h"
#include "PID.h"
#include "math.h"

float KP=650,KD=7300,Bias=0,BiasL=0,limv=0.8,maxv=1,v;
float kpup=-80,kdup=27,kpv=0.8 ,kiv=0/200,kt=0;
float p0=4,a=0.7;
float p,pa,GY,GZ,err,err0;
int16_t errmax=15,error=0,error0=0;
int16_t outv,outup,outt,pwmout,pwmoutz,pwmouty; 
int16_t ez,ey,ax,ay,az,gx,gy,gz,num=0;
int16_t ee=0,es=0,lowout_last=0,lowout=0;
int16_t circle=0,flag=0,turn=0,flagt1=0,flagt2=0;
int32_t jc;
#define pi 3.1415927  
#define lb 0.95238    

int main(void){
	ALL_Init();
	MPU6050_Angle_Init();
	while(1)
	{
		MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz);
		Sensor_JC();
		ez=Encoder_Z_Get();
		ey=Encoder_Y_Get();
		Control_v();
		Control_turn();
	}
}

void TIM1_UP_IRQHandler(void){

	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		MPU6050_Angle();
		Control_up();
		pwmout=outup+kpup*outv;
		pwmoutz=pwmout-outt;
		pwmouty=pwmout+outt;
		Control_Limit();
		Motor_Z_SetSpeed(pwmoutz);
		Motor_Y_SetSpeed(pwmouty);
	}
}
