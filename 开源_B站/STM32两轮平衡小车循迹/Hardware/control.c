#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "motor.h"
#include "encoder.h"
#include "Timer.h"


extern float kpup,kdup,kpv,kiv,kt,a,p0;
extern int16_t es,lowout_last,ee,lowout;
extern int16_t outv,outup,outt,pwmout,pwmoutz,pwmouty;
extern int16_t ez,ey,ax,ay,az,gx,gy,gz;
extern float p,r,y,pa,GY,GZ,err,error;

void ALL_Init(void){
	OLED_Init();		
	MPU6050_Init();
	Motor_Z_Init();
	Motor_Y_Init();
	Encoder_Z_Init();
	Encoder_Y_Init();
	Timer_Init();
}
void Control_up(void){
	outup=kpup*(p-p0)+kdup*(GY-0);
}
void Control_v(void){
	ee=(ez+ey)-0;
	lowout=(1-a)*ee+a*lowout_last;
	lowout_last=lowout;
	es+=ee;
	if(es>=10000) es=10000;
	if(es<=-10000) es=-10000;
	outv=kpv*ee+kiv*es;
}
void Control_turn(void){
	outt=kt*GZ+err;
}
void Control_Limit(void){
	if(pwmoutz>=7200)  pwmoutz=7200;
	if(pwmoutz<=-7200) pwmoutz=-7200;
	if(pwmouty>=7200)  pwmouty=7200;
	if(pwmouty<=-7200) pwmouty=-7200;
}

