/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：V1.0
修改时间：2023-03-02

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update：2023-03-02

All rights reserved
***********************************************/

#include "Lidar.h"
#include <string.h>

PointDataProcessDef PointDataProcess[420];//更新390个数据
PointDataProcessDef Dataprocess[400];      //用于小车避障、跟随、走直线、ELE雷达避障的雷达数据
PointDataProcessDef TempData[12]={0};  //超过了0度的下一圈数据临时存储
LiDARFrameTypeDef Pack_Data;			//雷达接收的数据储存在这个变量之中

static const uint8_t CrcTable[256] =
{
 0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3,
 0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c, 0xa9, 0xe4, 0x33,
 0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8,
 0xf5, 0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77,
 0x3a, 0x94, 0xd9, 0x0e, 0x43, 0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55,
 0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea, 0x3e, 0x73, 0xa4,
 0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f,
 0x62, 0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff,
 0xb2, 0x1c, 0x51, 0x86, 0xcb, 0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2,
 0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d, 0x88, 0xc5, 0x12,
 0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99,
 0xd4, 0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14,
 0x59, 0xf7, 0xba, 0x6d, 0x20, 0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36,
 0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89, 0x63, 0x2e, 0xf9,
 0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72,
 0x3f, 0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2,
 0xef, 0x41, 0x0c, 0xdb, 0x96, 0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1,
 0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e, 0xeb, 0xa6, 0x71,
 0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa,
 0xb7, 0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35,
 0x78, 0xd6, 0x9b, 0x4c, 0x01, 0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17,
 0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8
};//用于crc校验的数组

/**************************************************************************
Function: LIDAR_USART_Init
Input   : none
Output  : none
函数功能：雷达串口初始化
入口参数: 无 
返回  值：无
**************************************************************************/	 	
//串口2
void LIDAR_USART_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;


	// 打开串口GPIO的时钟
	LIDAR_USART_GPIO_APBxClkCmd(LIDAR_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	LIDAR_USART_APBxClkCmd(LIDAR_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = LIDAR_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LIDAR_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = LIDAR_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LIDAR_USART_RX_GPIO_PORT, &GPIO_InitStructure);	

	//配置USART为中断源 
	NVIC_InitStructure.NVIC_IRQChannel = LIDAR_USART_IRQ;
	//抢断优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	//子优先级 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//使能中断 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化配置NVIC
	NVIC_Init(&NVIC_InitStructure);


	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = LIDAR_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(LIDAR_USARTx, &USART_InitStructure);

	// 使能串口接收中断
	USART_ITConfig(LIDAR_USARTx, USART_IT_RXNE, ENABLE);	

	// 使能串口
	USART_Cmd(LIDAR_USARTx, ENABLE);	    

}

