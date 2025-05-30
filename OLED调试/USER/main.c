#include "stm32f10x.h"
#include "delay.h"         // 延时函数
#include "oled.h"          // OLED显示

/*********************
主函数
功能：系统初始化与主循环
*********************/
int main(void)	
{
    /* 基础初始化 */
    delay_init();            // 延时函数初始化

    /* 外设初始化 */
    OLED_Init();             // OLED显示屏初始化
    OLED_Clear();

    /* 显示初始化 */
    OLED_ShowString(0, 3, "jiao du:", 12);  // 第3行显示"角度："
    OLED_ShowString(0, 4, "ju li:", 12);    // 第4行显示"距离："
    OLED_ShowString(0, 5, "sd:", 12);       // 第5行显示"速度："

    /* 主循环 */
    while(1)	
    {


    }
}
