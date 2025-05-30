//////////////////////////////////////////////////////////////////////////////////
// 防止头文件重复包含的预处理指令
#ifndef __OLED_H 
#define __OLED_H			  

// 包含系统级头文件
#include "sys.h"        // 微控制器系统配置头文件
#include "stdlib.h"     // 标准库函数，可能用于动态内存操作

// OLED 基础配置宏定义
#define OLED_MODE 0     // 通信模式选择（0=I2C模式，1=SPI模式或其他）
#define SIZE 8          // 默认字体尺寸（可能指8x8像素）
#define XLevelL 0x00    // OLED显存X轴地址低位
#define XLevelH 0x10    // OLED显存X轴地址高位
#define Max_Column 128  // OLED屏幕最大列数（横向分辨率）
#define Max_Row 64      // OLED屏幕最大行数（纵向分辨率）
#define Brightness 0xFF // 屏幕亮度设置（FF=最大亮度）
#define X_WIDTH 128     // 屏幕X轴像素数
#define Y_WIDTH 64      // 屏幕Y轴像素数

//-----------------OLED IIC 硬件引脚配置----------------
// 使用GPIOA的Pin2和Pin3作为I2C通信引脚
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_2) // SCL时钟线置低
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_2)    // SCL时钟线置高

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_3) // SDA数据线置低
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_3)   // SDA数据线置高

// 数据传输模式定义
#define OLED_CMD 0   // 命令传输模式
#define OLED_DATA 1  // 数据传输模式

//-----------------OLED 功能函数声明----------------
// 基础通信函数
void OLED_WR_Byte(unsigned dat, unsigned cmd);  // 向OLED写入单字节数据/命令
void OLED_IIC_Start(void);                      // I2C通信起始信号
void OLED_IIC_Stop(void);                       // I2C通信终止信号
void OLED_Write_IIC_Command(unsigned char IIC_Command); // 发送命令字节
void OLED_Write_IIC_Data(unsigned char IIC_Data);        // 发送数据字节
void OLED_Write_IIC_Byte(unsigned char IIC_Byte);        // 发送单字节（内部用）
void OLED_IIC_Wait_Ack(void);                   // 等待ACK信号

// 屏幕控制函数
void OLED_Display_On(void);     // 开启屏幕显示
void OLED_Display_Off(void);    // 关闭屏幕显示（休眠模式）
void OLED_Init(void);           // OLED初始化配置
void OLED_Clear(void);          // 清空屏幕内容

// 图形绘制函数
void OLED_DrawPoint(u8 x, u8 y, u8 t);  // 在(x,y)坐标绘制像素点（t=1亮/0灭）
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot); // 矩形区域填充（dot=填充模式）
void OLED_Set_Pos(unsigned char x, unsigned char y); // 设置显存写入起始位置

// 字符显示函数
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size);  // 显示单个ASCII字符
void OLED_ShowString(u8 x, u8 y, u8 *p, u8 Char_Size); // 显示字符串
void OLED_ShowCHinese(u8 x, u8 y, u8 no);              // 显示中文字符（需字库支持）

// 数字显示函数
void OLED_ShowNumber(u8 x, u8 y, u32 num, u8 len, u8 size); // 显示定长整数
void OLED_Num_write(unsigned char x, unsigned char y, unsigned char asc); // 显示单数字
void OLED_Num2(u8 x, u8 y, int number);    // 显示2位整数
void OLED_Num3(u8 x, u8 y, int number);    // 显示3位整数
void OLED_Num4(u8 x, u8 y, int number);    // 显示4位整数
void OLED_Num5(u8 x, u8 y, unsigned int number); // 显示5位无符号整数

// 特殊符号与高级显示
void OLED_fuhao_write(u8 x, u8 y, u8 asc);       // 显示特殊符号
void OLED_Float(u8 Y, u8 X, double real, u8 N);  // 显示浮点数（N位整数部分）
void OLED_Float2(u8 Y, u8 X, double real, u8 N1, u8 N2); // 浮点数显示（N1整数位，N2小数位）

// 图像显示函数
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]); // 显示位图图像
void fill_picture(u8 fill_Data);       // 全屏填充特定模式
void Picture(void);                   // 预定义图片显示函数

#endif // 结束头文件保护
