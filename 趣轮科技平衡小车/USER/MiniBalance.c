/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：5.7
修改时间：2021-04-29

 
Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update：2021-04-29

All rights reserved
***********************************************/
#include "stm32f10x.h"
#include "sys.h"
u8 Way_Angle=2;                             //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波 
u8 Flag_front,Flag_back,Flag_Left,Flag_Right,Flag_velocity=2; //蓝牙遥控相关的变量
u8 Flag_Stop=1,Flag_Show=0;                 //电机停止标志位和显示标志位  默认停止 显示打开
int Motor_Left,Motor_Right;                 //电机PWM变量 应是Motor的 向Moto致敬	
int Temperature;                            //温度变量
int Voltage;                                //电池电压采样相关的变量
float Angle_Balance,Gyro_Balance,Gyro_Turn; //平衡倾角 平衡陀螺仪 转向陀螺仪
u32 Distance;                               //超声波测距
u8 delay_50,delay_flag,PID_Send; 						//延时和调参相关变量
u8 Flag_follow=0,Flag_avoid=0;							//超声波跟随、超声波壁障标志位
float Acceleration_Z;                       //Z轴加速度计  
float RC_Velocity,RC_Turn_Velocity,Move_X,Move_Z,PS2_ON_Flag;			    //遥控控制的速度
float Balance_Kp=25500,Balance_Kd=130,Velocity_Kp=16000,Velocity_Ki=80,Turn_Kp=4200,Turn_Kd=0;//PID参数（放大100倍）
u8 one_lap_data_success_flag=0;         //雷达数据完成一圈的接收标志位
int lap_count=0;//当前雷达这一圈数据有多少个点
int PointDataProcess_count=0,test_once_flag=0,Dividing_point=0;//雷达接收数据点的计算值、接收到一圈数据最后一帧数据的标志位、需要切割数据的数据数
int main(void)
{ 
  MY_NVIC_PriorityGroupConfig(2);	//设置中断分组
	delay_init();	    	            //延时函数初始化	
	JTAG_Set(JTAG_SWD_DISABLE);     //关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //打开SWD接口 可以利用主板的SWD接口调试
	LED_Init();                     //初始化与 LED 连接的硬件接口
	KEY_Init();                     //按键初始化
	uart_init(115200);              //串口一初始化
	MiniBalance_PWM_Init(7199,0);   //初始化PWM 10KHZ与电机硬件接口，用于驱动电机
	Encoder_Init_TIM3();            //编码器接口
	Encoder_Init_TIM4();            //初始化编码器4
	Adc_Init();
	IIC_Init();                     //IIC初始化
	OLED_Init();                    //OLED初始化
	MPU6050_initialize();           //MPU6050初始化	
	DMP_Init();                     //初始化DMP 
	MiniBalance_EXTI_Init();        //MPU6050 5ms定时中断初始化，节省定时器资源，减少cpu负担
	while(1)
	{						
			 oled_show();          			//显示屏打开
		
	}
}

