#include "stm32f10x.h"                  // Device header

extern int16_t errmax,error,error0;
extern int32_t jc;
extern float limv,maxv,v;
extern float KP,KD,outt,Bias,BiasL;

void PID_XunJi(void)
	{
	switch (jc)
	{
		case 10000000:  error=-13; v=maxv; break; 
		case 11000000:  error=-11; v=maxv; break; 
		case  1000000:  error=-9;  v=maxv; break; 
		case  1100000:  error=-7;  v=maxv; break; 
		case   100000:  error=-5;  v=maxv; break; 
		case   110000:  error=-3;  v=maxv; break; 
		case    10000:  error=-1;  v=maxv; break; 
		
		/*左直角转弯*/
		case  1110000:  error=-errmax; v=limv; break; 
		case 11111100:  error=-errmax; v=limv; break; 
		case 11111000:  error=-errmax; v=limv; break; 
		case 11110000:  error=-errmax; v=limv; break; 
		case 11100000:  error=-errmax; v=limv; break;
		
		/*直行*/
		case    11000:  error=0;   v=maxv; break;		
		
		/*右弯*/
		case     1000:  error=1;   v=maxv; break; 
		case     1100:  error=3;   v=maxv; break; 
		case      100:  error=5;   v=maxv; break; 
		case      110:  error=7;   v=maxv; break; 
		case       10:  error=9;   v=maxv; break; 
		case       11:  error=11;  v=maxv; break; 
		case        1:  error=13;  v=maxv; break; 
		
		/*右直角转弯*/
		case      111:  error=errmax;  v=limv; break; 
		case     1111:  error=errmax;  v=limv; break; 
		case    11111:  error=errmax;  v=limv; break; 
		case   111111:  error=errmax;  v=limv; break;
		case     1110:  error=errmax;  v=limv; break;
   }	
}
void PID_OUT(void)//PWM计算输出（转向环PD）
{
	Bias=error-error0;//计算偏差
	outt=KP*Bias+KD*(Bias-BiasL);//位置式PID控制器
	BiasL=Bias;//保存上一次偏差 
}

