#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "kalman.h"
#include "led.h"
//正点原子定时器驱动函数库

void TIM4_Int_Init(u16 arr,u16 psc);
 
#endif