/**************************************************************************
Function: LIDAR_USART_IRQHandler
Input   : none
Output  : none
函数功能：雷达接收中断
入口参数：无
返回  值：无
**************************************************************************/
//串口2
void LIDAR_USART_IRQHandler(void)//接收ld14雷达数据，一帧47个字节
{	
	static u8 state = 0;//状态位	，指示当前数据帧的位置
	static u8 crc = 0;	//校验和
	static u8 cnt = 0;	//用于一帧12个点的计数
	
	u8 temp_data;
	if(USART_GetITStatus(LIDAR_USARTx, USART_IT_RXNE) != RESET) //接收到数据
	{	
		
    USART_ClearITPendingBit(LIDAR_USARTx,USART_IT_RXNE);		
		temp_data=USART_ReceiveData(LIDAR_USARTx); 
#ifdef LD14	
		if (state > 5)	
		{
			if(state < 42)
			{
				if(state%3 == 0)		//一帧数据中的序号为6,9.....39的数据，距离值低8位
				{
					Pack_Data.point[cnt].distance = (u16)temp_data;
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
				}
				else if(state%3 == 1)	//一帧数据中的序号为7,10.....40的数据，距离值高8位
				{
					Pack_Data.point[cnt].distance = ((u16)temp_data<<8)+Pack_Data.point[cnt].distance;
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
				}
				else					//一帧数据中的序号为8,11.....41的数据，置信度
				{
					Pack_Data.point[cnt].confidence = temp_data;
					cnt++;	
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
				}
			}
			else 
			{
				switch(state)
				{
					case 42:
						Pack_Data.end_angle = (u16)temp_data;						//结束角度低8位
						state++;
						crc = CrcTable[(crc^temp_data) & 0xff];
						break;
					case 43:
						Pack_Data.end_angle = ((u16)temp_data<<8)+Pack_Data.end_angle;//结束角度高8位
						state++;
						crc = CrcTable[(crc^temp_data) & 0xff];
						break;
					case 44:
						Pack_Data.timestamp = (u16)temp_data;						//时间戳低8位
						state++;
						crc = CrcTable[(crc^temp_data) & 0xff];
						break;
					case 45:
						Pack_Data.timestamp = ((u16)temp_data<<8)+Pack_Data.timestamp;//时间戳高8位
						state++;
						crc = CrcTable[(crc^temp_data) & 0xff];
						break;
					case 46:
						Pack_Data.crc8 = temp_data;		//雷达传来的校验和
						if(Pack_Data.crc8 == crc)		//校验正确
						{
							LED=~LED;
							data_process();				//接收到一帧且校验正确可以进行数据处理
						}
						else
						{
							memset(&Pack_Data, 0, sizeof(LiDARFrameTypeDef));//清零
						}
						crc = 0;
						state = 0;
						cnt = 0;//复位
						break;
					default: break;
				}
			}
		}
		else 
		{
			switch(state)
			{
				case 0:
					if(temp_data == HEADER)									//头固定
					{
						Pack_Data.header = temp_data;
						state++;
						crc = CrcTable[(crc^temp_data) & 0xff];				//开始进行校验
					} else state = 0,crc = 0;
					break;
				case 1:
					if(temp_data == LENGTH)									//测量的点数，目前固定
					{
						Pack_Data.ver_len = temp_data;
						state++;
						crc = CrcTable[(crc^temp_data) & 0xff];
					} else state = 0,crc = 0;
					break;
				case 2:
					Pack_Data.speed = (u16)temp_data;						//雷达的转速低8位，单位度每秒
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
					break;
				case 3:
					Pack_Data.speed = ((u16)temp_data<<8)+Pack_Data.speed;	//雷达的转速高8位
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
					break;
				case 4:
					Pack_Data.start_angle = (u16)temp_data;					//开始角度低8位，放大了100倍
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
					break;
				case 5:
					Pack_Data.start_angle = ((u16)temp_data<<8)+Pack_Data.start_angle;
					state++;
					crc = CrcTable[(crc^temp_data) & 0xff];
					break;
				default: break;
			}
		}
#endif
	}	
} 
/**************************************************************************
Function: data_process
Input   : none
Output  : none
函数功能：数据处理函数
入口参数：无
返回  值：无
**************************************************************************/
//完成一帧接收后进行处理
void data_process(void)
{
	//雷达每帧数据是固定12个点，雷达一圈大约有xx个点但是不是一个固定值。雷达一秒转大约xx圈，
	//程序是等雷达转完1圈后再将一整圈的数据来使用而不是一帧一帧数据的实时更新而是获取到一整圈雷达数据后再更新。
	//由于雷达每一圈的点数都不是固定的（因为转速不是恒定会有微微小偏差），所以每一圈的倒数第一帧数据很大概率是“前半段数据是当前这一圈的末尾，后半段数据是下一圈的开头”，
	//因此雷达数据处理的时候会根据这一帧数据的开始角度和结束角度判断是否对数据进行切割。
	u16 i,j,m;
	float start_angle = Pack_Data.start_angle/100.0;  //计算12个点的开始角度，数字传输时放大了100倍
	float end_angle = Pack_Data.end_angle/100.0;
	float area_angle[12]={0};//一帧数据的平均角度	
	//180度的地方是车头，做一定的转换使车头方向是0度
	if((start_angle -= 180)<0)
		start_angle += 360;
	if((end_angle -= 180)<0)
		end_angle += 360;
	if(start_angle>end_angle)//开始和结束角度被0度刚好分开的情况
	{
	   end_angle +=360;	
	}
	for(m=0;m<12;m++) //获取每个点的角度值
	{
		area_angle[m]=start_angle+(end_angle-start_angle)/10.5*m;
		if(area_angle[m]>360.0)  //情况1：开始和结束角度被0度刚好分开的情况
		{
			if(test_once_flag==0) //找到第几个点是分界点
		  {
		    test_once_flag=1; //找到了标志位置1
		    Dividing_point=m;  	
			area_angle[m] -=360; //还原点的正确角度值
		   }
		  else area_angle[m] -=360; //还原点的正确角度值
		}
	}
	if ((end_angle <= 360) && (end_angle >= 359) ) //情况2：特殊情况，刚好数据不被0点切割，0点是被两帧数据都夹在中间
	{
	    test_once_flag=1;
		Dividing_point=12;
	  }
	if(test_once_flag) //这帧数据要做数据切割的情况（这帧数据包含了这一圈的数据和下一圈的数据）
	{
		
		 for(i=0;i<Dividing_point;i++)    //这一帧数据360度之前的点正常保留
		 {
			  PointDataProcess[PointDataProcess_count+i].angle = area_angle[i];
			  PointDataProcess[PointDataProcess_count+i].distance = Pack_Data.point[i].distance;
		 }
		 PointDataProcess_count=PointDataProcess_count+Dividing_point;
		  for(j=0;j<12-Dividing_point;j++)    //把下一圈数据存放在临时存储数组里
		 {
			TempData[j].angle = area_angle[j+Dividing_point];
			TempData[j].distance = Pack_Data.point[j+Dividing_point].distance;
		 }
		 for(m=0;m<PointDataProcess_count;m++)    //这时一整圈的数据已经收集完成，把一整圈数据复制到另一个数组进行使用
		 {
			 Dataprocess[m].angle=PointDataProcess[m].angle;
			 Dataprocess[m].distance = PointDataProcess[m].distance;
		 }
		 lap_count=PointDataProcess_count;//获取当前的这一圈有多少个点
		 one_lap_data_success_flag=1;//一圈数据更新完成
		 test_once_flag=0; //标志位清零，下一帧数据必定不需要做数据切割
		 PointDataProcess_count=0;//计算第几个点的计算变量清零
	}
 else//这组数据不需要做数据切割
 {
	  	for(i=0;i<12;i++)    //把一帧数据存放在distance_sum数组里
	    {
			if(one_lap_data_success_flag) //如果上一帧数据做了切割，那么就要先把之前临时存储的数据取出来放在这一圈数据的最前面
			 {
				 for(j=0;j<12-Dividing_point;j++)    
				 {
					PointDataProcess[j].angle = TempData[j].angle;
					PointDataProcess[j].distance = TempData[j].distance; 
				 }
				one_lap_data_success_flag=0;//现在是新的一圈数据，置零
				PointDataProcess_count=PointDataProcess_count+(12-Dividing_point);//累加
				Dividing_point=0;//判断切割点的数清零
			 }
     	  PointDataProcess[PointDataProcess_count+i].angle = area_angle[i];
		  PointDataProcess[PointDataProcess_count+i].distance = Pack_Data.point[i].distance;
	   }
	   PointDataProcess_count=PointDataProcess_count+12; //累加12
 }
}

